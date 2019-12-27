#!/bin/bash

/usr/bin/systemctl stop mongodbarbiter@experiment_v4x_db.service

/usr/bin/systemctl stop mongodbserver@experiment_v4x_db.service

/usr/bin/systemctl disable mongodbarbiter@experiment_v4x_db.service

/usr/bin/systemctl disable mongodbserver@experiment_v4x_db.service

rm -f /etc/systemd/system/mongodbarbiter@.service /etc/systemd/system/mongodbserver@.service
cp mongodbarbiter@.service mongodbserver@.service /etc/systemd/system/

/usr/bin/systemctl daemon-reload

/usr/bin/systemctl enable mongodbarbiter@experiment_v4x_db.service

/usr/bin/systemctl enable mongodbserver@experiment_v4x_db.service

/usr/bin/systemctl start  mongodbarbiter@experiment_v4x_db.service
 
/usr/bin/systemctl start  mongodbserver@experiment_v4x_db.service

/usr/bin/systemctl status mongodbarbiter@experiment_v4x_db.service

/usr/bin/systemctl status mongodbserver@experiment_v4x_db.service
