#!/bin/bash
X=3
if (( $X == 1+1+1+1 )) || (( $RANDOM % 2 == 1 )) ;
then
  echo "YES"
fi
