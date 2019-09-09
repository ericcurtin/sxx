FROM centos:7

RUN yum -y upgrade && yum -y install openssh-server

