#!/usr/bin/env python

#using python 2.7
#~/conftool.py list_databases '{"operation" : "listdatabases", "dataformat":"csv", "filter":{}}'
import sys
import importlib
import inspect
import conftoolg
import json
import re
import fnmatch
import os
import shutil

fhicl_schema='schema.fcl'

any_file_pattern = '*.*'
fcl_file_pattern = '*.fcl'

artdaq_database_uri=None

def __copy_default_schema():
  if os.path.isfile(fhicl_schema):
    return

  schema='schema.fcl'
  
  try:
    schema=os.environ['ARTDAQ_DATABASE_CONFDIR']+'/' +fhicl_schema
  except KeyError:    
    schema=os.path.dirname(os.path.realpath(__file__))+'/../conf'+ fhicl_schema

  if not os.path.isfile(schema):
    print ('File not found', schema)
    sys.exit(1)
  
  shutil.copyfile(schema,fhicl_schema)
  print 'Info: Copied ' + schema + ' to ' + fhicl_schema

def __report_error(result):
  print >> sys.stderr, 'Error:' + result[1]

def __allow_importing_incomplete_configurations():
  try:
    allow_incomplete=os.environ['ARTDAQ_DATABASE_ALLOW_INCOMPLETE_CONFIGURATIONS'].lower()
    if allow_incomplete=='true':
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
  return  match.group(1) + str(int(match.group(2)) + 1).zfill(5) if match else config+str(1).zfill(5)
  
def __latest_config_name(configNamePrefix):
  query = json.loads('{"operation" : "findconfigs", "dataformat":"gui", "filter":{}}')  
  query['filter']['configurations.name']=configNamePrefix+'*' 
  result = conftoolg.find_configurations(json.dumps(query))
  
  if result[0] is not True:
    __report_error(result)
    return None

  configs=list(c['name'].encode('ascii') for c in json.loads(result[1])['search'])
  
  if not configs:
    return None
  
  if __ends_on_5digitnumber(configNamePrefix):
    configs=list(config for config in configs if configNamePrefix==config)
  else:
    configs=list(config for config in configs if re.match(r'(^'+configNamePrefix+')(\d{5}$)', config))

  if not configs:
    return None 
  
  configs.sort()
  
  return configs[-1]

def __read_document(query):
  query['dataformat']='origin'
  
  result = conftoolg.read_document(json.dumps(query))
  
  if result[0] is not True:
    __report_error(result)
    return (None,None)
  
  return  (query['filter']['entities.name'].encode('ascii'),result[1],query['collection'].encode('ascii'))

def __write_document(query,document):
  result = conftoolg.write_document(json.dumps(query),document)
  
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
  with open(fhicl_schema,'r') as fhicl_file:
    return fhicl_file.read()

def __read_schema():
  layout_fhicl = __read_fhicl_schema()
  fhicl_file=fhicl_schema
  result = conftoolg.fhicl_to_json(layout_fhicl,fhicl_file)  
  if result[0] is not True:
    __report_error(result)
    return None

  layout_json= json.loads(result[1])
  return layout_json['document']['data']['main']

def __composition_reader(subsets,layout,files):
  for f in files: 
    for l in subsets:
      for d in layout[l]:
	match = re.match(d['pattern'].encode('ascii'), f)
	if match:
	  entity_name=match.group(2)	    
	  if 'entity' in d:
	    entity_name_rule=d['entity'].encode('ascii')  
	    entity_name = eval(entity_name_rule)	      
	  entity_userdata_map=((d['collection'].encode('ascii'), entity_name, f ))
	  yield entity_userdata_map

def __list_excluded_files(config,layout,files,configuration_composition):
  excluded_files=list(files)

  for entry in configuration_composition:
    excluded_files.remove(entry[2])

  return excluded_files
  
def __create_entity_userdata_map(cfgs):
  return dict( (cfg[1],open(cfg[2], 'r').read()) for cfg in cfgs)

def getListOfAvailableRunConfigurations(searchString='*'):
  query = json.loads('{"operation" : "findconfigs", "dataformat":"gui", "filter":{}}')
  query['filter']['configurations.name']=searchString if searchString.endswith('*') else searchString+'*'

  result = conftoolg.find_configurations(json.dumps(query))
  
  if result[0] is not True:
    __report_error(result)
    return None
  
  return list(c['name'].encode('ascii') for c in json.loads(result[1])['search'])


