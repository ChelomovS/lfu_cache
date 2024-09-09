#!/bin/bash

for file in *.in ; do 
    echo $file LFU hits:; ../build/lfu/lfu_cache < $file 
    echo $file Belady hits:; ../build/belady/belady_cache < $file
done 
