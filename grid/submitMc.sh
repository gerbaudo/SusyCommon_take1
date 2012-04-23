#!/bin/bash

iteration="i9"
#iteration="test4"

# get the samples of interest
if [[ $# = 0 ]]; then
        echo "submit all samples"
        pattern="mc11"
else
        pattern=$1
fi
matches=(`cat mcSamples.txt | grep $pattern | tr '\t' ','`)
echo "${#matches[@]} matches"

# set it up manually I guess
#source ${ATLAS_LOCAL_ROOT_BASE}/packageSetups/atlasLocalPandaClientSetup.sh

# Loop over samples
for line in ${matches[@]}; do
        info=(`echo $line | tr ',' ' '`)
        sample=${info[0]}
        inDS=${info[1]}
        sumw=${info[2]}

	outDS="user.Steve.$iteration.$sample.SusyNt/"

        command="./gridScript.sh %IN -s $sample -w $sumw"

	echo 
	echo "__________________________________________________________________________________________________________"
        echo "INPUT   $inDS"
        echo "OUTPUT  $outDS"
        echo "sample  $sample"
        echo "sumw    $sumw"

	
	# prun command
	prun --exec "$command" --tmpDir /tmp --inTarBall=area.tar --useRootCore \
                --excludedSite=MANC,QMUL,MWT2,ECDF,WEIZMANN,RHUL,OX,SARA,SHEF,PIC \
                --nGBPerJob=MAX \
		--match "*root*" --outputs "susyNt.root" \
                --extFile '*.so,*.root' \
                --athenaTag=17.0.5.5 \
		--inDS  $inDS \
		--outDS $outDS

done
