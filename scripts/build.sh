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
  d_exe "root" "$name"\
    "groupadd -g $gid $group && useradd -M -s /bin/bash -g $gid -u $UID $user"
  d_exe "$UID" "$name" "cd $PWD && $cmd"
  docker rm -f "$name" || true
}

d_compile() {
  local cc=$1
  local cxx=$2
  local pre=$3

  if [ -z "$pre" ]; then
    pre="true"
  fi

  local cmd="$pre && export CC=$cc && export CXX=$cxx && rm -rf build &&\
    mkdir build && cd build && cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr .. &&\
    make VERBOSE=1 -j3 all doc && make package && ctest -j3"

  if [[ "$doc" != *"centos"* ]] && [[ "$doc" != *"opensuse"* ]]; then
    cmd="$cmd && cd - && rm -rf build && mkdir build && cd build &&\
      cmake .. -DCOVERAGE=ON && make VERBOSE=1 -j3 && ctest -j3 &&\
      cd - && rm -rf build && mkdir build && cd build && cmake .. -DASAN=ON &&\
      make VERBOSE=1 -j3 && ctest -j3 &&\
      cd - && rm -rf build && mkdir build && cd build && cmake .. -DUBSAN=ON &&\
      make VERBOSE=1 -j3 && ctest -j3"
  fi

  d_run "$name" "$doc" "$cmd"

  mv build/*.deb pkgs/ 2>/dev/null || true
  mv build/*.rpm pkgs/ 2>/dev/null || true
}

set -e

DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$DIR/.."

rm -rf pkgs
mkdir pkgs

for doc in $(dockerfiles/docker.sh list); do
  name=$(printf "$doc" | sed "s#curtine/##" | sed "s/:/-/")

  if [[ "$doc" == *"centos"* ]]; then
    d_compile "gcc" "g++" ". /opt/rh/devtoolset-7/enable"
    continue
  else
    d_compile "clang" "clang++"
  fi

  d_compile "gcc" "g++"
done

