#!/bin/bash

export artdaq_database_version=v1_03_13
export config_editor_version=v1_00_01

function export_global_config()
{
  arg_strings=($*)
  num_args=${#arg_strings[*]}
        
 if [ $num_args -lt 1 ]; then
   echo "Error: Invalid argument count in export_global_config(); arg_strings=\"${arg_strings[*]}\" $*"
   echo "Usage: export_global_config global-config"   
   return 1
 fi

 newconfig_dir=./newconfig

 if [ -d "${newconfig_dir}" ]; then
 /usr/bin/rm -rf ${newconfig_dir}
 fi

 /usr/bin/mkdir -p ${newconfig_dir}


 global_config_name=$1
 
 tmp_file_name=config.${global_config_name}.tar.bzip2.base64

 conftool -o globalconfload -d filesystem -f gui -g ${global_config_name} -r ${tmp_file_name}.out

 cat ${tmp_file_name}.out |cut -d ":" -f2 |cut -d "\"" -f2  |base64 -d |tar xjfv - -C ./ > /dev/null 2>&1

 cp -R  tmp/*/*  ${newconfig_dir} > /dev/null 2>&1

 rm -rf tmp ${tmp_file_name}.out > /dev/null 2>&1

 echo "Exported ${global_config_name} into ${newconfig_dir}"
 
}

function import_global_config()
{
  arg_strings=($*)
  num_args=${#arg_strings[*]}

 if [ $num_args -lt 1 ]; then
   echo "Error: Invalid argument count in import_global_config(); arg_strings=\"${arg_strings[*]}\" $*"
    echo "Usage: import_global_config global-config [version-name]"         
   return 1
 fi

 config_dir=./config

 global_config_name=$1
 
 version_name=v${global_config_name}-ver01
  
 if [ $num_args -eq 2 ]; then
  version_name=$2
 fi

 tmp_file_name=config.${global_config_name}.tar.bzip2.base64

 tar cjf - ./config/*  -C config |base64 --wrap=0 > ${tmp_file_name}

 conftool -o globalconfstore -d filesystem -f fhicl -v ${version_name} -g ${global_config_name} -s ${tmp_file_name} -r ~/result.json
  
 rm ${tmp_file_name}

 conftool -o findconfigs  -f gui -g ${global_config_name}

}


function setup_artdaq_database(){  
artdaq_database_qqual=$(ups active |grep artdaq_core | grep -Eo "\-q[^-]+")
artdaq_database_version=$(ups list -aK+ artdaq_database ${artdaq_database_qqual} |sort -k 2 -r |head -n 1 | grep -Eo "\v[^\"]+")
setup artdaq_database ${artdaq_database_version} ${artdaq_database_qqual}
echo $(ups active |grep artdaq_database)
}

function setup_config_editor(){
config_editor_qqual=$(ups active |grep artdaq_core | grep -Eo "\-q[^-]+")
config_editor_version=$(ups list -aK+ config_editor ${config_editor_qqual} |sort -k 2 -r |head -n 1 | grep -Eo "\v[^\"]+")
setup artdaq_node_server  ${config_editor_version} ${config_editor_qqual}
echo $(ups active |grep artdaq_node_server)
}


#setup artdaq_node_server v1_00_01 -q e10:prof:s35
#export ARTDAQ_DATABASE_DATADIR=/home/nfs/dunedaq/daqarea/databases
#unset ARTDAQ_DATABASE_URI

