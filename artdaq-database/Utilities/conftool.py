#!/usr/bin/env python

# using python 2.7
# ~/conftool.py list_databases '{"operation" : "listdatabases", "dataformat":"csv", "filter":{}}'
from __future__ import print_function
import sys
import inspect
import conftoolp
import json
import re
import fnmatch
import os
import shutil
import time
import subprocess
import socket
import hashlib

fhicl_schema = 'schema.fcl'
flags_file = 'flags.fcl'

any_file_pattern = '*.*'
fcl_file_pattern = '*.fcl'

artdaq_database_uri = None


def __copy_default_schema():
    if os.path.isfile(fhicl_schema):
        return

    schema = 'schema.fcl'

    try:
        schema = os.environ['ARTDAQ_DATABASE_CONFDIR']+'/' + fhicl_schema
    except KeyError:
        schema = os.path.dirname(os.path.realpath(__file__))+'/../conf' + fhicl_schema

    if not os.path.isfile(schema):
        print ('Error: Cannot open {}. File not found.'.format(schema))
        sys.exit(1)

    shutil.copyfile(schema, fhicl_schema)
    print ('Info: Copied ' + schema + ' to ' + fhicl_schema)


def __report_error(result):
    print ('Error:' + result[1], file=sys.stderr)


def __allow_importing_incomplete_configurations():
    try:
        allow_incomplete = os.environ['ARTDAQ_DATABASE_ALLOW_INCOMPLETE_CONFIGURATIONS'].lower()
        if allow_incomplete == 'true':
            return True
    except KeyError:
        return False

    return False


def __remove_run(config):
    has_run = re.match(r'^\d+/(.*)', config)
    return has_run.group(1) if has_run else config


def __get_prefix(config):
    config = __remove_run(config)
    match = re.match(r'(.*[^\d])(\d+$)', config)
    return match.group(1) if match else config


def __ends_on_5digitnumber(config):
    match = re.match(r'.*[^\d](\d{5}$)', config)
    return True if match else False


def __increment_config_name(config):
    config = __remove_run(config)
    match = re.match(r'(.*[^\d])(\d+$)', config)
    return match.group(1) + str(int(match.group(2)) + 1).zfill(5) if match else config+str(1).zfill(5)


def __archiveuri_from_uri(uri):
    match = re.match(r'.*://.*/(.*?(?=(?:\?)|$))', uri)

    if not match.group(1):
        print('Error: Invalid URI ', uri)
        sys.exit(1)

    return uri.replace(match.group(1), match.group(1)+'_archive')


def __latest_config_name(configNamePrefix):
    query = json.loads('{"operation" : "findconfigs", "dataformat":"gui", "filter":{}}')
    query['filter']['configurations.name'] = configNamePrefix+'*'
    result = conftoolp.find_configurations(json.dumps(query))

    if result[0] is not True:
        __report_error(result)
        return None

    configs = [c['name'] for c in json.loads(result[1])['search']]

    if not configs:
        return None

    if __ends_on_5digitnumber(configNamePrefix):
        configs = [config for config in configs if configNamePrefix == config]
    else:
        configs = [config for config in configs if re.match(r'(^'+configNamePrefix+r')(\d{5}$)', config)]

    if not configs:
        return None

    configs.sort()

    return configs[-1]


def __read_document(query):
    query['dataformat'] = 'origin'

    result = conftoolp.read_document(json.dumps(query))

    if result[0] is not True:
        __report_error(result)
        return (None, None)

    return (query['filter']['entities.name'], result[1], query['collection'])


def __write_document(query, document):
    result = conftoolp.write_document(json.dumps(query), document)

    if result[0] is not True:
        __report_error(result)

    return result


def __find_files(directory, pattern):
    for root, dirs, files in os.walk(directory):
        for basename in files:
            if fnmatch.fnmatch(basename, pattern):
                filename = os.path.join(root, basename)
                yield filename


def __read_fhicl_schema():
    with open(fhicl_schema, 'r') as fhicl_file:
        return fhicl_file.read()


def __read_schema():
    layout_fhicl = __read_fhicl_schema()
    fhicl_file = fhicl_schema
    result = conftoolp.fhicl_to_json(layout_fhicl, fhicl_file)
    if result[0] is not True:
        __report_error(result)
        return None

    layout_json = json.loads(result[1])
    return layout_json['document']['data']['main']


