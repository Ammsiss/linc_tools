#!/bin/bash

VALGRIND='valgrind
    --quiet
    --vgdb=no
    --leak-check=full
    --errors-for-leak-kinds=definite,indirect,possible'

bear --append -- make --quiet
$VALGRIND ./test_all -s
