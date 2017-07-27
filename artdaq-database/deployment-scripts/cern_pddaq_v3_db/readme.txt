copy scripts to pduser@pddaq-gen05-daq0:/daq/database/cern_pddaq_v3_db/

Login to pddaq-gen05-daq0 as pduser and run the following:
 mkdir -p /daq/database/cern_pddaq_v3_db/var/tmp
 mkdir -p /daq/database/cern_pddaq_v3_db/data
 mkdir -p /daq/database/cern_pddaq_v3_db/logs
 mkdir -p /daq/database/cern_pddaq_v3_db/webconfigeditor
 chown -R pduser:pduser /daq/database/cern_pddaq_v3_db

 Review software release versions and qualifiers in mongod.env 
 and  webconfigeditor.env, make changes if  necessary.

 mongod.env
---------------------------------------
MONGOD_DATABASE_NAME=cern_pddaq_v3_db #do not change

MONGOD_BASE_DIR=/daq/database         #do not change

MONGOD_PORT=27027                     #do not change

MONGOD_UPS_VER=v3_4_6                 #update to match the current artdaq bundle release
MONGOD_UPS_QUAL="e14:prof"            #update to match the current artdaq bundle release
--------------------------------------

 webconfigeditor.env
---------------------------------------
MONGOD_DATABASE_NAME=cern_pddaq_v3_db
MONGOD_BASE_DIR=/daq/database         #do not change
MONGOD_PORT=27027                     #do not change

WEBEDITOR_BASE_PORT=8880              #do not change

WEBEDITOR_UPS_VER=v1_00_08a           #update to match the current artdaq bundle release
WEBEDITOR_UPS_QUAL="e14:prof:s48"     #update to match the current artdaq bundle release
---------------------------------------