def __validate_schema(schema):
    def to_string(dict):
        result = '{ '
        for key in dict:
            result += key + ':' + dict[key] + ' '
        result += '}'
        return result

    errors_count = 0

    for l in ['artdaq_processes', 'artdaq_includes', 'system_layout', 'run_history']:
        if l not in schema:
            print ('Error: Invalid schema.fcl; A top level ' + l + ' sequence is missing.')
            errors_count += 1
            continue

        if isinstance(schema[l], list) is not True:
            print ('Error: Invalid schema.fcl; A top level ' + l + ' is not a sequence type.')
            errors_count += 1
            continue

        idx = -1

        for d in schema[l]:
            idx += 1
            if isinstance(d, dict) is not True:
                print ('Error: Invalid schema.fcl; A top level ' + l + ' is not a sequence of tables type.')
                errors_count += 1
                break

            error_message = 'Error: Schema rule ' + l + ':['+to_string(d) + '] '

            if 'collection' not in d or 'pattern' not in d:
                print (error_message + 'is missing either "collection" or "pattern" key-value pairs.')
                errors_count += 1
                continue

            r = d['pattern']

            try:
                if re.compile(r).groups < 3:
                    print (
                        error_message +
                        ' has an invalid regular expression in the "pattern" key-value pair.' +
                        '\n Regular expressions must have at least three capturing groups, example : (.*)(metadata)(\.fcl$) or (.*)(ssp\d+(_hw_cfg|))(\.fcl$).' +
                        '\n The second capturing group is used as a default value for the "entity" key-value pair, e.g. entity:"match.group(2)"')
                    errors_count += 1
                    continue
            except re.error:
                print (error_message + ' has an invalid regular expression in the "pattern" key-value pair.')
                errors_count += 1
                continue

            if 'entity' in d:
                entity=d['entity'].encode('ascii')
                match = re.match(r'(.*)((gr1)(gr2)(gr3)(gr4)(gr5)(gr6)(gr7)(gr8)(gr9))(\.fcl$)', './demo/gr1gr2gr3gr4gr5gr6gr7gr8gr9.fcl')
                entity_name_rule = d['entity']
                try:
                    eval(entity_name_rule)
                except Exception as e:
                    print (
                        error_message +
                        ' has an invalid eval expression in the "entity" key-value pair. Exception message:' +
                        e.__doc__ +
                        '\nDetails: Suppose the entity key-value pair if defined as \'entity: "match.group(3)+\'-\'+match.group(4)+\'-\'+match.group(7)"\'' +
                        '\n and the pattern key-value pair if defined as \'pattern: "(.*)((gr1)(gr2)(gr3)(gr4)(gr5)(gr6)(gr7)(gr8)(gr9))(\.fcl$)"\'' +
                        '\n then the evaluated entity/process name for a file "./demo/gr1gr2gr3gr4gr5gr6gr7gr8gr9.fcl" is "gr1-gr2-rg5".' +
                        '\n And the exported configuration file will be "gr1-gr2-rg5.fcl".')
                    errors_count += 1
                    continue

    if errors_count > 0:
        timestamp = time.strftime('%Y-%m-%d_%H%M%S', time.localtime())
        saved_fcl_name = './'+fhicl_schema + '.' + timestamp
        shutil.move(fhicl_schema, saved_fcl_name)
        print ('Info: Old schema.fcl was renamed to ' + saved_fcl_name)
        __copy_default_schema()
        print ('Info: Replaced schema.fcl with the default one; review schema.fcl and re-run the import command.')
        exit(1)


def __composition_reader(subsets, layout, files):
    __validate_schema(layout)
    for f in files:
        for l in subsets:
            for d in layout[l]:
                match = re.match(d['pattern'], f)
                if match:
                    entity_name = match.group(2)
                    if 'entity' in d:
                        entity_name_rule = d['entity']
                        entity_name = eval(entity_name_rule)
                    entity_userdata_map = ((d['collection'], entity_name, f))
                    yield entity_userdata_map


def __list_excluded_files(config, layout, files, configuration_composition):
    excluded_files = list(files)

    for entry in configuration_composition:
        if entry[2] in excluded_files:
            excluded_files.remove(entry[2])

    return excluded_files


def __create_entity_userdata_map(cfgs):
    return {cfg[1]: open(cfg[2], 'r').read() for cfg in cfgs}


def __hashConfiguration(entity_userdata_map):
    if not entity_userdata_map:
        return False

    hashes = []

    for entity in entity_userdata_map:
        if entity == 'schema':
            continue
        hash = hashlib.md5(entity_userdata_map[entity].encode('ascii')).hexdigest()
        hashes.append(entity + ':' + hash)

    hashes.sort()
    hash = hashlib.md5(','.join(hashes).encode('ascii')).hexdigest()
    hashes.append('configuration'+':' + hash)
    entity_userdata_map['hashes'] = '\n'.join(hashes)
    return True


def getListOfAvailableRunConfigurations(searchString='*'):
    try:
        artdaq_database_uri = os.environ['ARTDAQ_DATABASE_URI']
    except KeyError:
        print ('Error: ARTDAQ_DATABASE_URI is not set.')
        return False

    query = json.loads('{"operation" : "findconfigs", "dataformat":"gui", "filter":{}}')
    query['filter']['configurations.name'] = searchString if searchString.endswith('*') else searchString+'*'

    result = conftoolp.find_configurations(json.dumps(query))

    if result[0] is not True:
        __report_error(result)
        return None

    return [c['name'] for c in json.loads(result[1])['search']]


