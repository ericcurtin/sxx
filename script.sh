#!/bin/bash

docker build -t test .

LIST_DOCKERS="docker ps -qa"

$LIST_DOCKERS | xargs docker rm -f

KEYGEN="ssh-keygen -t rsa -f /etc/ssh/ssh_host_rsa_key -N ''"
SET_PASS="echo 'password' | passwd --stdin root; /usr/sbin/sshd -D"

docker run -itd test /bin/bash -c "$KEYGEN; $SET_PASS"
docker run -itd test /bin/bash -c "$KEYGEN; $SET_PASS"

$LIST_DOCKERS | xargs docker inspect --format '{{ .NetworkSettings.IPAddress }}'

