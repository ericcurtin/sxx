#!/bin/bash

d_exe() {
  local user=$1
  local name=$2
  local cmd=$3

  docker exec -u "$user" "$name" /bin/bash -c "$cmd"
}

d_run() {
  local name=$1
  local img=$2
  local cmd=$3

  local user=$(id -un)
  local gid=$(id -g)
  local group=$(id -gn)

  docker rm -f "$name" || true
  docker run --privileged -d -v /tmp:/tmp -v "/home/$user:/home/$user" -h\
         "$name" --name "$name" "$img" init
  d_exe "root" "$name" "groupadd -g $gid $group && useradd -M -s /bin/bash -g\
                        $gid -u $UID $user"
  d_exe "$UID" "$name" "cd $PWD && $cmd"
  docker rm -f "$name" || true
}

d_compile() {
  local cc=$1
  local cxx=$2

  d_run "$name" "$doc" "export CC=$cc &&\
                        export CXX=$cxx &&\
                        rm -rf bin &&\
                        mkdir bin &&\
                        cd bin &&\
                        cmake .. &&\
                        make -j3 all"
  docker rm -f "$name" || true
}

set -eu

DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$DIR/.."

for doc in $(dockerfiles/docker.sh list); do
  name=$(printf "$doc" | sed "s#curtine/##" | sed "s/:/-/")

  if [[ "$doc" == *"centos"* ]]; then
    d_run "$name" "$doc"\
      "export CC=gcc && export CXX=g++ && . /opt/rh/devtoolset-7/enable &&\
       rm -rf bin && mkdir bin && cd bin && cmake .. && make -j3 all"
    continue
  elif [[ "$doc" == *"debian"* ]]; then
    d_compile "clang" "clang++"
  fi

  d_compile "gcc" "g++"
done