def __getFlagsMaskFromFile(filename):
    if not os.path.isfile(filename):
        if filename == flags_file:
            print ('Warning: Cannot open {}. File not found.'.format(filename))
            return dict()
        else:
            print ('Error: Cannot open {}. File not found.'.format(filename))
            sys.exit(1)

    with open(filename, 'r') as flagsfile:
        fhicl_buffer = "".join(l for l in flagsfile.readlines())
        result = conftoolp.fhicl_to_json(fhicl_buffer, filename)
        if result[0] is not True:
            __report_error(result)
            return None

        flags = json.loads(result[1])['document']['data']['main']
        return {f: flags[f] for f in flags if re.match(r'(^flag_)([A-Za-z]+)$', f)}


def __getListOfMaskedRunConfigurations(flags, useMask=True):
    if not artdaq_database_uri.startswith("mongodb://"):
        print ('Warning: __getListOfMaskedRunConfigurations is not implemented; returning an empty result.')
        return {}

    query = json.loads('{"operation" : "searchcollection", "collection":"Flags","dataformat":"json", "filter":{"pipeline":[] } }')
    pipeline = query['filter']['pipeline']

    if useMask:
        fixed_flags = {}
        for k in flags:
            fixed_flags['document.data.main.{}'.format(k)] = flags[k]
        pipeline.append(json.loads('{ "$match":' + json.dumps(fixed_flags) + '}'))

    pipeline.append(json.loads('{"$project":{"_id": 0, "config":"$configurations.name","version":"$version"}}'))
    pipeline.append(json.loads('{"$unwind":{"path":"$config"} }'))
    pipeline.append(json.loads('{"$sort":{ "config": 1, "version": 1}}'))
    pipeline.append(json.loads('{"$group":{"_id": "$config","version": { "$last": "$version" }}}'))
    pipeline.append(json.loads('{"$project":{"_id": 0, "config":"$_id","version":"$version"}}'))
    pipeline.append(json.loads('{"$sort":{ "config": 1, "version": 1}}'))
    # print (json.dumps(pipeline))
    result = conftoolp.search_collection(json.dumps(query))

    if result[0] is not True:
        __report_error(result)
        return None
    # print (result[1])

    cvs = [c['query']['result'] for c in json.loads(result[1])['search']]

    result = {}

    for p in cvs:
        result[p['config']] = p['version']

    return result


def getListOfMaskedRunConfigurations(filename=flags_file):
    try:
        artdaq_database_uri = os.environ['ARTDAQ_DATABASE_URI']
    except KeyError:
        print ('Error: ARTDAQ_DATABASE_URI is not set.')
        return False

    flags = __getFlagsMaskFromFile(filename)

    if not flags:
        print('Warning: No flags were read from ' + filename)
        return []
    mask = __getListOfMaskedRunConfigurations(flags)
    last = __getListOfMaskedRunConfigurations(flags, False)

    return [c for c in mask if last[c] == mask[c]]


def getListOfAvailableRunConfigurationsSubtractMasked(filename=flags_file):
    m = getListOfMaskedRunConfigurations(filename)
    a = getListOfAvailableRunConfigurations('*')
    return [c for c in a if c not in m]


def getListOfArchivedRunConfigurations(searchString='*'):
    try:
        artdaq_database_uri = os.environ['ARTDAQ_DATABASE_URI']
    except KeyError:
        print ('Error: ARTDAQ_DATABASE_URI is not set.')
        return False

    os.environ['ARTDAQ_DATABASE_URI'] = __archiveuri_from_uri(artdaq_database_uri)
    print ('Info: ARTDAQ_DATABASE_URI was set to ' + os.environ['ARTDAQ_DATABASE_URI'])

    result = getListOfAvailableRunConfigurations(searchString)

    os.environ['ARTDAQ_DATABASE_URI'] = artdaq_database_uri
    print ('Info: ARTDAQ_DATABASE_URI was set to ' + os.environ['ARTDAQ_DATABASE_URI'])

    return result


def getListOfAvailableRunConfigurationPrefixes(searchString='*'):
    try:
        artdaq_database_uri = os.environ['ARTDAQ_DATABASE_URI']
    except KeyError:
        print ('Error: ARTDAQ_DATABASE_URI is not set.')
        return False

    query = json.loads('{"operation" : "findconfigs", "dataformat":"gui", "filter":{}}')
    query['filter']['configurations.name'] = searchString if searchString.endswith('*') else searchString+'*'

    result = conftoolp.find_configurations(json.dumps(query))

    if result[0] is not True:
        __report_error(result)
        return None

    configs = [c['name'] for c in json.loads(result[1])['search']]
    return list({__get_prefix(c) for c in configs})


def __getConfigurationComposition(config):
    if config is None:
        return None

    query = json.loads('{"operation" : "buildfilter", "dataformat":"gui", "filter":{}}')
    query['filter']['configurations.name'] = config

    result = conftoolp.configuration_composition(json.dumps(query))

    if result[0] is not True:
        __report_error(result)
        return None

    print ('Last configuration', config)
    # print ('Composition:', result[1])
    return [__read_document(c['query']) for c in json.loads(result[1])['search']]


