#!/bin/bash

database_work_dir=${HOME}/work-db-v4-dir

source /software/products/setup

setup artdaq_database v1_04_82 -q e17:prof:s83

export ARTDAQ_DATABASE_URI="mongodb://experiment:$password@192.168.1.1:28047,192.168.1.2:28047/experiment_db?replicaSet=rs0&authSource=admin"

export LD_LIBRARY_PATH=$MONGODB_FQ_DIR/lib64:$LD_LIBRARY_PATH

if [ ! -d ${database_work_dir} ]; then 
  mkdir -p ${database_work_dir}
fi

#cd ${database_work_dir}

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
export ARTDAQ_DATABASE_CONFDIR=/data/artdaq_database/config
#cd $database_work_dir
echo
echo
echo "Listing RunConfigurations available in artdaq_database:"
conftool.py getListOfAvailableRunConfigurations
echo 
echo 
echo "Instructions: https://cdcvs.fnal.gov/redmine/projects/artdaq-utilities/wiki/Artdaq-config-conftool"
echo 
echo 
