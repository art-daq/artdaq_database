#!/bin/bash

export artdaq_database_version=v1_03_11
export config_editor_version=v1_00_01

conftool_log_dir=/home/nfs/daqoutput/daqlogs/conftool

function show_help(){
printf "\n\nUsage: $(basename $0) [OPERATION] [OPTION]...\n"
printf " -h [ --help ]              Produce help message\n"
printf " -o [ --operation ] arg     Database operation [import_global_config or export_global_config]\n"
printf " -g [ --configuration ] arg Global configuration name [globConfig001]\n"
printf " -v [ --version ] arg       Configuration version [ver001]\n"
printf " -s [ --source ] arg        Configuration directory name[config]\n"
printf "Example: \n\tconftool.sh -o export_global_config -g demo003 -s newconfig"
printf "\n\tconftool.sh -o import_global_config -g demo004 -v ver004 -s config"
printf "\n"
printf "\nLogs in ${conftool_log_dir}\n"
ls -t  ${conftool_log_dir} |head -3
echo
}

unset ARTDAQ_DATABASE_URI

yes=yes
no=no


function read_command_line_options() {
  show_help_present=$no
  global_config_name_present=$no
  source_dir_present=$no
  version_name_present=$no
  enable_debug_present=$no

  getopt --test > /dev/null
  
  if [[ $? -ne 4 ]]; then
    echo "\"getopt\" is not working."
    return 1
  fi
  
  PARSED=$(getopt --options     "o:g:s:v:h" \
		  --longoptions "operation:,configuration:,version:,help" \
		  --name "$0" -- "$@")

  if [[ $? -ne 0 ]]; then
      echo "\"getopt\" returned errors."
      return  2
  fi

  eval set -- "$PARSED"

  while true; do
      case "$1" in
	  -g|--configuration)
	      global_config_name="$2"
	      global_config_name_present=$yes
	      shift 2
	      ;;
	  -v|--version)
	      version_name="$2"
	      version_name_present=$yes
	      shift 2
	      ;;
	  -s|--source)
	      source_dir="$2"
	      source_dir_present=$yes
	      shift 2
	      ;;
	  -o|--operation)
	      operation_name="$2"
	      operation_name_present=$yes
	      shift 2
	      ;;

	  -d|--debug)
	      enable_debug_present=$yes
	      shift
	      ;;
	  -h|--help)
	      show_help_present=$yes
	      shift
	      ;;
	      
	  --)
	      shift
	      break
	      ;;
   	   *)
	      return 3
	      ;;
      esac
  done
  
  if  [[ "${show_help_present}" == "${yes}" ]]; then
     return 0
  fi
  
  if [[ "${operation_name}" == "-"* ]] \
  || [[ "${global_config_name}" == "-"* ]] \
  || [[ "${version_name}" == "-"* ]] \
  || [[ "${source_dir}" == "-"* ]] ;then  
    return 4
  fi
  
  if [[ "${operation_name_present}" == "${no}" ]] \
  || ( [[ "${operation_name}" != "import_global_config" ]] \
  && [[ "${operation_name}" != "export_global_config" ]] ); then
  printf "\nError: operation is unsupported or missing; operation=<${operation_name}>"
    return 5
  fi
  
  if [[ "${operation_name}" == "export_global_config" ]] \
    && [[ "${global_config_name_present}" == "${no}" ]]; then
      printf "\nError: export_global_config requires the configuration option"
      return 6
  fi

  if [[ "${operation_name}" == "export_global_config" ]] \
    && [[ "${version_name_present}" == "${yes}" ]]; then
      printf "\nError: export_global_config does not require the version option"
      return 6
  fi
  
  if [[ "${operation_name}" == "import_global_config" ]] \
    && ([[ "${global_config_name_present}" == "${no}" ]] || [[ "${version_name_present}" == "${no}" ]] ) ; then
      printf "\nError: import_global_config requires both the configuration and version options"
      return 6
  fi
  
  if [[ "${global_config_name_present}" == "${yes}" ]] \
    && ( [[ "${global_config_name}" =~ [^[:alnum:]] ]] \
    ||   [[ "${global_config_name}" =~ [^[:digit:]]$ ]] ); then
    printf "\nError: configuration must be an alphanumeric string and ending with a digit; configuration=<${global_config_name}>"
   return 7
  fi

  if [[ "${version_name_present}" == "${yes}" ]] \
    && ( [[ "${version_name}" =~ [^[:alnum:]] ]] \
    ||   [[ "${version_name}" =~ [^[:digit:]]$ ]] ); then
    printf "\nError: version must be an alphanumeric string and ending with a digit; version=<${version_name}>"
   return 7
  fi
  
  return 0
} 