def __exportConfiguration(config):
    entity_name = 0
    user_data = 1
    collection = 2

    if not __ends_on_5digitnumber(config):
        print ('Error: Configuration does not have five digits at the end.')
        return False

    cfgs = __getConfigurationComposition(config)

    if not cfgs:
        return False

    special_files  = [ ['schema', 'SystemLayout'], ['flags','Flags'], ['hashes','Hashes'] ]

    for special_file in special_files:
        for cfg in cfgs:
            if cfg[entity_name] == special_file[0] and cfg[collection] == special_file[1]:
                file_name =  cfg[entity_name] + '.fcl'
                with open(file_name, 'w') as fcl_file:
                    fcl_file.write(cfg[user_data])
                    cfg_entry = (cfg[collection], cfg[entity_name], file_name)
                    if sys.version_info[0] < 3:
                        cfg_entry = (cfg_entry[0].encode('ascii'), cfg_entry[1].encode('ascii'), cfg_entry[2].encode('ascii'))
                    print ('Exported', cfg_entry)
                    break


    __copy_default_schema()

    schema = __read_schema()

    include_dirs = []

    for l in schema['artdaq_includes']:
        include_dirs.append(l['collection'])

    special_entity_names = [ f[0] for f in special_files]

    for cfg in cfgs:
        if cfg[entity_name] in special_entity_names:
            continue

        path = cfg[collection] if cfg[collection] in include_dirs else __get_prefix(config)

        if not os.path.exists(path):
            os.makedirs(path)

        fcl_name = path+'/'+cfg[entity_name]+'.fcl'

        with open(fcl_name, 'w') as fcl_file:
            fcl_file.write(cfg[user_data])

        cfg_entry = (cfg[collection], cfg[entity_name], fcl_name)
        if sys.version_info[0] < 3:
            cfg_entry = (cfg_entry[0].encode('ascii'), cfg_entry[1].encode('ascii'), cfg_entry[2].encode('ascii'))
        print('Exported', cfg_entry)

    return True


def getLatestConfiguration(configNamePrefix):
    configs = __getConfigurationComposition(configNamePrefix)
    if configs is None:
        return None
    return {cfg[0]: cfg[1] for cfg in configs}


def __importConfiguration(configNameOrconfigPrefix, update=False, updateFlagsOnly=False):
    configPrefix = __get_prefix(configNameOrconfigPrefix)

    if configNameOrconfigPrefix == configPrefix and update is True:
        print ('Error: Updates are not allowed using the configuration prefix; configNameOrconfigPrefix=', configNameOrconfigPrefix)

    if update:
        config = configNameOrconfigPrefix
        version = config+'-ver001'

        collection_name = 'SystemLayout'
        if updateFlagsOnly:
            collection_name = 'Flags'

        found_versions = listVersions(collection_name)
        # print (found_versions)
        if version in found_versions or updateFlagsOnly:
            versions = sorted([v for v in found_versions if v.startswith(config)])
            version = config+'-ver000'
            if versions:
                version = versions[-1]
            match = re.match(r'(.*[^\d]\d{5}-ver)(\d+$)', version)
            version = match.group(1)+str(int(match.group(2)) + 1).zfill(3) if match else config+'-ver'+str(1).zfill(3)
            print ('Updating configuration', config)
            print ('Info: Storing version ' + version)
        else:
            print ('Error: Configuration update failed; configuration ' + configNameOrconfigPrefix + ' is not found.')
            return False
    else:
        config = __latest_config_name(configPrefix)
        config = __increment_config_name(config) if config else __increment_config_name(configPrefix)
        version = config+'-ver001'
        print ('New configuration', config)

    __copy_default_schema()

    schema = __read_schema()

    cfg_composition = list(
                           __composition_reader(
                               ['artdaq_processes', 'artdaq_includes', 'system_layout'],
                               schema, __find_files('.', fcl_file_pattern)))

    excluded_files = __list_excluded_files(__get_prefix(config), schema, __find_files('.', any_file_pattern), cfg_composition)

    if len(excluded_files) > 0:
        print ('Warning: The following files will be excluded from being loaded into the artdaq database '
               + ', '.join(excluded_files) + '. Update ' + fhicl_schema + ' to include them.')

        if not __allow_importing_incomplete_configurations():
            print ('Error: Importing of incomplete configurations is not allowed;'
                   + 'set ARTDAQ_DATABASE_ALLOW_INCOMPLETE_CONFIGURATIONS to TRUE to allow.')
            return False

    if update and  updateFlagsOnly:
      cfg_composition = [ cfg for cfg in cfg_composition if cfg[1] == 'flags' ]

    entity_userdata_map = __create_entity_userdata_map(cfg_composition)

    if not update:
        __hashConfiguration(entity_userdata_map)
        cfg_composition.append(('Hashes', 'hashes', './hashes.fcl'))

    cfg_composition_dict = dict(zip(iter([cfg[1] for cfg in cfg_composition]), iter(cfg_composition)))

    for entry in entity_userdata_map:
        query = json.loads('{"operation" : "store", "dataformat":"fhicl", "filter":{}}')
        query['filter']['configurations.name'] = config
        query['collection'] = cfg_composition_dict[entry][0]
        query['filter']['version'] = version
        query['filter']['entities.name'] = entry

        result = __write_document(query, entity_userdata_map[entry])

        cfg_entry = cfg_composition_dict[entry]
        if sys.version_info[0] < 3:
            cfg_entry = (cfg_entry[0].encode('ascii'), cfg_entry[1].encode('ascii'), cfg_entry[2].encode('ascii'))

        if result[0] is not True:
            print ('Failed importing', cfg_entry)
            return False

        print('Imported', cfg_entry)

    if update:
        print('Updated configuration', config)
    else:
        print('New configuration', config)

    return True


