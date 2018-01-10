FROM opensuse:42.3

RUN zypper addrepo --no-gpgcheck\
      http://ftp.gwdg.de/pub/opensuse/repositories/games/openSUSE_Leap_42.3/\
      opensuse-games &&\
    zypper install -y systemd-sysvinit git cmake gcc-c++ llvm-clang\
      poco-devel &&\
    zypper clean &&\
    rm -rf /tmp/* /var/tmp/*

