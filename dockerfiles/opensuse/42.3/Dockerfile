FROM opensuse:42.3

RUN zypper update -y &&\
    zypper install -y systemd-sysvinit cmake gcc-c++ llvm-clang perl rpm-build\
      boost-devel clang &&\
    zypper clean &&\
    rm -rf /tmp/* /var/tmp/*

