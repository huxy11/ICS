#!/bin/bash

function init() {
  if [ $1 ] ; then
    sed -i -e "/^export $1=.*/d" ~/.bashrc
    echo "export $1=`readlink -e $1`" >> ~/.bashrc
  fi
}

read -r -p "Are you sure to initialize evns ? [y/n] " input
case $input in
  [yY])
    init NEMU_HOME
    init AM_HOME
    init NAVY_HOME

    echo "Initialization finishes!"
    echo "By default this script will add environment variables into ~/.bashrc."
    echo "After that, please run 'source ~/.bashrc' to let these variables take effect."
    echo "If you use shell other than bash, please add these environment variables manually."
    ;;

  [nN])
    ;;

  *)
    echo "Invalid input..."
    ;;
esac
