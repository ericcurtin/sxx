#!/bin/bash

set -eu

DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$DIR/.."

if [ "$(uname)" != "Linux" ]; then
  make -j3
  exit 0
fi

dockerfiles/docker.sh build

for doc in $(dockerfiles/docker.sh list); do
  name=$(printf "$doc" | sed "s#curtine/##" | sed "s/:/-/")
  docker rm -f "$name" || true
  d-run "$name" "$doc"
  docker exec -u "$UID" "$name" /bin/bash -c "cd $PWD &&\
                                              rm -rf bin &&\
                                              mkdir bin &&\
                                              cd bin &&\
                                              export CXX=g++ &&\
                                              cmake .. &&\
                                              make -j3 VERBOSE=1 &&\
                                              make -j3 clean"
  docker exec -u "$UID" "$name" /bin/bash -c "cd $PWD &&\
                                              rm -rf bin &&\
                                              mkdir bin &&\
                                              cd bin &&\
                                              export CXX=clang++ &&\
                                              cmake .. &&\
                                              make -j3 VERBOSE=1 &&\
                                              make -j3 clean"
done

