Copy setup_database.sh to pduser@pddaq-gen05-daq0:/daq/artdaq/
Update the two lines in setup_database.sh so they are consistent with the current
release of the artdaq software. Verify that the same values are used in the 
pduser@pddaq-gen05-daq0:/daq/database/cern_pddaq_v*_db/*.env files.

Files:
/daq/database/cern_pddaq_v3_db/mongod.env
/daq/database/cern_pddaq_v3_db/webconfigeditor.env

 setup artdaq_database v1_04_26 -q e14:prof:s48
 export ARTDAQ_DATABASE_URI="mongodb://127.0.0.1:27027/cern_pddaq_db"

---------------------------------------------------------------------------------

Follow instructions in readme files below.

artdaq-database/readme.txt
cern_pddaq_v3_db/readme.txt
systemd/readme.txt
cern_pddaq_v2_db/readme.txt

To start artdaq config database services login to pddaq-gen05-daq0 as root 
and run the following:

a) for database v2 (old version):
    systemctl start  mongodbserver@cern_pddaq_v2_db.service
    systemctl start  webconfigeditor@cern_pddaq_v3_db.service
b) for database v3 (new version):
    systemctl start  mongodbserver@cern_pddaq_v2_db.service
    systemctl start  webconfigeditor@cern_pddaq_v2_db.service


To check running state of  artdaq config database services login to pddaq-gen05-daq0 as root
and run the following:

a) for database v2 (old version):
    systemctl status  mongodbserver@cern_pddaq_v2_db.service
    systemctl status  webconfigeditor@cern_pddaq_v3_db.service
b) for database v3 (new version):
    systemctl status  mongodbserver@cern_pddaq_v2_db.service
    systemctl status  webconfigeditor@cern_pddaq_v2_db.service

