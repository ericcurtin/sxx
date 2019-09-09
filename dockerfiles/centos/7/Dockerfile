FROM centos:7

RUN yum install -y epel-release centos-release-scl &&\
    yum install -y cmake devtoolset-7-gcc-c++ make rpm-build boost-devel\
      perl-Pod-Checker &&\
    yum clean all &&\
    rm -rf /tmp/* /var/tmp/* /var/cache/yum

