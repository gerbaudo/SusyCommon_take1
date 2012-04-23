#!/bin/bash

iteration="i9"
#iteration="test4"

# SUSY D3PD grid datasets
datasets=(
        "group10.phys-susy.data11_7TeV.periodB.physics_Egamma.PhysCont.NTUP_SUSYSKIM.pro10_v01_p832/"
        "group10.phys-susy.data11_7TeV.periodB.physics_Muons.PhysCont.NTUP_SUSYSKIM.pro10_v01_p832/"
        "group10.phys-susy.data11_7TeV.periodD.physics_Egamma.PhysCont.NTUP_SUSYSKIM.pro10_v01_p832/"
        "group10.phys-susy.data11_7TeV.periodD.physics_Muons.PhysCont.NTUP_SUSYSKIM.pro10_v01_p832/"
        "group10.phys-susy.data11_7TeV.periodE.physics_Egamma.PhysCont.NTUP_SUSYSKIM.pro10_v01_p832/"
        "group10.phys-susy.data11_7TeV.periodE.physics_Muons.PhysCont.NTUP_SUSYSKIM.pro10_v01_p832/"
        "group10.phys-susy.data11_7TeV.periodF.physics_Egamma.PhysCont.NTUP_SUSYSKIM.pro10_v01_p832/"
        "group10.phys-susy.data11_7TeV.periodF.physics_Muons.PhysCont.NTUP_SUSYSKIM.pro10_v01_p832/"
        "group10.phys-susy.data11_7TeV.periodG.physics_Egamma.PhysCont.NTUP_SUSYSKIM.pro10_v01_p832/"
        "group10.phys-susy.data11_7TeV.periodG.physics_Muons.PhysCont.NTUP_SUSYSKIM.pro10_v01_p832/"
        "group10.phys-susy.data11_7TeV.periodH.physics_Egamma.PhysCont.NTUP_SUSYSKIM.pro10_v01_p832/"
        "group10.phys-susy.data11_7TeV.periodH.physics_Muons.PhysCont.NTUP_SUSYSKIM.pro10_v01_p832/"
        "group10.phys-susy.data11_7TeV.periodI.physics_Egamma.PhysCont.NTUP_SUSYSKIM.pro10_v01_p832/"
        "group10.phys-susy.data11_7TeV.periodI.physics_Muons.PhysCont.NTUP_SUSYSKIM.pro10_v01_p832/"
        "group10.phys-susy.data11_7TeV.periodJ.physics_Egamma.PhysCont.NTUP_SUSYSKIM.pro10_v01_p832/"
        "group10.phys-susy.data11_7TeV.periodJ.physics_Muons.PhysCont.NTUP_SUSYSKIM.pro10_v01_p832/"
        "group10.phys-susy.data11_7TeV.periodK.physics_Egamma.PhysCont.NTUP_SUSYSKIM.pro10_v01_p832/"
        "group10.phys-susy.data11_7TeV.periodK.physics_Muons.PhysCont.NTUP_SUSYSKIM.pro10_v01_p832/"
)


# Setup Panda before running this script
#source ${ATLAS_LOCAL_ROOT_BASE}/packageSetups/atlasLocalPandaClientSetup.sh

# Loop over datasets
for inDS in ${datasets[@]}; do

	# get sample name from input dataset name
        sample=${inDS#*phys-susy.}
        sample=${sample%.PhysCont.*}
        sample=${sample/physics_/}

	# final output ds name
	outDS="user.Steve.$iteration.$sample.SusyNt/"

        command="./gridScript.sh %IN -s $sample"

	echo 
        echo "__________________________________________________________________________________________________________"
	echo "INPUT   $inDS"
	echo "OUTPUT  $outDS"
        echo "sample  $sample"
	
	# prun command
	prun --exec "$command" --useRootCore --tmpDir /tmp \
             --excludedSite=MANC,QMUL,MWT2,ECDF,WEIZMANN,RHUL,OX,SARA,SHEF,PIC \
             --nGBPerJob=MAX \
             --extFile '*.so,*.root' --match "*root*" --outputs "susyNt.root" \
             --inTarBall=area.tar \
             --athenaTag=17.0.5.5 \
	     --inDS  $inDS \
	     --outDS $outDS

done
