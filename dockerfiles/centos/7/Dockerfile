FROM centos:7

RUN yum update -y &&\
    yum install -y cmake gcc-c++ git make clang &&\
    yum clean all &&\
    rm -rf /tmp/* /var/tmp/* /var/cache/yum

