#! /bin/sh
TEST=$1
shift
SKEL=$1
shift
$SKEL $TEST -- 2>&1 | FileCheck37 -allow-empty $TEST
