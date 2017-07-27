copy scripts to root@pddaq-gen05-daq0:/etc/systemd/system/

Login to pddaq-gen05-daq0 and confirm that copied scripts are executable.
ls -al /etc/systemd/system/*@cern_pddaq_v*.service

To enable artdaq config database services login to pddaq-gen05-daq0 as root
and run the following:

systemctl daemon-reload

a) for database v2 (old version):
    systemctl enable  mongodbserver@cern_pddaq_v2_db.service
    systemctl enable  webconfigeditor@cern_pddaq_v3_db.service
b) for database v3 (new version):
    systemctl enable  mongodbserver@cern_pddaq_v2_db.service
    systemctl enable  webconfigeditor@cern_pddaq_v2_db.service

