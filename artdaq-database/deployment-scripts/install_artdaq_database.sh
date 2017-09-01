#!/bin/bash
#----------------------------------------------------------------
# Configuration parameters
#----------------------------------------------------------------
ARTDAQ_UPS_QUAL="e14:prof:s50"
WEBEDITOR_UPS_VER=v1_00_09a
ARTDAQ_DB_UPS_VER=v1_04_27

ARTDAQ_BASE_DIR=/nfs/sw/artdaq
ACTIVE_DATABASEBASE_NAME=cern_pddaq_v3x_db

#----------------------------------------------------------------
# Optional configuration parameters
#----------------------------------------------------------------
WEBEDITOR_UPS_QUAL=${ARTDAQ_UPS_QUAL}
ARTDAQ_DB_UPS_QUAL=${ARTDAQ_UPS_QUAL}

INACTIVE_DATABASEBASES="cern_pddaq_v2_db cern_pddaq_v3_db"

#ARTDAQ_BASE_DIR=/nfs/rscratch/daq/artdaq

DATABASE_BASE_DIR=$(dirname ${ARTDAQ_BASE_DIR})/database
PRODUCTS_BASE_DIR=${ARTDAQ_BASE_DIR}/products
#PRODUCTS_BASE_DIR=/scratch/products

MONGOD_PORT=27037
WEBEDITOR_BASE_PORT=8880

#----------------------------------------------------------------
# Implementaion
#----------------------------------------------------------------
rc_success=0 
rc_failure=1

timestamp=$(date -d "today" +"%Y%m%d%H%M%S")
dblist=(${INACTIVE_DATABASEBASES} ${ACTIVE_DATABASEBASE_NAME})

run_as_user=$(id -u -n )
run_as_group=$(id -g -n ${run_as_user})

source ${PRODUCTS_BASE_DIR}/setup
unsetup_all  >/dev/null 2>&1 

printf "Info: Configuring artdaq_database services to run using the following credentials:\n"
printf "\t\tuser=${run_as_user}, group=${run_as_group}\n"

user_prompts=false

function have_artdaq_database() {
#----------------------------------------------------------------
#----------------------------------------------------------------
printf "\nInfo: Verifying availability of artdaq_database\n"
#----------------------------------------------------------------

if [ -z ${ACTIVE_DATABASEBASE_NAME+x} ]; then
        printf "Error: ACTIVE_DATABASEBASE_NAME is unset. Aborting.\n";return $rc_failure;  else
        printf "Info: ACTIVE_DATABASEBASE_NAME is set to '${ACTIVE_DATABASEBASE_NAME}'\n"
fi

if [ -z ${ARTDAQ_DB_UPS_VER+x} ]; then
        printf "Error: ARTDAQ_DB_UPS_VER is unset. Aborting.\n";return $rc_failure;  else
        printf "Info: ARTDAQ_DB_UPS_VER is set to '${ARTDAQ_DB_UPS_VER}'\n"
fi

if [ -z ${ARTDAQ_DB_UPS_QUAL+x} ]; then
        printf "Error: ARTDAQ_DB_UPS_QUAL is unset. Aborting.\n";return $rc_failure;  else
        printf "Info: ARTDAQ_DB_UPS_QUAL is set to '${ARTDAQ_DB_UPS_QUAL}'\n"
fi

setup artdaq_database ${ARTDAQ_DB_UPS_VER} -q ${ARTDAQ_DB_UPS_QUAL} 
RC=$?
if [ $RC -ne 0 ]; then
	printf "Error: Failed setting artdaq_database. Aborting.\n"; return $rc_failure;
fi

conftool_bin=$(command -v conftool)
if [ -z "conftool_bin" ] &&  [ ! -x ${conftool_bin} ]; then
	printf "Error: conftool was not setup. Aborting.\n"; return $rc_failure; else
        printf "Info: conftool found: '${conftool_bin}'\n"
fi

return $rc_success
}

