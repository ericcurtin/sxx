FROM centos:7

RUN yum install -y epel-release &&\
    yum install -y cmake gcc-c++ git make clang poco-devel &&\
    yum clean all &&\
    rm -rf /tmp/* /var/tmp/* /var/cache/yum

