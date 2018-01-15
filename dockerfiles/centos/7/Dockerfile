FROM centos:7

RUN yum install -y epel-release centos-release-scl &&\
    yum install -y cmake devtoolset-7-gcc-c++ poco-devel make &&\
    yum clean all &&\
    rm -rf /tmp/* /var/tmp/* /var/cache/yum

