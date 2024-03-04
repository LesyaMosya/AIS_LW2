#!/bin/bash

if [! -n "$1" ]
then
  echo "First parameter must be user Name"
else
mkdir "build"
cd build/
cmake .. -G"Ninja"
ninja

mkdir /home/$1/ais_lw2_server/

cp AIS_LW2_Server /home/$1/ais_lw2_server/AIS_LW2_Server

settingsDir=/home/$1/ais_lw2_server/settings.cfg

if [ -d $settingsDir ]
then
  rm $settingsDir
fi
touch $settingsDir
echo "Created $settingsDir"
sudo cat << EOF >> settingsDir
save_path = "/home/$1/test/";
threads = 3;
port = 5000;
file_max_size = 1024;
EOF

serviceDir=/etc/systemd/system/ais_lw2_server_daemon.service
if [ -d $serviceDir ]
then
  rm $serviceDir
fi
sudo touch $serviceDir
echo "Created $serviceDir"

sudo cat << EOF >> $serviceDir
[Unit]
Description=ais_lw2_server_daemon

[Service]
WorkingDirectory=/home/$1/ais_lw2_server/
ExecStart=/home/$1/ais_lw2_server/AIS_LW2_Server
Restart=always
RestartSec=10 # 
SyslogIdentifier=ais_lw2_server
User=root

[Install]
WantedBy=multi-user.target
EOF

sudo systemctl enable ais_lw2_server_daemon.service
sudo systemctl start ais_lw2_server_daemon.service
sudo systemctl status ais_lw2_server_daemon.service
fi
