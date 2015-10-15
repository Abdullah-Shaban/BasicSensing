Simple sensing example using imec sensing engine with the WARP radio board
===================

Requirements:

  * [Install Open VPN client](https://openvpn.net/index.php/open-source/downloads.html)
  * [Request an w-iLab.t testbed account](http://ilabt.iminds.be/gettingstarted)
  * [Basic knowledge of the w-iLab.t Zwijnaarde testbed](http://ilabt.iminds.be/node/93)

Content of this folder:
  * example.ns is a simple ns script to define an experiment to use the sensing engine in Emulab
  * setupscaldio.sh and setupwarpfe.sh are the scripts to program imec sensing engine
  * environment.sh contains the environment varialbes that need to be set
  * hardware, ztex contains the first level firmware required by the setupscaldio.sh or the setupwarpfe.sh
  * firmware contains the firmware needed during run time by the sensing engine
  * apps contains an example application using the imec sensing engine
  * SensingEngineUserManual.pdf contains the detailed configuration of the sensing engine
  
  
Deployment:
  * 5 imec sensing engines with WARP frontend are attached to the following nodes respectively: zotacC2,zotacJ2,zotacF4,dssC5,dssJ5
  
Usage:
  * To use imec sensing engine, you need to have an emulab experiment swapped in with at least one of the nodes listed above. 
  	* For a dss node, you need to use the image 'ubu14_imecse_dss'
  	* For a zotac node, you need to use the iamge 'ubu14_imecse_zotac'
	* The ns script used to define emulab experiment is 

  * To setup the environment variables, run:
		$ source environment.sh
  * To program the sensing engine with WARP front end:
    	$ ./setupwarpfe.sh 1
  * Create a binary using the imec sensing engine's C library, an example is provided in the apps folder, to compile the example run:
		$ cd apps; make 
  * Hopefully the compilation is successful, and now you can run the binary file 'MAIN' in the Output folder as shown below, it will scan the 14 Wi-Fi channels in the 2.4 GHz ISM band
		$ Output/MAIN 



