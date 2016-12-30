#! /usr/local/bin/bash

strategies=(identity reverse)

usage() {
  echo "Usage: $0 [strategies] output_dir file"
  echo "available strategies:"

  for strat in ${strategies[@]}
  do
    echo -e "  $strat"
  done
}

argc=$#
if [ $argc -lt 3 ] 
then
  usage
  exit 1
fi

nstrats=$(expr $argc - 2)
declare -a strats
for i in $(seq 1 $nstrats)
do
  strats[$i]=$1
  shift
done

output_dir=$1
shift

filename=$1
shift

if ! [ -d $output_dir ]
then
  echo "Directory $output_dir does not exist"
  exit 2
fi

if ! [ -e $filename ]
then
  echo "Input file $filename does not exist"
  exit 3
fi

for strat in ${strats[@]}
do
  s=$strat
  s+="_"
  base=$(basename $filename)
  cp $filename "$output_dir/$s$base"
  skel -reorder -$strat "$output_dir/$s$base" --
done
