#/bin/bash

daqswdir=/home/pduser/daqsw

artdaq_database_ver=v1_04_04

daqsw_qqual="e10:prof:s41"

if [ ! -d "${daqswdir}" ]; then
	echo ${daqswdir} is missing.
	exit 1
fi

if [ ! -d "${daqswdir}/databases/logs" ]; then
	mkdir -p ${daqswdir}/databases/logs
fi


rm -rf ${daqswdir}/TRASH/*
rm -rf ${daqswdir}/db/*
rm -rf ${daqswdir}/tmp/*

artdaqwebguilog=${daqswdir}/databases/logs/artdaqwebgui-$(date +"%Y%m%d-%H%M%S").log

source ${daqswdir}/products/setup > ${artdaqwebguilog}  2>&1

setup artdaq_database  ${artdaq_database_ver}  -q ${daqsw_qqual} > ${artdaqwebguilog}  2>&1
setup nodejs            v4_5_0     > ${artdaqwebguilog}  2>&1

ups active

export ARTDAQ_DATABASE_DATADIR=${daqswdir}/databases

export ARTDAQ_DATABASE_URI="filesystemdb://${daqswdir}/databases/filesystemdb/test_configuration_db"
#export ARTDAQ_DATABASE_URI="mongodb://127.0.0.1:27017/test_configuration_db"


echo "ARTDAQ_DATABASE_URI=$ARTDAQ_DATABASE_URI" > ${artdaqwebguilog}  2>&1

nodejs=$(which node)

echo "nodejs=$nodejs" > ${artdaqwebguilog}  2>&1

nodejs_app="${daqswdir}/databases/artdaq-utilities-node-server/serverbase.js"

gobackdir=$(pwd)

cd $(dirname ${nodejs_app})
nohup $nodejs $(basename ${nodejs_app}) > ${artdaqwebguilog}  2>&1 & 
echo $! > ${daqswdir}/databases/artdaqwebgui.pid

cd ${gobackdir}
