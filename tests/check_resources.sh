#!/bin/bash
RETURN=0
ls /dev/shm/qb* 2>/dev/null
if [ $? -eq 0 ]
then
	echo
	echo "Error: shared memory segments not closed/unlinked"
	echo
	RETURN=1
fi
ps aux | grep -v grep | grep lt-check
if [ $? -eq 0 ]
then
	echo "test program frozen"
	RETURN=1
fi

exit $RETURN