function have_artdaq_node_server(){
#----------------------------------------------------------------
#----------------------------------------------------------------
printf "\nInfo: Verifying availability of artdaq_node_server\n"
#----------------------------------------------------------------

if [ -z ${WEBEDITOR_UPS_VER+x} ]; then
        printf "Error: WEBEDITOR_UPS_VER is unset. Aborting.\n";return $rc_failure;  else
        printf "Info: WEBEDITOR_UPS_VER is set to '${WEBEDITOR_UPS_VER}'\n"
fi

if [ -z ${WEBEDITOR_UPS_QUAL+x} ]; then
        printf "Error: WEBEDITOR_UPS_QUAL is unset. Aborting.\n";return $rc_failure;  else
        printf "Info: WEBEDITOR_UPS_QUAL is set to '${WEBEDITOR_UPS_QUAL}'\n"
fi

setup artdaq_node_server ${WEBEDITOR_UPS_VER} -q ${WEBEDITOR_UPS_QUAL} 
RC=$?
if [ $RC -ne 0 ]; then
	printf "Error: Failed setting artdaq_node_server. Aborting.\n"; return $rc_failure;
fi

node_bin=$(command -v node)
if [ -z "$node_bin" ] &&  [ ! -x ${node_bin} ]; then
	printf "Error: node was not setup. Aborting.\n"; return $rc_failure; else
        printf "Info: node found: '${node_bin}'\n"
fi

return $rc_success
}

function disable_services(){
local disable_services_file="/tmp/disable_services-${timestamp}.sh"
#----------------------------------------------------------------
#----------------------------------------------------------------
printf "\nInfo: Creating ${disable_services_file}\n"
#----------------------------------------------------------------

echo "#!/bin/bash" > ${disable_services_file}
for db in ${dblist[@]}
  do for action in stop disable 
    do for service in webconfigeditor mongodbserver
      do echo systemctl ${action} ${service}@${db}.service >> ${disable_services_file}
    done
  done
done
chmod a+x  ${disable_services_file}
printf "#-----------------------file contents begin----------------------\n"
cat ${disable_services_file}
printf "#-----------------------file contents end------------------------\n"

printf "\nInfo: Open another shell and run ${disable_services_file} as root.\n"

while $user_prompts; do
    read -p "Did you run ${disable_services_file} as root?" yn
    case $yn in
        [Yy]* ) break;;
        [Nn]* ) return $rc_failure;;
        * ) echo "Please answer yes or no.";;
    esac
done

return $rc_success
}

function create_database_dirs(){
#----------------------------------------------------------------
#----------------------------------------------------------------
printf "\nInfo: Configuring artdaq_database and artdaq_node_server\n"
#----------------------------------------------------------------
if [ -d ${DATABASE_BASE_DIR} ]; then
  mv ${DATABASE_BASE_DIR} ${DATABASE_BASE_DIR}.${timestamp}
  RC=$?
  if [ $RC -ne 0 ]; then
    printf "Error: Failed moving ${DATABASE_BASE_DIR} to ${DATABASE_BASE_DIR}.${timestamp}  Aborting.\n"; return $rc_failure;else
    printf "Info: Moved ${DATABASE_BASE_DIR} to ${DATABASE_BASE_DIR}.${timestamp}\n"
  fi
fi

mkdir -p ${DATABASE_BASE_DIR}
RC=$?
if [ $RC -ne 0 ]; then
    printf "Error: Failed creating ${DATABASE_BASE_DIR}. Aborting.\n"; return $rc_failure; else
    printf "Info: Created ${DATABASE_BASE_DIR}"
fi

mkdir -p ${DATABASE_BASE_DIR}/${ACTIVE_DATABASEBASE_NAME}/{systemd,var/tmp,logs,data,webconfigeditor}
RC=$?
if [ $RC -ne 0 ]; then
    printf "Error: Failed creating subdirectories in ${DATABASE_BASE_DIR}. Aborting.\n"; return $rc_failure; else
    printf "Info: Created subdirectories in ${DATABASE_BASE_DIR}:\n"
    printf "$(find ${DATABASE_BASE_DIR}/${ACTIVE_DATABASEBASE_NAME} -type d -print)\n"
fi

return $rc_success
}


