FROM opensuse:42.3

RUN zypper up -y &&\
    zypper install -y systemd-sysvinit\
                      git\
                      cmake\
                      gcc-c++\
                      llvm-clang\
                      Shellcheck &&\
    zypper clean &&\
    rm -rf /tmp/* /var/tmp/*

