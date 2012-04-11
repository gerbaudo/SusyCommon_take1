#!/bin/bash

#iteration="i3"
iteration="test3"

# SUSY D3PD grid datasets
datasets=(
        #"group10.phys-susy.data11_7TeV.periodB.physics_Egamma.PhysCont.NTUP_SUSYSKIM.pro10_v01_p832/"
        #"group10.phys-susy.data11_7TeV.periodD.physics_Egamma.PhysCont.NTUP_SUSYSKIM.pro10_v01_p832/"
        #"group10.phys-susy.data11_7TeV.periodE.physics_Egamma.PhysCont.NTUP_SUSYSKIM.pro10_v01_p832/"
        #"group10.phys-susy.data11_7TeV.periodF.physics_Egamma.PhysCont.NTUP_SUSYSKIM.pro10_v01_p832/"
        "group10.phys-susy.data11_7TeV.periodG.physics_Egamma.PhysCont.NTUP_SUSYSKIM.pro10_v01_p832/"
        #"group10.phys-susy.data11_7TeV.periodH.physics_Egamma.PhysCont.NTUP_SUSYSKIM.pro10_v01_p832/"
        #"group10.phys-susy.data11_7TeV.periodI.physics_Egamma.PhysCont.NTUP_SUSYSKIM.pro10_v01_p832/"
        #"group10.phys-susy.data11_7TeV.periodJ.physics_Egamma.PhysCont.NTUP_SUSYSKIM.pro10_v01_p832/"
        #"group10.phys-susy.data11_7TeV.periodK.physics_Egamma.PhysCont.NTUP_SUSYSKIM.pro10_v01_p832/"
        #"group10.phys-susy.data11_7TeV.periodB.physics_Muons.PhysCont.NTUP_SUSYSKIM.pro10_v01_p832/"
        #"group10.phys-susy.data11_7TeV.periodD.physics_Muons.PhysCont.NTUP_SUSYSKIM.pro10_v01_p832/"
        #"group10.phys-susy.data11_7TeV.periodE.physics_Muons.PhysCont.NTUP_SUSYSKIM.pro10_v01_p832/"
        #"group10.phys-susy.data11_7TeV.periodF.physics_Muons.PhysCont.NTUP_SUSYSKIM.pro10_v01_p832/"
        #"group10.phys-susy.data11_7TeV.periodG.physics_Muons.PhysCont.NTUP_SUSYSKIM.pro10_v01_p832/"
        #"group10.phys-susy.data11_7TeV.periodH.physics_Muons.PhysCont.NTUP_SUSYSKIM.pro10_v01_p832/"
        #"group10.phys-susy.data11_7TeV.periodI.physics_Muons.PhysCont.NTUP_SUSYSKIM.pro10_v01_p832/"
        #"group10.phys-susy.data11_7TeV.periodJ.physics_Muons.PhysCont.NTUP_SUSYSKIM.pro10_v01_p832/"
        #"group10.phys-susy.data11_7TeV.periodK.physics_Muons.PhysCont.NTUP_SUSYSKIM.pro10_v01_p832/"
)


# Setup Panda before running this script
#source ${ATLAS_LOCAL_ROOT_BASE}/packageSetups/atlasLocalPandaClientSetup.sh

# Loop over datasets
for inDS in ${datasets[@]}; do

	# strip prefix
	#shortname=${inDS#*.*.*.}
        shortname=${inDS#*phys-susy.}
        shortname=${shortname%.PhysCont.*}
        shortname=${shortname/physics_/}

	# final output ds name
	outDS="user.Steve.$iteration.$shortname.SusyNt/"

        command="./gridScript.sh %IN -s $shortname"

	echo 
	echo "----------------------------------------------------------------------------------------------------------"
	echo "INPUT   $inDS"
	echo "OUTPUT  $outDS"
        echo "short   $shortname"
        echo "command $command"
	
	# prun command
	prun --exec "$command" --useRootCore --tmpDir /tmp \
             --extFile '*.so,*.root' --match "*root*" --outputs "susyNt.root" \
             --inTarBall=area.tar \
             --athenaTag=17.0.5.5 \
             --nFiles=1 \
	     --inDS  $inDS \
	     --outDS $outDS

done
