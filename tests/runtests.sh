#!/usr/bin/env bash

echo -e "\e[0;33m--------------------------------"
echo -e "|      Running unit tests      |"
echo -e "--------------------------------\e[0;0m"

for i in tests/*_tests
do
    if test -f $i 
    then
        if $VALGRIND ./$i 2>> tests/tests.log 
        then
            echo $i PASS
        else
            echo "ERROR: in test $i: Dumping tests/tests.log..."
            echo "------"
            tail tests/tests.log
            exit 1
        fi
    fi
done

echo -e "\e[0;33m--------------------------------"
echo -e "|    Finished running tests    |"
echo -e "--------------------------------\e[0;0m"