def getListOfArchivedRunConfigurations(searchString='*'):  
  try:
    artdaq_database_uri=os.environ['ARTDAQ_DATABASE_URI']
  except KeyError:
    print 'Error: ARTDAQ_DATABASE_URI is not set.'
    return False

  os.environ['ARTDAQ_DATABASE_URI']=artdaq_database_uri+'_archive'
  print 'Info: ARTDAQ_DATABASE_URI was set to ' + os.environ['ARTDAQ_DATABASE_URI']

  result=getListOfAvailableRunConfigurations(searchString)

  os.environ['ARTDAQ_DATABASE_URI']=artdaq_database_uri
  print 'Info: ARTDAQ_DATABASE_URI was set to ' + os.environ['ARTDAQ_DATABASE_URI']

  return result 

	  
#std::vector<std::string> getListOfAvailableRunConfigurationPrefixes(std::string searchString = "*"); 
def getListOfAvailableRunConfigurationPrefixes(searchString='*'):
  query = json.loads('{"operation" : "findconfigs", "dataformat":"gui", "filter":{}}')
  query['filter']['configurations.name']=searchString if searchString.endswith('*') else searchString+'*'

  result = conftoolg.find_configurations(json.dumps(query))
  
  if result[0] is not True:
    __report_error(result)
    return None
  
  configs=list(c['name'].encode('ascii') for c in json.loads(result[1])['search'])
  return list(set( __get_prefix(c) for c in configs))

def __getConfigurationComposition(config):
  if config is None:
    return None

  query = json.loads('{"operation" : "buildfilter", "dataformat":"gui", "filter":{}}')  
  query['filter']['configurations.name']=config 
  
  result = conftoolg.configuration_composition(json.dumps(query))
    
  if result[0] is not True:
    __report_error(result)
    return None
  
  print ('Last configuration',config)
  return list(__read_document(c['query']) for c in json.loads(result[1])['search'])
   
def __exportConfiguration(config):
  entity_name=0
  user_data=1
  collection=2

  if not __ends_on_5digitnumber(config):
    print 'Error: Configuration does not have five digits at the end.'
    return False

  cfgs = __getConfigurationComposition(config)
  
  if not cfgs:
      return False
  
  for cfg in cfgs:
    if cfg[entity_name] == 'schema' and cfg[collection]=='SystemLayout':
      with open(fhicl_schema, "w") as fcl_file:
	fcl_file.write(cfg[user_data]) 
	print ('Exported',(cfg[collection],cfg[entity_name],fhicl_schema))
	break
  
  __copy_default_schema()

  schema =__read_schema()
  
  include_dirs=[]
  
  for l in schema['artdaq_includes']:
    include_dirs.append(l['collection'])
    
  for cfg in cfgs:
    if cfg[entity_name]=='schema':
      continue
    
    path = cfg[collection] if cfg[collection] in include_dirs else __get_prefix(config)
    
    if not os.path.exists(path):
      os.makedirs(path)
    
    fcl_name = path+'/'+cfg[entity_name]+'.fcl'
    
    with open(fcl_name, "w") as fcl_file:
      fcl_file.write(cfg[user_data])

    print ('Exported',(cfg[collection],cfg[entity_name],fcl_name))

  return True

#std::map<std::string /*entityName*/, std::string /*FHiCL document*/> getLatestConfiguration(std::string const& configNamePrefix); 
def getLatestConfiguration(configNamePrefix):
  return dict((cfg[0],cfg[1]) for cfg in __getLatestConfiguration(configNamePrefix))

def importConfiguration(configNameOrconfigPrefix):
  configPrefix= __get_prefix(configNameOrconfigPrefix)
  
  config = __latest_config_name(configPrefix)
  
  config = __increment_config_name(config) #if config else __increment_config_name(configPrefix)

  schema =__read_schema()   
  
  cfg_composition =list( __composition_reader(['artdaq_processes','artdaq_includes','system_layout'], schema, __find_files('.',fcl_file_pattern)))

  excluded_files=__list_excluded_files(__get_prefix(config), schema, __find_files('.',any_file_pattern),cfg_composition)
  
  if len(excluded_files)>0:
      print 'Warning: The following files will be excluded from being loaded into the artdaq database ' \
	+ ', '.join(excluded_files) + '. Update ' + fhicl_schema + ' to include them.'
  
  if not __allow_importing_incomplete_configurations():
      print 'Error: Importing of incomplete configurations is not allowed; ' \
	+ 'set ARTDAQ_DATABASE_ALLOW_INCOMPLETE_CONFIGURATIONS to TRUE to allow.'
      return False 
    
  print ('New configuration', config)
 
  for entry in cfg_composition:
    query = json.loads('{"operation" : "store", "dataformat":"fhicl", "filter":{}}')  
    query['filter']['configurations.name']=config
    query['collection']=entry[0]
    query['filter']['version']=config+'-ver001'
    query['filter']['entities.name']=entry[1]       
  
    result=__write_document(query,open(entry[2], 'r').read())
    print ('Import',entry)
    if result[0] is not True:
      return False


  return True

