FROM centos:7

RUN yum install -y epel-release &&\
    yum update -y &&\
    yum install -y cmake gcc-c++ git make clang ShellCheck boost-regex\
        boost-devel &&\
    yum clean all &&\
    rm -rf /tmp/* /var/tmp/* /var/cache/yum