def __exportConfigurationWithBulkloader(config):
    try:
        artdaq_database_uri = os.environ['ARTDAQ_DATABASE_URI']
    except KeyError:
        print ('Error: ARTDAQ_DATABASE_URI is not set.')
        return False

    artdaq_database_remote_host = None

    try:
        artdaq_database_remote_host = os.environ['ARTDAQ_DATABASE_REMOTEHOST']
        print ('Info: ARTDAQ_DATABASE_REMOTEHOST is ' + artdaq_database_remote_host)
    except KeyError:
        artdaq_database_remote_host = None

    try:
        os.environ['ARTDAQ_DATABASE_URI'] = __archiveuri_from_uri(artdaq_database_uri)
        found_configurations = getListOfAvailableRunConfigurations(config.split('/')[0])
    except Exception:
        print ('Error: Unable to query configurations.')
        return False
    finally:
        os.environ['ARTDAQ_DATABASE_URI'] = artdaq_database_uri
    if config not in found_configurations:
        print ('Error: Configuration ' + config + ' is not archived.')
        return False

    envvars = dict(os.environ)

    keys = ['PATH', 'LD_LIBRARY_PATH', 'PYTHONPATH', 'ARTDAQ_DATABASE_DATADIR', 'ARTDAQ_DATABASE_CONFDIR']

    setenvvars_command = ' '.join('export {}="{}";'.format(k, v) for k, v in {k: envvars[k] for k in keys if k in envvars}.items())
    setenvvars_command += 'export ARTDAQ_DATABASE_URI="{}";'.format(__archiveuri_from_uri(envvars['ARTDAQ_DATABASE_URI']))
    setenvvars_command += 'echo BULKDOWNLOADER is running on $(hostname -s) and ARTDAQ_DATABASE_URI=$ARTDAQ_DATABASE_URI'

    data_files_dir = os.getcwd() if artdaq_database_remote_host is None else '$bulkloader_data_dir'
    cddir_command = 'cd {}'.format(data_files_dir)

    config_tokens = config.split('/')

    bulkloader_command = 'bulkdownloader -r {} -c {} -p {} -t $(( $(nproc)/2 ))\nrc=$?'.format(
        config_tokens[0], config_tokens[1], data_files_dir)

    retcode_command = '[[ $rc -eq 0 ]] && echo True || echo False'

    sshopts = '-o "StrictHostKeyChecking=no" -o "UserKnownHostsFile=/dev/null" -o "BatchMode=yes" -o "LogLevel=QUIET"'

    scp_command1 = 'export bulkloader_data_dir=/tmp/bulkloader/$(uuidgen);'
    scp_command1 += 'mkdir -p $bulkloader_data_dir; cd $bulkloader_data_dir'

    scp_command2 = 'scp {} -r $bulkloader_data_dir/* {}:{}'.format(sshopts, socket.gethostname(), os.getcwd())

    cleanup_commad = 'rm -rf /tmp/bulkloader/'

    command = ''

    if artdaq_database_remote_host is None:
        command = ';'.join([setenvvars_command, cddir_command, bulkloader_command, retcode_command])
    else:
        command = 'ssh {} {} \'{}\''.format(artdaq_database_remote_host, sshopts, ';'.join(
            [setenvvars_command, scp_command1, cddir_command, bulkloader_command, scp_command2, cleanup_commad, retcode_command]))

    task = None
    stdoutbuff = None
    stderrbuff = None

    try:
        task = subprocess.Popen(command, stdout=subprocess.PIPE, shell=True)
        stdoutbuff, stderrbuff = task.communicate()
        result = task.wait()
    except Exception as e:
        print ('Error: Failed running bulkdownloader over ssh on ' + artdaq_database_remote_host + '. Exception message: ' + str(e))
        result = 1

    if result != 0 or not stdoutbuff.endswith("True\n"):
        print (stdoutbuff[:-7])
        print (stderrbuff)
        return False

    print (stdoutbuff[:-6])

    return True


def importConfiguration(configNameOrconfigPrefix):
    __importConfiguration(configNameOrconfigPrefix)


def updateConfigurationFlags(configNameOrconfigPrefix):
    __importConfiguration(configNameOrconfigPrefix, True, True)


def exportConfiguration(configNamePrefix):
    # if not __ends_on_5digitnumber(configNamePrefix):
    #  print 'Error: Configuration does not have five digits at the end.'
    #  return False

    config = __latest_config_name(configNamePrefix)

    return __exportConfiguration(config)


