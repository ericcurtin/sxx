FROM centos:6

RUN yum install -y epel-release centos-release-scl &&\
    yum install -y upstart cmake devtoolset-7-gcc-c++ make boost-devel\
      rpm-build &&\
    yum clean all &&\
    rm -rf /tmp/* /var/tmp/* /var/cache/yum

