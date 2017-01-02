#! /usr/local/bin/bash

usage() {
  echo "Usage $0 y|n|m manifest"
}

if [ $# -ne 2 ]
then
  usage
  exit 1
fi

line=$(head -n1 $2 | tail -1)
file=$(head -n2 $2 | tail -1)

case $1 in
  y*)
    replace="PAR_FOR"
    ;;
  n*)
    replace="NO_PAR_FOR"
    ;;
  m*)
    replace="MAYBE_PAR_FOR"
    ;;
esac

sed -i '' $line's/for/'$replace'/' $file