def __archiveConfiguration(configuration_name, run_number, entity_userdata_map, update):
    if not entity_userdata_map or not configuration_name:
        return False

    if not __hashConfiguration(entity_userdata_map):
        return False

    configuration = str(run_number)+"/"+configuration_name
    version = str(run_number)+"/"+configuration_name
    found_versions = listVersions('SystemLayout')
    # print (found_versions)
    if version in found_versions:
        if not update:
            print ('Error: Configuration ' + str(run_number)+"/"+configuration_name + ' is already archived.')
            return False
        else:
            versions = sorted([v for v in found_versions if v.startswith(version)])
            version = versions[-1]
            match = re.match(r'(.*[^\d]\d{5}v)(\d+$)', version)
            version = match.group(1)+str(int(match.group(2)) + 1) if match else version+"v"+str(1)
            print ('Info: storing version ' + version)
    else:
        if update:
            print ('Error: Configuration ' + str(run_number)+"/"+configuration_name + ' is not archived.')
            return False

    __copy_default_schema()

    for entry in entity_userdata_map:
        query = json.loads('{"operation" : "store", "dataformat":"fhicl", "filter":{}}')
        query['filter']['configurations.name'] = configuration
        query['collection'] = 'RunHistory'

        if entry == 'schema':
            query['collection'] = 'SystemLayout'

        query['filter']['version'] = version
        query['filter']['entities.name'] = entry
        query['filter']['runs.name'] = str(run_number)

        result = __write_document(query, entity_userdata_map[entry])

        if result[0] is not True:
            return False

    return True


def archiveRunConfiguration(config, run_number, update=False):
    try:
        artdaq_database_uri = os.environ['ARTDAQ_DATABASE_URI']
    except KeyError:
        print ('Error: ARTDAQ_DATABASE_URI is not set.')
        return False

    if artdaq_database_uri.startswith("mongodb://") and not update:
        return __archiveConfigurationWithBulkloader(config, run_number, update)

    __copy_default_schema()

    schema = __read_schema()

    cfg_composition = list(__composition_reader(['run_history', 'system_layout'], schema, __find_files('.', fcl_file_pattern)))

    excluded_files = __list_excluded_files(__get_prefix(config), schema, __find_files('.', any_file_pattern), cfg_composition)

    if len(excluded_files) > 0:
        print ('Warning: The following files will be excluded from being loaded into the artdaq database '
               + ', '.join(excluded_files) + '. Update ' + fhicl_schema + ' to include them.')

        if not __allow_importing_incomplete_configurations():
            print ('Error: Importing of incomplete configurations is not allowed; '
                   + 'set ARTDAQ_DATABASE_ALLOW_INCOMPLETE_CONFIGURATIONS to TRUE to allow.')
            return False

    entity_userdata_map = __create_entity_userdata_map(cfg_composition)

    os.environ['ARTDAQ_DATABASE_URI'] = __archiveuri_from_uri(artdaq_database_uri)
    print ('Info: ARTDAQ_DATABASE_URI was set to ' + os.environ['ARTDAQ_DATABASE_URI'])

    result = __archiveConfiguration(config, run_number, entity_userdata_map, update)

    os.environ['ARTDAQ_DATABASE_URI'] = artdaq_database_uri
    print ('Info: ARTDAQ_DATABASE_URI was set to ' + os.environ['ARTDAQ_DATABASE_URI'])

    if result is not True:
        return False

    for cfg_entry in cfg_composition:
        if sys.version_info[0] < 3:
            cfg_entry = (cfg_entry[0].encode('ascii'), cfg_entry[1].encode('ascii'), cfg_entry[2].encode('ascii'))
        print ('Archived', cfg_entry)

    return True


def updateArchivedRunConfiguration(config, run_number):
    return archiveRunConfiguration(config, run_number, True)