def exportConfiguration(configNamePrefix):
  if not __ends_on_5digitnumber(configNamePrefix):
    print 'Error: Configuration does not have five digits at the end.'
    return False

  config=__latest_config_name(configNamePrefix)

  return __exportConfiguration(config)


#bool /* status */ archiveConfiguration(std::string configuration_name,int run_number, std::map<std::string, std::string>); 
def archiveConfiguration(configuration_name,run_number,entity_userdata_map):
  if not entity_userdata_map or not configuration_name:
    return False
  
  version=str(run_number)+"/"+configuration_name
  
  if version in listVersions('RunHistory'):
    print 'Error: Run ' +str(run_number) + ' already archived.'
    return False
  
  __copy_default_schema()
  
  for entry in entity_userdata_map:
    query = json.loads('{"operation" : "store", "dataformat":"fhicl", "filter":{}}')  
    query['filter']['configurations.name']=version
    query['collection']='RunHistory'
    query['filter']['version']=version
    query['filter']['entities.name']=entry       
    query['filter']['runs.name']=str(run_number) 
  
    result=__write_document(query,entity_userdata_map[entry])

    if result[0] is not True:
      return False

  return True  

def archiveRunConfiguration(config,run_number):  
  try:
    artdaq_database_uri=os.environ['ARTDAQ_DATABASE_URI']
  except KeyError:
    print 'Error: ARTDAQ_DATABASE_URI is not set.'
    return False

  __copy_default_schema()

  schema =__read_schema()

  cfg_composition =list( __composition_reader(['run_history'], schema, __find_files('.',fcl_file_pattern)))

  excluded_files=__list_excluded_files(__get_prefix(config), schema, __find_files('.',any_file_pattern),cfg_composition)
  
  if len(excluded_files)>0:
      print 'Warning: The following files will be excluded from being loaded into the artdaq database ' \
	+ ', '.join(excluded_files) + '. Update ' + fhicl_schema + ' to include them.'
  
  if not __allow_importing_incomplete_configurations():
      print 'Error: Importing of incomplete configurations is not allowed; ' \
	+ 'set ARTDAQ_DATABASE_ALLOW_INCOMPLETE_CONFIGURATIONS to TRUE to allow.'
      return False 
  
  entity_userdata_map=__create_entity_userdata_map(cfg_composition)

  os.environ['ARTDAQ_DATABASE_URI']=artdaq_database_uri+'_archive'
  print 'Info: ARTDAQ_DATABASE_URI was set to ' + os.environ['ARTDAQ_DATABASE_URI']

  result=archiveConfiguration(config,run_number,entity_userdata_map)

  os.environ['ARTDAQ_DATABASE_URI']=artdaq_database_uri
  print 'Info: ARTDAQ_DATABASE_URI was set to ' + os.environ['ARTDAQ_DATABASE_URI']

  if result is not True:
     return False

  for entry in cfg_composition:
    print ('Archive',entry)

  return True  

def exportArchivedRunConfiguration(config):  
  if not __ends_on_5digitnumber(config):
    print 'Error: Configuration does not have five digits at the end.'
    return False

  try:
    artdaq_database_uri=os.environ['ARTDAQ_DATABASE_URI']
  except KeyError:
    print 'Error: ARTDAQ_DATABASE_URI is not set.'
    return False

  os.environ['ARTDAQ_DATABASE_URI']=artdaq_database_uri+'_archive'
  print 'Info: ARTDAQ_DATABASE_URI was set to ' + os.environ['ARTDAQ_DATABASE_URI']

  result=__exportConfiguration(config)

  os.environ['ARTDAQ_DATABASE_URI']=artdaq_database_uri
  print 'Info: ARTDAQ_DATABASE_URI was set to ' + os.environ['ARTDAQ_DATABASE_URI']

  return result  

def listDatabases(): 
  result = conftoolg.list_databases('{"operation" : "listdatabases", "dataformat":"csv", "filter":{}}')  
  if result[0] is not True:
    __report_error(result)
    return None
  
  return result[1].split(',')

def listVersions(collection_name):
  query = json.loads('{"operation" : "findversions", "dataformat":"csv", "filter":{}}')  
  query['collection']=collection_name
  query['filter']['entities.name']="*"       

  result = conftoolg.find_versions (json.dumps(query))  
  if result[0] is not True:
    __report_error(result)
    return None
  
  return result[1].split(',')

 
