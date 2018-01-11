FROM centos:6

RUN yum install -y epel-release devtoolset-7 &&\
    yum install -y cmake poco-devel &&\
    yum clean all &&\
    rm -rf /tmp/* /var/tmp/* /var/cache/yum