def __archiveConfigurationWithBulkloader(config, run_number, update):
    try:
        artdaq_database_uri = os.environ['ARTDAQ_DATABASE_URI']
    except KeyError:
        print ('Error: ARTDAQ_DATABASE_URI is not set.')
        return False

    artdaq_database_remote_host = None

    try:
        artdaq_database_remote_host = os.environ['ARTDAQ_DATABASE_REMOTEHOST']
        print ('Info: ARTDAQ_DATABASE_REMOTEHOST is ' + artdaq_database_remote_host)
    except KeyError:
        artdaq_database_remote_host = None

    version = str(run_number)+'/'+config

    try:
        os.environ['ARTDAQ_DATABASE_URI'] = __archiveuri_from_uri(artdaq_database_uri)
        found_versions = listVersions('SystemLayout')
    except Exception:
        print ('Error: Unable to query versions.')
        return False
    finally:
        os.environ['ARTDAQ_DATABASE_URI'] = artdaq_database_uri
    if version in found_versions:
        if not update:
            print ('Error: Configuration ' + str(run_number)+'/'+config + ' is already archived.')
            return False
    else:
        if update:
            print ('Error: Configuration ' + str(run_number)+'/'+config + ' is not archived.')
            return False

    __copy_default_schema()
    schema = __read_schema()

    cfg_composition = list(__composition_reader(['run_history', 'system_layout'], schema, __find_files('.', fcl_file_pattern)))

    excluded_files = __list_excluded_files(__get_prefix(config), schema, __find_files('.', any_file_pattern), cfg_composition)

    if len(excluded_files) > 0:
        print ('Warning: The following files will be excluded from being loaded into the artdaq database '
               + ', '.join(excluded_files) + '. Update ' + fhicl_schema + ' to include them.')

        if not __allow_importing_incomplete_configurations():
            print ('Error: Importing of incomplete configurations is not allowed; '
                   + 'set ARTDAQ_DATABASE_ALLOW_INCOMPLETE_CONFIGURATIONS to TRUE to allow.')
            return False

    envvars = os.environ

    keys = ['PATH', 'LD_LIBRARY_PATH', 'PYTHONPATH', 'ARTDAQ_DATABASE_DATADIR', 'ARTDAQ_DATABASE_CONFDIR']

    setenvvars_command = ' '.join('export {}="{}";'.format(k, v) for k, v in {k: envvars[k] for k in keys if k in envvars}.items())
    setenvvars_command += 'export ARTDAQ_DATABASE_URI="{}";'.format(__archiveuri_from_uri(envvars['ARTDAQ_DATABASE_URI']))
    setenvvars_command += 'echo BULKLOADER is running on $(hostname -s) and ARTDAQ_DATABASE_URI=$ARTDAQ_DATABASE_URI'

    data_files_dir = os.getcwd() if artdaq_database_remote_host is None else '$bulkloader_data_dir'
    cddir_command = 'cd {}'.format(data_files_dir)

    bulkloader_command = 'bulkloader -r {} -c {} -p {} -t $(( $(nproc)/2 ))\nrc=$?'.format(run_number, config, data_files_dir)
    retcode_command = '[[ $rc -eq 0 ]] && echo True || echo False'

    sshopts = '-o "StrictHostKeyChecking=no" -o "UserKnownHostsFile=/dev/null" -o "BatchMode=yes" -o "LogLevel=QUIET"'

    scp_command = 'export bulkloader_data_dir=/tmp/bulkloader/$(uuidgen);'
    scp_command += 'mkdir -p $bulkloader_data_dir; cd $bulkloader_data_dir;'
    scp_command += 'scp {} -r {}:{} $bulkloader_data_dir'.format(sshopts, socket.gethostname(), os.getcwd())

    cleanup_commad = 'rm -rf /tmp/bulkloader/'

    command = ''

    if artdaq_database_remote_host is None:
        command = ';'.join([setenvvars_command, cddir_command, bulkloader_command, retcode_command])
    else:
        command = 'ssh {} {} \'{}\''.format(artdaq_database_remote_host, sshopts, ';'.join(
            [setenvvars_command, scp_command, cddir_command, bulkloader_command, cleanup_commad, retcode_command]))

    task = None
    stdoutbuff = None
    stderrbuff = None
    # print ('command >>>>', command, '<<<< ' )
    try:
        task = subprocess.Popen(command, stdout=subprocess.PIPE, shell=True)
        stdoutbuff, stderrbuff = task.communicate()
        result = task.wait()
    except Exception as e:
        print ('Error: Failed running bulkloader over ssh on ' + artdaq_database_remote_host + '. Exception message: ' + str(e))
        result = 1

    if result != 0 or not stdoutbuff.endswith("True\n"):
        print (stdoutbuff[:-7])
        print (stderrbuff)
        return False

    print (stdoutbuff[:-6])

    for cfg_entry in cfg_composition:
        if sys.version_info[0] < 3:
            cfg_entry = (cfg_entry[0].encode('ascii'), cfg_entry[1].encode('ascii'), cfg_entry[2].encode('ascii'))
        print ('Archived', cfg_entry)

    return True


def exportArchivedRunConfiguration(config):
    try:
        artdaq_database_uri = os.environ['ARTDAQ_DATABASE_URI']
    except KeyError:
        print ('Error: ARTDAQ_DATABASE_URI is not set.')
        return False

    if artdaq_database_uri.startswith("mongodb://") and len(config.split('/')) == 2:
        return __exportConfigurationWithBulkloader(config)

    os.environ['ARTDAQ_DATABASE_URI'] = __archiveuri_from_uri(artdaq_database_uri)
    print ('Info: ARTDAQ_DATABASE_URI was set to ' + os.environ['ARTDAQ_DATABASE_URI'])

    result = __exportConfiguration(config)

    os.environ['ARTDAQ_DATABASE_URI'] = artdaq_database_uri
    print ('Info: ARTDAQ_DATABASE_URI was set to ' + os.environ['ARTDAQ_DATABASE_URI'])

    return result


def listDatabases():
    result = conftoolp.list_databases('{"operation" : "listdatabases", "dataformat":"csv", "filter":{}}')
    if result[0] is not True:
        __report_error(result)
        return None

    return result[1].split(',')


def listVersions(collection_name):
    query = json.loads('{"operation" : "findversions", "dataformat":"csv", "filter":{}}')
    query['collection'] = collection_name
    query['filter']['entities.name'] = "*"

    result = conftoolp.find_versions(json.dumps(query))
    if result[0] is not True:
        __report_error(result)
        return None

    return result[1].split(',')


