#!/bin/bash

echo "------------------Testing------------------"
echo
for file in *.in ; do 
    echo $file LFU hits:;    ../build/lfu/lfu_cache < $file
done 

for file in *.in ; do 
    echo $file Belady hits:; ../build/belady/belady_cache < $file
done