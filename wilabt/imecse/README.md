Simple sensing example using USRP 
===================

Deployment:
There are 7 imec sensing engine deployed in wilab2 in total:
  * 5 imec sensing engines with WARP frontend are attached to the nodes: zotacC2,zotacJ2,zotacF4,dssC5,dssJ5
  * 2 imec sensing engines with scaldio frontend are attached to the zotac nodes: zotacJ3, zotacJ4

To use imec sensing engine, you need to have an emulab experiment swapped in with at least one of the nodes listed above.
Each imec sensing engine has a unic spiderID and for scaldio frontend, it also has a frontend ID.
The following table contains the configuration in wilab2, to get more information see the SenisngEngineUserManual.pdf

node    spiderID frontendID
zotacC2	139     	0
zotacJ2	140	        0
zotacJ3	129	        1
zotacF4	147	        0
zotacJ4	130	        19
dssC5	145	        0
dssJ5	146	        0


Content of this folder:
  * setupscaldio.sh and setupwarpfe.sh are the scripts to program imec sensing engine
  * environment.csh contains the environment varialbes that need to be set
  * hardware, ztex contains the first level firmware required by the setupscaldio.sh or the setupwarpfe.sh
  * firmware and scaldio_files contains the firmware needed during run time by the sensing engine
  * includes, libaries contains the sensing engine header files and the compiled library for linux 2.6 64bit platform
  * apps contains an example application using the imec sensing engine
  * SensingEngineUserManual.pdf contains the detailed configuration of the sensing engine

Usage:
    To setup the environment variables, run:
	$ source environment.sh
    To program the sensing engine with WARP front end:
    	$./setupwarpfe.sh
    To program the sensing engine with scaldio frontend
        $ ./setupscaldio.sh
    Create a binary using the imec sensing engine's C library, an example is provided in the apps folder, to compile the example run:
	$ cd apps; make 
    Hopefully the compilation is successful, and now you can run the binary to scan the 14 Wi-Fi channels in the 2.4 GHz ISM band, replace the spider id and frontend id according to your sensing engine
	$ run ./MAIN spiderid frontend



