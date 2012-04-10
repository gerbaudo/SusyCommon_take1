#!/bin/bash

#iteration="i3"
iteration="test3"

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
        #xsec=${info[2]}
        #sumw=${info[3]}

	outDS="user.Steve.$iteration.$sample.SusyNt/"

	echo 
	echo "__________________________________________________________________________________________________________"
        echo "sample  $sample"
	echo "inDS    $inDS"
	echo "outDS   $outDS"
        #echo "xsec    $xsec"
        #echo "sumw    $sumw"

	
	# prun command
	prun --exec "./gridScript.sh %IN" --tmpDir /tmp --inTarBall=area.tar --useRootCore \
		--match "*root*" --outputs "susyNt.root" --athenaTag=17.0.3 --extFile '*.so,*.root' \
                --nFiles=1 \
		--inDS  $inDS \
		--outDS $outDS

done
