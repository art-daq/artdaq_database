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

