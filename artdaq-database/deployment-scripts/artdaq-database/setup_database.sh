#!/bin/bash

artdaq_swroot_dir=${HOME}/daqsw

current_artdaq_database_uri="mongodb://127.0.0.1:27037/artdaq_db"
#current_artdaq_database_uri="mongodb://127.0.0.1:27037/fnal_icarus_db"
#current_artdaq_database_uri="mongodb://127.0.0.1:27037/cern_pddaq_db"

database_work_dir=${artdaq_swroot_dir}/database/work-db-v3-dir

#source /grid/fermiapp/products/artdaq/setup
#source /cvmfs/fermilab.opensciencegrid.org/products/artdaq/setup
source ${artdaq_swroot_dir}/products/setup

setup artdaq_database v1_04_48 -q e14:prof:s50

export ARTDAQ_DATABASE_URI=${current_artdaq_database_uri}


if [ ! -d ${database_work_dir} ]; then 
  mkdir -p ${database_work_dir}
fi

cd ${database_work_dir}

conftool.py

_complete_conftool(){
  local current_string=${COMP_WORDS[COMP_CWORD]}
  local complete_list=$(conftool.py |grep -v Usage |grep conftool |cut -d ' ' -f 3)

  if [  ${COMP_WORDS[COMP_CWORD-1]} = "exportConfiguration" ];then
    complete_list=$(conftool.py getListOfAvailableRunConfigurations)
  elif [  ${COMP_WORDS[COMP_CWORD-1]} = "getListOfAvailableRunConfigurations" ];then
    complete_list=$(conftool.py getListOfAvailableRunConfigurationPrefixes)
  elif [  ${COMP_WORDS[COMP_CWORD-1]} = "importConfiguration" ];then
    complete_list=$(conftool.py getListOfAvailableRunConfigurationPrefixes)
  fi
  
  COMPREPLY=($(compgen -W '${complete_list[@]}' -- "$current_string"))
  return 0
}

complete -F _complete_conftool conftool.py
