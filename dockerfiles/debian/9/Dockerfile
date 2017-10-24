FROM debian:9

RUN apt update -y &&\
    apt upgrade -y &&\
    apt install -y init git cmake g++ clang shellcheck &&\
    rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*

