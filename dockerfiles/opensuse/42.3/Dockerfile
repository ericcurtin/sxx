FROM opensuse:42.3

RUN zypper up -y &&\
    zypper install -y systemd-sysvinit git cmake gcc-c++ clang &&\
    zypper clean &&\
    rm -rf /tmp/* /var/tmp/*