function create_database_configs(){
#----------------------------------------------------------------
#-----------------------mongod.env-------------------------------
filename="${DATABASE_BASE_DIR}/${ACTIVE_DATABASEBASE_NAME}/mongod.env"
#----------------------------------------------------------------
touch ${filename}
RC=$?
if [ $RC -ne 0 ]; then
    printf "Error: Failed creating ${filename}. Aborting.\n"; return $rc_failure;
fi
cat <<EOF > ${filename}
MONGOD_DATABASE_NAME=${ACTIVE_DATABASEBASE_NAME}
MONGOD_BASE_DIR=${DATABASE_BASE_DIR}
MONGOD_PORT=${MONGOD_PORT}
MONGOD_UPS_VER=v3_4_6
MONGOD_UPS_QUAL="e14:prof"
#MONGOD_NUMA_CTRL="numactl --interleave=all"
EOF
RC=$?
if [ $RC -ne 0 ]; then
    printf "Error: Failed creating ${filename}. Aborting.\n"; return $rc_failure; else
    printf "\nInfo: Created  ${filename}\n"
    printf "#-----------------------file contents begin----------------------\n"
    cat ${filename}
    printf "#-----------------------file contents end------------------------\n"
fi

#----------------------------------------------------------------
#-----------------------webconfigeditor.env----------------------
filename="${DATABASE_BASE_DIR}/${ACTIVE_DATABASEBASE_NAME}/webconfigeditor.env"
#----------------------------------------------------------------
touch ${filename}
RC=$?
if [ $RC -ne 0 ]; then
    printf "Error: Failed creating ${filename}. Aborting.\n"; return $rc_failure;
fi

cat <<EOF > ${filename}
MONGOD_DATABASE_NAME=${ACTIVE_DATABASEBASE_NAME}
MONGOD_BASE_DIR=${DATABASE_BASE_DIR}
MONGOD_PORT=${MONGOD_PORT}
WEBEDITOR_BASE_PORT=${WEBEDITOR_BASE_PORT}
WEBEDITOR_UPS_VER=${WEBEDITOR_UPS_VER}
WEBEDITOR_UPS_QUAL=${WEBEDITOR_UPS_QUAL}
EOF
RC=$?
if [ $RC -ne 0 ]; then
    printf "Error: Failed creating ${filename}. Aborting.\n"; return $rc_failure; else
    printf "\nInfo: Created  ${filename}\n"
    printf "#-----------------------file contents begin----------------------\n"
    cat ${filename}
    printf "#-----------------------file contents end------------------------\n"
fi

return $rc_success
}