def json_to_fhicl(filename):
    with open(filename, 'r') as json_file:
        layout_json = json.load(json_file)

    result = conftoolp.json_to_fhicl(layout_json, filename)
    if result[0] is not True:
        __report_error(result)
        return None

    with open(filename, 'w') as fhicl_file:
        layout_json = fhicl_file.write(result[1])

    return result[1]


def fhicl_to_json(filename):
    with open(filename, 'r') as fhicl_file:
        layout_fhicl = fhicl_file.read()

    result = conftoolp.fhicl_to_json(layout_fhicl, filename)
    if result[0] is not True:
        __report_error(result)
        return None

    with open(filename[:-4] + '.json', 'w') as json_file:
        json_file.write(result[1])

    return result[1]


def listCollections():
    result = conftoolp.list_collections('{"operation" : "listcollections", "dataformat":"csv", "filter":{}}')
    if result[0] is not True:
        __report_error(result)
        return None

    return result[1].split(',')


def listConfigurations():
    result = conftoolp.find_configurations('{"operation" : "findconfigs", "dataformat":"csv", "filter":{}}')
    if result[0] is not True:
        __report_error(result)
        return None

    return result[1].split(',')


def readDatabaseInfo():
    result = conftoolp.read_dbinfo('{"operation" : "readdbinfo", "dataformat":"json", "filter":{}}')
    if result[0] is not True:
        __report_error(result)
        return None

    return result[1]


def exportDatabase():
    print ('ARTDAQ_DATABASE_URI', os.environ['ARTDAQ_DATABASE_URI'])
    result = conftoolp.export_database('{"operation" : "exportdatabase", "dataformat":"json", "source": ".",  "filter":{}}')
    if result[0] is not True:
        __report_error(result)

    return result[0]


def importDatabase():
    print ('ARTDAQ_DATABASE_URI', os.environ['ARTDAQ_DATABASE_URI'])
    result = conftoolp.import_database('{"operation" : "importdatabase", "dataformat":"json", "source": ".", "filter":{}}')
    if result[0] is not True:
        __report_error(result)

    return result[0]


def help():
    print ('Usage: conftool.py [operation] [config name prefix]')
    print ('')
    print ('Example:')
    print (' conftool.py exportConfiguration demo_safemode00003')
    print (' conftool.py importConfiguration demo_safemode')
    print (' conftool.py updateConfigurationFlags demo_safemode00003')
    print (' conftool.py getListOfAvailableRunConfigurationPrefixes')
    print (' conftool.py getListOfAvailableRunConfigurations')
    print (' conftool.py getListOfAvailableRunConfigurations demo_')
    print (' conftool.py getListOfAvailableRunConfigurationsSubtractMasked  {} #file with configuration flags'.format(flags_file))
    print (' conftool.py getListOfMaskedRunConfigurations {} #file with configuation flags'.format(flags_file))
    # print (' conftool.py exportDatabase #writes archives into the current  directory')
    # print (' conftool.py importDatabase #reads archives from the current  directory')
    print (' conftool.py archiveRunConfiguration demo_safemode 23 #where 23 is a run number')
    print (' conftool.py updateArchivedRunConfiguration demo_safemode 23 #where 23 is a run number')
    print (' conftool.py getListOfArchivedRunConfigurations 23 #where 23 is a run number')
    print (' conftool.py exportArchivedRunConfiguration 23/demo_safemode00003 #where 23/demo_safemode00003 is a configuration name')
    print (' conftool.py listDatabases')
    print (' conftool.py listCollections')
    print (' conftool.py readDatabaseInfo')
    return True


if __name__ == "__main__":
    if len(sys.argv) == 1 or sys.argv[1] in ['--h', '-h', 'h', '--help', '-help', 'help', '--info', 'info']:
        help()
        sys.exit(0)

    try:
        artdaq_database_uri = os.environ['ARTDAQ_DATABASE_URI']
    except KeyError:
        print ('Error: ARTDAQ_DATABASE_URI is not set.')
        sys.exit(1)

    if sys.argv[1] in [o[0] for o in inspect.getmembers(sys.modules[__name__]) if inspect.isfunction(o[1])]:
        getattr(conftoolp, 'enable_trace')()
        thefunc = getattr(sys.modules[__name__], sys.argv[1])

        if sys.version_info[0] < 3:
            args = inspect.getargspec(thefunc)
        else:
            args = inspect.getfullargspec(thefunc)

        result = thefunc(*sys.argv[2:len(args[0]) + 2]) if args else thefunc()

        if isinstance(result, list):
            print ('\n'.join([str(el) for el in result]))
        else:
            print (result)

        if isinstance(result,bool) and (result is False):
            sys.exit(1)

        sys.exit(0)

    functions_list = [o[0] for o in inspect.getmembers(conftoolp) if inspect.isbuiltin(o[1])]
    if sys.argv[1] not in functions_list:
        print ('Error: Unrecognised API function.')
        print ('Avaialble functions:')
        print (functions_list)
        sys.exit(1)

    getattr(conftoolp, 'enable_trace')()
    result = getattr(conftoolp, sys.argv[1])(*sys.argv[2:len(sys.argv)])

    if result[0] is True:
        print (result[1])
        sys.exit(0)
    else:
        __report_error(result)
        sys.exit(1)
