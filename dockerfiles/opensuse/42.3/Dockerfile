FROM opensuse:42.3

RUN zypper addrepo --no-gpgcheck\
      http://ftp.gwdg.de/pub/opensuse/repositories/games/openSUSE_Leap_42.3/\
      opensuse-games &&\
    zypper update -y &&\
    zypper install -y systemd-sysvinit cmake gcc-c++ llvm-clang poco-devel perl\
      rpm-build boost-devel clang &&\
    zypper clean &&\
    rm -rf /tmp/* /var/tmp/*