function create_database_services(){
printf "Info: Configuring artdaq_database services to run using the following credentials:\n"
printf "\t\tuser=${run_as_user}, group=${run_as_group}\n";

#----------------------------------------------------------------
#-----------------------webconfigeditor@.service-----------------
filename="${DATABASE_BASE_DIR}/${ACTIVE_DATABASEBASE_NAME}/systemd/webconfigeditor@${ACTIVE_DATABASEBASE_NAME}.service"
#----------------------------------------------------------------
touch ${filename}
RC=$?
if [ $RC -ne 0 ]; then
    printf "Error: Failed creating ${filename}. Aborting.\n"; return $rc_failure;
fi
cat <<EOF > ${filename}
# Quick HowTo:
# 1. Copy this file to /etc/systemd/system/webconfigeditor@${ACTIVE_DATABASEBASE_NAME}.service
# 2. Run "systemctl daemon-reload"
# 3. Run "systemctl enable webconfigeditor@${ACTIVE_DATABASEBASE_NAME}.service"
# 5. Run "systemctl start webconfigeditor@${ACTIVE_DATABASEBASE_NAME}.service"
# 
# 6. Check status "systemctl status webconfigeditor@${ACTIVE_DATABASEBASE_NAME}.service"
# 7. Stop "systemctl stop webconfigeditor@${ACTIVE_DATABASEBASE_NAME}.service"

[Unit]
Description=WebConfigEditor service
After=syslog.target network.target

[Service]
Type=forking
User=${run_as_user}
Group=${run_as_group}

EnvironmentFile=${DATABASE_BASE_DIR}/%i/webconfigeditor.env
#Environment=NODE_ENV=production
#ExecStartPre=${DATABASE_BASE_DIR}/webconfigeditor-ctrl.sh clean 
ExecStart=${DATABASE_BASE_DIR}/webconfigeditor-ctrl.sh start 
ExecStop=${DATABASE_BASE_DIR}/webconfigeditor-ctrl.sh stop 
#ExecStopPost=${DATABASE_BASE_DIR}/webconfigeditor-ctrl.sh clean 
PIDFile=${DATABASE_BASE_DIR}/%i/var/tmp/webconfigeditor.pid


Restart=always
RestartSec=120

[Install]
WantedBy=multi-user.target

EOF
RC=$?
if [ $RC -ne 0 ]; then
    printf "Error: Failed creating ${filename}. Aborting.\n"; return $rc_failure; else
    printf "\nInfo: Created  ${filename}\n"
    printf "#-----------------------file contents begin----------------------\n"
    cat ${filename}
    printf "#-----------------------file contents end------------------------\n"
fi


#----------------------------------------------------------------
#-----------------------mongodbserver@.service-----------------
filename="${DATABASE_BASE_DIR}/${ACTIVE_DATABASEBASE_NAME}/systemd/mongodbserver@${ACTIVE_DATABASEBASE_NAME}.service"
#----------------------------------------------------------------
touch ${filename}
RC=$?
if [ $RC -ne 0 ]; then
    printf "Error: Failed creating ${filename}. Aborting.\n"; return $rc_failure;
fi
cat <<EOF > ${filename}
# The mongodbserver service unit file
#
# Quick HowTo:
# 1. Copy this file to /etc/systemd/system/mongodbserver@${ACTIVE_DATABASEBASE_NAME}.service
# 2. Run "systemctl daemon-reload"
# 3. Run "systemctl enable mongodbserver@${ACTIVE_DATABASEBASE_NAME}.service"
# 5. Run "systemctl start mongodbserver@${ACTIVE_DATABASEBASE_NAME}.service"
# 
# 6. Check status "systemctl status mongodbserver@${ACTIVE_DATABASEBASE_NAME}.service"
# 7. Stop "systemctl stop mongodbserver@${ACTIVE_DATABASEBASE_NAME}.service"


[Unit]
Description=Mongo database service
After=syslog.target network.target

[Service]
Type=forking
User=${run_as_user}
Group=${run_as_group}

EnvironmentFile=${DATABASE_BASE_DIR}/%i/mongod.env
#ExecStartPre=${DATABASE_BASE_DIR}/mongod-ctrl.sh clean 
ExecStart=${DATABASE_BASE_DIR}/mongod-ctrl.sh start 
ExecStop=${DATABASE_BASE_DIR}/mongod-ctrl.sh stop 
#ExecStopPost=${DATABASE_BASE_DIR}/mongod-ctrl.sh clean 
PIDFile=${DATABASE_BASE_DIR}/%i/var/tmp/mongod.pid


Restart=always
RestartSec=300

[Install]
WantedBy=multi-user.target


EOF
RC=$?
if [ $RC -ne 0 ]; then
    printf "Error: Failed creating ${filename}. Aborting.\n"; return $rc_failure; else
    printf "\nInfo: Created  ${filename}\n"
    printf "#-----------------------file contents begin----------------------\n"
    cat ${filename}
    printf "#-----------------------file contents end------------------------\n"
fi

return $rc_success
}

