#! /usr/local/bin/bash

usage() {
  echo "Usage: $0 manifest name strats..."
}

if [ $# -le 2 ]
then
  usage
  exit 1
fi

manifest=$1
shift

exe=$1
shift

strats="$@"

good=$(./"$exe" $ARGS)

for strat in ${strats[@]}
do
  out=$(./"$strat"_$exe $ARGS)
  if [ "$out" != "$good" ]
  then
    ./annotate.sh n $manifest
    exit 0
  fi
done

./annotate.sh m $manifest