function touch_log() {
conftool_log=${conftool_log_dir}/conftool-$(date +"%Y%m%d-%H%M%S")-${USER}.log
touch ${conftool_log}
}

function redirect_stdout_stderr_tolog(){  
conftool_log=${conftool_log_dir}/conftool-$(date +"%Y%m%d-%H%M%S")-${USER}.log
#exec 1<&-
#exec 2<&-

exec 3>&1 1>$conftool_log
exec 2>&1
}

function restore_stdout_stderr(){
#exec 1<&-
#exec 2<&-

exec 1>&3 3>&-
exec 2>&1
}

function export_global_config()
{
  arg_strings=($*)
  num_args=${#arg_strings[*]}
        
 if [ $num_args -lt 1 ]; then
   printf "\nError: invalid argument count in export_global_config(); arg_strings=\"${arg_strings[*]}\" $*"
   printf "\nUsage: export_global_config global-config"   
   return 1
 fi

 newconfig_dir=./newconfig

 if [[ "${source_dir_present}" == "${yes}" ]];then
   newconfig_dir=${source_dir}
 fi

 if [ -d "${newconfig_dir}" ]; then
  oldconfig_dir="${newconfig_dir}.$(date +"%Y%m%d-%H%M%S").saved"
  mv ${newconfig_dir} ${oldconfig_dir}
  printf "\nInfo: Saved old configuration into ${oldconfig_dir}"
 fi

 mkdir -p ${newconfig_dir}


 global_config_name=$1
 
 tmp_file_name=config.${global_config_name}.tar.bzip2.base64

 empty_search_result=$(conftool -o findconfigs  -f gui -g dummuy |grep \"search\")
 search_result=$(conftool -o findconfigs  -f gui -g ${global_config_name} |grep \"search\")
 
 if [[ "${search_result}" == "${empty_search_result}" ]];then
  printf "\nError: Failed to export config data; ${global_config_name} is missing in the database"
  return 3
 fi
 
 conftool -o globalconfload -d filesystem -f gui -g ${global_config_name} -r ${tmp_file_name}.out
 result_code=$? 
 if [[  $result_code -gt 0 ]]; then 
    printf "\nError: Failed to export config data"
    return $result_code
 fi 

 cat ${tmp_file_name}.out |cut -d ":" -f2 |cut -d "\"" -f2  |base64 -d |tar xjfv - -C ./ > /dev/null 2>&1

 cp -R  tmp/*/*  ${newconfig_dir} > /dev/null 2>&1

 rm -rf tmp ${tmp_file_name}.out > /dev/null 2>&1

 printf "\nInfo: Exported ${global_config_name} into ${newconfig_dir}"
 
}

function import_global_config()
{
  arg_strings=($*)
  num_args=${#arg_strings[*]}

 if [ $num_args -lt 1 ]; then
   printf "\nError: invalid argument count in import_global_config(); arg_strings=\"${arg_strings[*]}\" $*"
   printf "\nUsage: import_global_config global-config [version-name]"         
   return 1
 fi

 config_dir=config

 if [[ "${source_dir_present}" == "${yes}" ]];then
   config_dir=${source_dir}
 fi

 if [[ ! -d "$(pwd)/${config_dir}" ]]; then
  printf "\nError: ${config_dir} directory is missing in $(pwd)"
  return 2
 fi
 
 global_config_name=$1

 version_name=v${global_config_name}$(date +"%Y%m%d%H%M%S")
 
 if [ $num_args -eq 2 ]; then
  version_name=$2
 fi

 empty_search_result=$(conftool -o findconfigs  -f gui -g dummuy |grep \"search\")
 search_result=$(conftool -o findconfigs  -f gui -g ${global_config_name} |grep \"search\")
 
 if [[ "${search_result}" != "${empty_search_result}" ]];then
  printf "\nError: Failed to import config data; ${global_config_name} already exists in the database"
  return 3
 fi
 
 tmp_file_name=config.${global_config_name}.tar.bzip2.base64

 tar cjf - ${config_dir}/*  -C ${config_dir} |base64 --wrap=0 > ${tmp_file_name}

 conftool -o globalconfstore -d filesystem -f origin -v ${version_name} -g ${global_config_name} -s ${tmp_file_name} -r ~/result.json
 result_code=$? 
 if [[  $result_code -gt 0 ]]; then 
    printf "\nError: Failed to import config data"
 fi 
  
 rm ${tmp_file_name}

 search_result=$(conftool -o findconfigs  -f gui -g ${global_config_name} |grep \"search\")
 
 if [[ "${search_result}" == "${empty_search_result}" ]];then
  printf "\nError: Failed to import config data; no record found"
  return 4
 fi

 return $result_code
}

function was_ups_setup()
{ 
 if [ ! -e "$( printenv |grep SETUP_UPS |grep -Eo '/.+')/setup" ]; then
  printf "\nError: UPS environment was not setup"
  return 1
 fi
 
 source "$( printenv |grep SETUP_UPS |grep -Eo '/.+')/setup"
 
 return 0
}


function setup_artdaq_database()
{ 
  was_ups_setup

  if [[  $? -gt 0 ]]; then 
    printf "\nError: UPS environment must be setup first"
    return 1
  fi 

  if [ ! -z ${ARTDAQ_DATABASE_DIR+x} ]; then
    #printf "\nInfo: $(ups active |grep artdaq_database)"
    return 0
  fi
  
  if [ -z ${ARTDAQ_CORE_DIR+x} ]; then 
    printf "\nError: artdaq product is not setup"
    return 2
  fi
  
  artdaq_database_qqual=$(ups active |grep artdaq_core | grep -Eo "\-q[^-]+")
#  artdaq_database_version=$(ups list -aK+ artdaq_database ${artdaq_database_qqual} |sort -k 2 -r |head -n 1 | grep -Eo "\v[^\"]+")
  
  setup artdaq_database ${artdaq_database_version} ${artdaq_database_qqual}
  result_code=$? 
  if [[  $result_code -gt 0 ]]; then 
    printf "\nError: Unable to \"setup artdaq_database ${artdaq_database_version} ${artdaq_database_qqual} \""
    return 3
  fi 

  #printf "\nInfo: Configured $(ups active |grep artdaq_database)"  
  return $result_code
}

function setup_config_editor(){
  was_ups_setup

  if [[  $? -gt 0 ]]; then 
    printf "\nError: UPS environment must be setup first"
    return 1
  fi 

  if [ ! -z ${ARTDAQ_NODE_SERVER_DIR+x} ]; then
    #printf "\nInfo: $(ups active |grep artdaq_node_server)"
    return 0
  fi
  
  if [ -z ${ARTDAQ_CORE_DIR+x} ]; then 
    printf "\nError: artdaq product is not setup"
    return 2
  fi
  
  config_editor_qqual=$(ups active |grep artdaq_core | grep -Eo "\-q[^-]+")
  #config_editor_version=$(ups list -aK+ artdaq_node_server ${config_editor_qqual} |sort -k 2 -r |head -n 1 | grep -Eo "\v[^\"]+")
  setup artdaq_node_server  ${config_editor_version} ${config_editor_qqual}

  result_code=$? 
  if [[  $result_code -gt 0 ]]; then 
    printf "\nError: Unable to \"setup artdaq_node_server  ${config_editor_version} ${config_editor_qqual} \""
    return 3
  fi 

  #printf "\nInfo: Configured $(ups active |grep artdaq_node_server)"  
  return $result_code  
}


function main() 
{
  read_command_line_options $*
  if [[ $? -ne 0 ]]; then
    printf "\nError: wrong or incomplete options \"$*\""
    show_help
    exit 1
  fi

  if [[ "${show_help_present}" == "${yes}" ]];then
    show_help
    exit 0
  fi

  setup_config_editor
  if [[  $? -gt 0 ]]; then 
    touch_log
    printf "\nInfo: Exiting..."
    return 1
  fi

  unset ARTDAQ_DATABASE_URI

  redirect_stdout_stderr_tolog
  
  printf "\nInfo: operation=${operation_name}, configuration=${global_config_name}, "
  
  if [[ "${version_name_present}" == "${yes}" ]] ; then
    printf "version=${version_name}, "
  fi

  if [[ "${source_dir_present}" == "${yes}" ]] ; then
    printf "source=${source_dir}"
  fi

  printf "\n"

  printf "\nInfo: using $(which conftool)"
  
  export ARTDAQ_DATABASE_DATADIR=/home/nfs/dunedaq/daqarea/databases
  
  printf "\n"
  
  if [[ "${operation_name}" == "import_global_config" ]]; then    
    import_global_config ${global_config_name} ${version_name}
    result_code=$? 
  elif [[ "${operation_name}" == "export_global_config" ]]; then
    export_global_config ${global_config_name}
    result_code=$? 
  fi 
  
  printf "\n"
  
  restore_stdout_stderr
  
  return $result_code
}

main $*
result_code=$?
if [[  $result_code -gt 0 ]]; then 
  printf "\nReturn status: failed\n"  
  cat ${conftool_log_dir}/$(ls -t  ${conftool_log_dir} |head -1)
  exit 1
fi 

printf "\nReturn status: succeed\n"
exit 0