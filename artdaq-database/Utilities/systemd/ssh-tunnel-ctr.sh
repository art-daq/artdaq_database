#!/bin/bash
nohup  ssh -4 -N \
-R127.0.0.1:28047:127.0.0.1:28047 \
experimentdaq@$1.fnal.gov -i  /home/experimentdaq/.ssh/id_rsa \
 -o "StrictHostKeyChecking no" -o "UserKnownHostsFile /dev/null" \
 -o "ServerAliveInterval 60" -o "ServerAliveCountMax 3" \
 -o "GSSAPIAuthentication no" -o "GSSAPIDelegateCredentials no" \
 -o "LogLevel INFO" -E  /tmp/ssh-tunnel-$1.log & 
