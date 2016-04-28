#!/bin/bash -x

AVR_CONTAINER_VERSION="latest"



function init() {

  DEPENDENCY_LABEL=`env | grep GO_DEPENDENCY_LABEL_ | cut -d= -f2 | sed 's/\s//g' | tr -d '\n'`


  if [ -z ${DEPENDENCY_LABEL} ]; then
    AVR_CONTAINER_VERSION="latest"
  else
    AVR_CONTAINER_VERSION="v${DEPENDENCY_LABEL}"
  fi


}
function fix_git_user() {
  git config --system user.name Docker && git config --system user.email docker@localhost
  ERROR_CODE=$?
  if [ ${ERROR_CODE} -ne 0 ]; then

      echo "Setting Git user failed"
      exit ${ERROR_CODE}
  fi
}
function compile() {
    # this files helps bundling the build commands
    # fix_git_user
    mkdir -p build && cd build && cmake .. && make
    ERROR_CODE=$?
    if [ ${ERROR_CODE} -ne 0 ]; then

        echo "Compilation failed"
        exit ${ERROR_CODE}
    fi
}

function build_software() {
  mkdir -p build;
  docker run --rm --user `id -u`:`id -g` -v $PWD:/build --entrypoint /build/build.sh ubirch/avr-build:${AVR_CONTAINER_VERSION}
  if [ $? -ne 0 ]; then
      echo "Docker build failed"
      exit 1
  fi
}

case "$1" in
    build)
        init
        build_software
        ;;
    *)
        compile
esac

exit 0
