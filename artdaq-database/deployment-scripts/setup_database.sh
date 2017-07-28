#!/bin/bash

database_work_dir=/daq/database/databases/work-db-v3-dir

#source /cvmfs/fermilab.opensciencegrid.org/products/artdaq/setup
source /daq/artdaq/products/setup
setup artdaq_database v1_04_26 -q e14:prof:s48

export ARTDAQ_DATABASE_URI="mongodb://127.0.0.1:27027/cern_pddaq_db"


if [ ! -d ${database_work_dir} ]; then 
  mkdir -p ${database_work_dir}
fi

cd ${database_work_dir}

conftool.py
