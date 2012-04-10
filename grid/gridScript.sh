#!/bin/bash

echo "**** GRID SCRIPT BEGIN ****"

# Convert the comma separated list of files into an array
list=(`echo $1 | tr ',' ' '`)
nFiles=${#list[@]}
echo $nFiles input files

# Write the list of files to the fileList
if [ -f gridFileList.txt ]; then
    rm gridFileList.txt
fi

for file in ${list[@]}; do
    echo $file >> gridFileList.txt
done

# MC variables
#if [[ $# -ge 4 ]]; then
    #echo "sample $2 xsec $3 sumw $4"
    #mcOpts="-s $2 -xsec $3 -sumw $4"
#fi

# Now, run the executable
NtMaker -f gridFileList.txt -d 1

exitcode=$?
if [[ $exitcode != 0 ]]; then
    echo "**** GRID SCRIPT FAIL ****"
    exit 1
fi

echo "**** GRID SCRIPT END ****"
