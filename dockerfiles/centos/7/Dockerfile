FROM centos:7

RUN yum install -y epel-release centos-release-scl &&\
    yum install -y cmake gcc devtoolset-7-gcc-c++ git make poco-devel &&\
    yum clean all &&\
    rm -rf /tmp/* /var/tmp/* /var/cache/yum