function copy_shell_scripts(){
#----------------------------------------------------------------
#-----------------------copy shell scripts-----------------------
filenames=(${ARTDAQ_DATABASE_DIR}/deployment-scripts/artdaq-database/\
{initd_functions,backup_artdaq_database.sh,webconfigeditor-ctrl.sh,mongod-ctrl.sh})
#----------------------------------------------------------------
for filename in ${filenames[@]}
do 
    if [ ! -f ${filename} ]; then
        printf "Error: ${filename} not found! Aborting.\n"; return $rc_failure ; else
        cp ${filename} ${DATABASE_BASE_DIR}
        printf "Info: coppied ${filename} to ${DATABASE_BASE_DIR}"
    fi
done
chmod a+rx ${DATABASE_BASE_DIR}/*.sh >/dev/null 2>&1
find ${DATABASE_BASE_DIR} -name  "*.sh" -type f -print |\
  xargs -n 1 sed -i "s|/daq/artdaq|${ARTDAQ_BASE_DIR}|g; \
		     s|/daq/database|${DATABASE_BASE_DIR}|g;\
		     s|cern_pddaq_v2_db|${ACTIVE_DATABASEBASE_NAME}|g"
RC=$?
if [ $RC -ne 0 ]; then
    printf "Error: Failed migrating systemd shell scripts. Aborting.\n"; return $rc_failure;else
    printf "Info: Migrated systemd shell scripts $(find ${DATABASE_BASE_DIR} -name  "*.sh" -type f -print)\n"
fi

return $rc_success
}

function start_mongod_instance(){
printf "Info: Starting mongod server instance\n"

source <(sed -E -n 's/[^#]+/export &/ p'  ${DATABASE_BASE_DIR}/${ACTIVE_DATABASEBASE_NAME}/mongod.env)

${DATABASE_BASE_DIR}/mongod-ctrl.sh start
RC=$?
if [ $RC -ne 0 ]; then
    printf "Error: Failed calling mongod-ctrl.sh start. Aborting.\n"; return $rc_failure;
fi

return $rc_success
}

function start_webeditor_instance(){
printf "Info: Starting webconfigeditor instance\n"

source <(sed -E -n 's/[^#]+/export &/ p'  ${DATABASE_BASE_DIR}/${ACTIVE_DATABASEBASE_NAME}/webconfigeditor.env)

${DATABASE_BASE_DIR}/webconfigeditor-ctrl.sh start
RC=$?
if [ $RC -ne 0 ]; then
    printf "Error: Failed calling webconfigeditor-ctrl.sh start. Aborting.\n"; return $rc_failure;
fi

return $rc_success
}


function check_mongod_instance(){
export ARTDAQ_DATABASE_URI="mongodb://127.0.0.1:${MONGOD_PORT}/cern_pddaq_db"
source <(sed -E -n 's/[^#]+/export &/ p'  ${DATABASE_BASE_DIR}/${ACTIVE_DATABASEBASE_NAME}/mongod.env)

failed_test_count=0
conftool_commands=(readDatabaseInfo listDatabases)
for conftool_command in ${conftool_commands[@]};do 
conftool_response=$(conftool.py ${conftool_command})
RC=$?
if [ $RC -ne 0 ]; then
    failed_test_count=$((failed_test_count+1))
    printf "Error: Failed calling conftool.py ${conftool_command} "; 
    printf "#-----------------------conftool.py ${conftool_command} begin---------------------\n"
    echo ${conftool_response}
    printf "#-----------------------conftool.py ${conftool_command} end-----------------------\n"
else
    printf "#-----------------------conftool.py ${conftool_command} begin---------------------\n"
    echo ${conftool_response}
    printf "#-----------------------conftool.py ${conftool_command} end-----------------------\n"
fi
done

if [ $failed_test_count -ne 0 ]; then
   printf "Error: Failed connecting to the running database instance.\n"
   return $rc_failure
fi

return $rc_success
}

function check_webeditor_instance(){
printf "Info: Checking webconfigeditor instance\n"
source <(sed -E -n 's/[^#]+/export &/ p'  ${DATABASE_BASE_DIR}/${ACTIVE_DATABASEBASE_NAME}/webconfigeditor.env)

${DATABASE_BASE_DIR}/webconfigeditor-ctrl.sh status
RC=$?
if [ $RC -ne 0 ]; then
   printf "Error: Failed connecting to the running webconfigeditor instance.\n"
   return $rc_failure
fi

return $rc_success
}


function run_conftool_tests(){
export ARTDAQ_DATABASE_URI="mongodb://127.0.0.1:${MONGOD_PORT}/cern_pddaq_db"
  
rm -rf /tmp/artdaqdb_test_data-*

printf "\nInfo: Initializing database\n"

tmpdir="/tmp/artdaqdb_test_data-${timestamp}"
for testfile in $(find ${ARTDAQ_DATABASE_DIR}/testdata -name  "np04_teststand*.taz" -type f -print);do 
  tmpname=$(basename ${testfile})
  tmpname=${tmpname%.taz}
  
  printf "\nInfo: Unpacking ${tmpname} to ${tmpdir}/${tmpname}\n"
  mkdir -p ${tmpdir}/${tmpname}; cd ${tmpdir}/${tmpname}
  tar xfz ${testfile} -C ${tmpdir}/${tmpname}
  cp ${ARTDAQ_DATABASE_FQ_DIR}/conf/schema.fcl ${tmpdir}/${tmpname}/

  printf "Info: Importing test configuration ${tmpname} from ${tmpdir}/${tmpname}\n"
  conftool.py importConfiguration ${tmpname}
  RC=$?
  if [ $RC -ne 0 ]; then
    printf "Error: Failed calling conftool.py importConfiguration ${tmpname}. "
  fi
  
  printf "Info: Exporting test configuration  ${tmpname}00001 to ${tmpdir}/${tmpname}-export\n"
  mkdir -p ${tmpdir}/${tmpname}-export
  cd ${tmpdir}/${tmpname}-export

  conftool.py exportConfiguration ${tmpname}00001
  RC=$?
  if [ $RC -ne 0 ]; then
    printf "Error: Failed calling conftool.py exportConfiguration ${tmpname}00001.\n"
  fi
done

printf "\nInfo: Running conftool.py tests\n"
  
test1=("\"listCollections\"" "\"['SystemLayout', 'RCEs', 'SSPs', 'expert_options', 'Components', 'Aggregators', 'EventBuilders', 'common_code']\"")
test2=("\"listDatabases\"" "\"['cern_pddaq_db']\"")
test3=("\"getListOfAvailableRunConfigurationPrefixes\"" "\"['np04_teststand_tests']\"")
test4=("\"getListOfAvailableRunConfigurations\"" "\"['np04_teststand_tests00001']\"")
test5=("\"getListOfAvailableRunConfigurations np04_teststand\"" "\"['np04_teststand_tests00001']\"")


tests=(test1[@] test2[@] test3[@] test4[@] test5[@])
test_cout=${#tests[@]};failed_test_count=0
for ((i=0; i<${test_cout}; i++)); do
  tmp=${!tests[i]}
  conftool_command=$(echo $tmp |  cut -d\" -f2)
  conftool_expected_responce=$(echo $tmp |  cut -d\" -f4)
  
  printf "\nTest #$i \"conftool.py $conftool_command\"\n"
  conftool_response=$(conftool.py ${conftool_command})
  RC=$?
  if [ $RC -ne 0 ]; then
    printf "Error: Failed calling conftool.py ${conftool_command}\n"
    printf "#-----------------------conftool.py ${conftool_command} begin---------------------\n"
    printf "${conftool_response}\n"
    printf "#-----------------------conftool.py ${conftool_command} end-----------------------\n"
    failed_test_count=$((failed_test_count+1))
  else
    if [ ! "$conftool_expected_responce" == "$conftool_response" ]; then
       failed_test_count=$((failed_test_count+1))
       printf "Warning: Wrong responce\n"
       printf "#----------------------- expected responce begin----------------------------------\n"
       echo "${conftool_expected_responce}\n"
       printf "#----------------------- expected responce end------------------------------------\n"
    fi
    printf "#-----------------------conftool.py ${conftool_command} begin---------------------\n"
    echo "${conftool_response}\n"
    printf "#-----------------------conftool.py ${conftool_command} end-----------------------\n"
  fi
done  

if [ $failed_test_count -ne 0 ]; then
   printf "Error: Deployment error, $failed_test_count test(s) failed.\n"
   return $rc_failure
fi

return $rc_success
}

function stop_mongod_instance(){
printf "Info: Stopping mongod server instance\n"
source <(sed -E -n 's/[^#]+/export &/ p'  ${DATABASE_BASE_DIR}/${ACTIVE_DATABASEBASE_NAME}/mongod.env)

${DATABASE_BASE_DIR}/mongod-ctrl.sh stop
RC=$?
if [ $RC -ne 0 ]; then
    printf "Error: Failed calling mongod-ctrl.sh stop.\n"
    killall mongod
    printf "Info: Killed all mongod running instances.\n"
fi

return $rc_success
}

function stop_webeditor_instance(){
printf "Info: Stopping webconfigeditor instance\n"
source <(sed -E -n 's/[^#]+/export &/ p'  ${DATABASE_BASE_DIR}/${ACTIVE_DATABASEBASE_NAME}/webconfigeditor.env)

${DATABASE_BASE_DIR}/webconfigeditor-ctrl.sh stop
RC=$?
if [ $RC -ne 0 ]; then
    printf "Error: Failed calling webconfigeditor-ctrl.sh stop.\n"
    killall node
    printf "Info: Killed all node running instances.\n"
fi

return $rc_success
}

function enable_services(){
local enable_services_file="/tmp/enable_services-${timestamp}.sh"
#----------------------------------------------------------------
#----------------------------------------------------------------
printf "\nInfo: Creating ${enable_services_file}\n"
#----------------------------------------------------------------
echo "#!/bin/bash" > ${enable_services_file}
for service in webconfigeditor mongodbserver; do
  echo "cp ${DATABASE_BASE_DIR}/${ACTIVE_DATABASEBASE_NAME}/systemd/${service}@${ACTIVE_DATABASEBASE_NAME}.service /etc/systemd/system/" >> ${enable_services_file}
  echo "systemctl daemon-reload" >> ${enable_services_file}  
  for action in enable start ; do
      echo systemctl ${action} ${service}@${ACTIVE_DATABASEBASE_NAME}.service >> ${enable_services_file}
  done
done

chmod a+x  ${enable_services_file}
printf "#-----------------------file contents begin----------------------\n"
cat ${enable_services_file}
printf "#-----------------------file contents end------------------------\n"

printf "\nInfo: Open another shell and run ${enable_services_file} as root.\n"

while $user_prompts; do
    read -p "Did you run ${enable_services_file} as root?" yn
    case $yn in
        [Yy]* ) break;;
        [Nn]* ) return $rc_failure;;
        * ) echo "Please answer yes or no.";;
    esac
done

return $rc_success
}
function main_program(){
printf "\nInfo: Running main_program\n"

have_artdaq_database
if [[ $? -ne $rc_success ]]; then 
   echo -e "\e[31;7;5mError: artdaq_database product is not installed.\e[0m"; return $rc_failure
fi

have_artdaq_node_server
if [[ $? -ne $rc_success ]]; then 
   echo -e "\e[31;7;5mError: have_artdaq_node_server product is not installed.\e[0m"; return $rc_failure
fi

disable_services
if [[ $? -ne $rc_success ]]; then 
   echo -e "\e[31;7;5mError: failed disabling services.\e[0m"; return $rc_failure
fi

create_database_dirs
if [[ $? -ne $rc_success ]]; then 
   echo -e "\e[31;7;5mError: failed creating working database directories.\e[0m"; return $rc_failure
fi

create_database_configs
if [[ $? -ne $rc_success ]]; then 
   echo -e "\e[31;7;5mError: failed creating database config files.\e[0m"; return $rc_failure
fi

create_database_services
if [[  $? -ne $rc_success ]]; then 
   echo -e "\e[31;7;5mError: failed creating database service configuration files.\e[0m"; return $rc_failure
fi

copy_shell_scripts
if [[  $? -ne $rc_success ]]; then 
   echo -e "\e[31;7;5mError: failed copping database shell scripts.\e[0m"; return $rc_failure
fi

start_mongod_instance
if [[  $? -ne $rc_success ]]; then
  echo -e "\e[31;7;5mError: failed starting mongo database.\e[0m"
  stop_mongod_instance
  return $rc_failure; else
  
  check_mongod_instance
  if [[  $? -ne $rc_success ]]; then 
      echo -e "\e[31;7;5mError: failed accessing mongo database.\e[0m"
      stop_mongod_instance
      return $rc_failure
  fi 
fi

run_conftool_tests
if [[  $? -ne $rc_success ]]; then 
   echo -e "\e[31;7;5mError: Deployment error.\e[0m"
   stop_mongod_instance
   return $rc_failure
fi

start_webeditor_instance
if [[  $? -ne $rc_success ]]; then 
   echo -e "\e[31;7;5mError: failed starting webconfigeditor.\e[0m"
   stop_webeditor_instance
   stop_mongod_instance
   return $rc_failure;else
   
   check_webeditor_instance
   if [[  $? -ne $rc_success ]]; then 
      echo -e "\e[31;7;5mError: failed accessing webconfigeditor.\e[0m"
      stop_webeditor_instance
      stop_mongod_instance
      return $rc_failure
   fi 
fi

local error_count=0

stop_webeditor_instance
if [[  $? -ne $rc_success ]]; then 
   echo -e "\e[31;7;5mError: failed stopping webconfigeditor.\e[0m"
   error_count=$((error_count+1))

fi 

stop_mongod_instance
if [[  $? -ne $rc_success ]]; then 
   echo -e "\e[31;7;5mError: failed stopping mongo database.\e[0m"
   error_count=$((error_count+1))
fi 

if [ $error_count -ne 0 ]; then
   echo -e "\e[31;7;5mError: Some of the services did not stop gracefully.\e[0m"
   return $rc_failure
fi

enable_services
if [[ $? -ne $rc_success ]]; then 
   echo -e "\e[31;7;5mError: failed enabling services.\e[0m"; return $rc_failure
fi

check_webeditor_instance
if [[  $? -ne $rc_success ]]; then 
    echo -e "\e[31;7;5mError: failed accessing webconfigeditor.\e[0m"
    return $rc_failure
fi 

echo -e "\e[0;7;5mInfo: Installation succeeded.\e[0m"

return $rc_success
}

function pull_products(){
cd /nfs/sw/artdaq/download  
wget http://scisoft.fnal.gov/scisoft/bundles/tools/pullProducts
chmod a+x pullProducts 

cp artdaq_database-1.04.27-Linux64bit+3.10-2.17-s50-e14-*_MANIFEST.txt /nfs/sw/artdaq/download
./pullProducts -l ../products slf7 artdaq_database-v1_04_27 s50-e14 debug
./pullProducts -l ../products slf7 artdaq_database-v1_04_27 s50-e14 prof

source /nfs/sw/artdaq/products/setup
setup artdaq_database v1_04_27 -q e14:prof:s50 
}

main_program
exit $?