def json_to_fhicl(filename):
  with open(filename,"r") as json_file:
    layout_json = json.load(json_file)

  result = conftoolg.json_to_fhicl(layout_json,filename)  
  if result[0] is not True:
    __report_error(result)
    return None

  with open(filename,"w") as fhicl_file:
    layout_json = fhicl_file.write(result[1])
  
  return result[1]

def fhicl_to_json(filename):
  with open(filename,"r") as fhicl_file:
    layout_fhicl = fhicl_file.read()

  result = conftoolg.fhicl_to_json(layout_fhicl,filename)  
  if result[0] is not True:
    __report_error(result)
    return None

  with open(filename[:-4] + '.json',"w") as json_file:
    json_file.write(result[1])
  
  return result[1]

def listCollections():
  result = conftoolg.list_collections('{"operation" : "listcollections", "dataformat":"csv", "filter":{}}')  
  if result[0] is not True:
    __report_error(result)
    return None
  
  return result[1].split(',')

def listConfigurations(): 
  result = conftoolg.find_configurations('{"operation" : "findconfigs", "dataformat":"csv", "filter":{}}')  
  if result[0] is not True:
    __report_error(result)
    return None
  
  return result[1].split(',')

def readDatabaseInfo(): 
  result = conftoolg.read_dbinfo('{"operation" : "readdbinfo", "dataformat":"json", "filter":{}}')  
  if result[0] is not True:
    __report_error(result)
    return None
  
  return result[1]

def exportDatabase():
  print ('ARTDAQ_DATABASE_URI',os.environ['ARTDAQ_DATABASE_URI'])
  result = conftoolg.export_database('{"operation" : "exportdatabase", "dataformat":"json", "source": ".",  "filter":{}}')
  if result[0] is not True:
    __report_error(result)

  return result[0]

def importDatabase():
  print ('ARTDAQ_DATABASE_URI',os.environ['ARTDAQ_DATABASE_URI'])
  result = conftoolg.import_database('{"operation" : "importdatabase", "dataformat":"json", "source": ".", "filter":{}}')
  if result[0] is not True:
    __report_error(result)

  return result[0]

def help():
  print 'Usage: conftool.py [operation] [config name prefix]'
  print ''
  print 'Example:'
  print ' conftool.py exportConfiguration demo_safemode00003'
  print ' conftool.py importConfiguration demo_safemode'
  print ' conftool.py getListOfAvailableRunConfigurationPrefixes'
  print ' conftool.py getListOfAvailableRunConfigurations'
  print ' conftool.py getListOfAvailableRunConfigurations demo_'
#  print ' conftool.py exportDatabase #writes archives into the current directory'
#  print ' conftool.py importDatabase #reads archives from the current directory'
  print ' conftool.py archiveRunConfiguration demo_safemode 23 #where 23 is a run number'
  print ' conftool.py getListOfArchivedRunConfigurations 23 #where 23 is a run number'
  print ' conftool.py exportArchivedRunConfiguration 23/demo_safemode00003 #where 23/demo_safemode00003 is a configuration name'
  print ' conftool.py listDatabases'
  print ' conftool.py listCollections'
  print ' conftool.py readDatabaseInfo'
  return True
 
if __name__ == "__main__":
  if len(sys.argv)==1 or sys.argv[1] in ['--h','-h','h','--help','-help','help','--info','info']:
    help()
    sys.exit(0)
  
  try:
    artdaq_database_uri=os.environ['ARTDAQ_DATABASE_URI']
  except KeyError:    
    print 'Error: ARTDAQ_DATABASE_URI is not set.'
    sys.exit(1)
  
  if sys.argv[1] in [o[0] for o in inspect.getmembers(sys.modules[__name__]) if inspect.isfunction(o[1])]:        
    getattr(conftoolg, 'enable_trace')()
    thefunc=getattr(sys.modules[__name__], sys.argv[1])
    args = inspect.getargspec(thefunc)
    result = thefunc(*sys.argv[2:len(args[0]) + 2]) if args else thefunc()

    if type(result) is list:
      print '\n'.join([ str(el) for el in result ])
    else:
      print result
      
    sys.exit(0)

  functions_list = [o[0] for o in inspect.getmembers(conftoolg) if inspect.isbuiltin(o[1])]  
  if sys.argv[1] not in functions_list:
    print 'Error: Unrecognised API function.'
    print 'Avaialble functions:'
    print  functions_list
    sys.exit(1)
    
  getattr(conftoolg, 'enable_trace')()
  result = getattr(conftoolg, sys.argv[1])(*sys.argv[2:len(sys.argv)])

  if result[0] is True:
    print result[1]
    sys.exit(0)
  else:
    __report_error(result)
    sys.exit(1)

