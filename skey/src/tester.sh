#!/bin/sh

out=`echo johndoe | ./key 88 ka9q2 | tail -1`
if test "$out" != 'NOLL AMRA FEE HOST BELA DEFT' ; then
        echo 'Test fail: johndoe'
        exit 1
else
    	echo 'Test Successful: johndoe'
fi
