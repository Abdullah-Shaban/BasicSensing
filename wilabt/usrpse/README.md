Simple sensing example using USRP 
===================

iMinds
-----
usrpse.ns: The ns script that describes the topology of 1 USRP and 1 host server on the testbed
usrpse_nonesweeping: The binary that operates USRP sensing engine in continuous capturing mode, prints output in stdout
usrpse_sweeping: The binary that operates USRP in sweeping mode, prints output in stdout
usrpse_wrap.rb: The ruby wrapper, that calls one of the above binaries, depending on the options, when seamless sample collection sensing is required (eg, to measure channel occupation ratio), the usrpse_nonesweeping is called, when broad band coverage is prefered, the usrpse_sweeping is called. 

Requirements:

  * [Install Open VPN client](https://openvpn.net/index.php/open-source/downloads.html)
  * [Request an w-iLab.t testbed account](http://ilabt.iminds.be/gettingstarted)
  * [Basic knowledge of the w-iLab.t Zwijnaarde testbed](http://ilabt.iminds.be/node/93)
  * [Basic knowledge of using USRP in w-iLab.t Zwijnaarde testbed](http://doc.ilabt.iminds.be/ilabt-documentation/wilabfacility.html#using-the-usrp-devices)

To run the cr application:

    $ reserve 1 USRP's and 1 server (can be either server5P or server1P) on the testbed reservation page
    $ define an emulab experiment using the usrpse.ns file (if it does not exist), adapt the USRP and server according to the reservation and swap in your experiment
    $ log into the reserved server and verify the connection between the server and each USRP via the 'uhd_usrp_probe' command
    $ execute the following command:
        sudo sysctl -w net.core.wmem_max=1048576
        sudo sysctl -w net.core.rmem_max=50000000
    $ Run ./usrpse_wrap.rb --help, the options are listed below (the option without default value is obligated):
        -g, --gain GAIN                  RFGain, default 30 dB
        -s, --spb SPB                    SamplePerBuffer Default 16777216 for channel occupation mode, else 1048576 
        -n, --fftsize N                  FFTsize, default 512
        -a, --args Addr                  The USRP IP address
        -k, --first_channel FIRSTCHANNEL FirstChannel, required for channel occupation mode, else ignored
        -m, --detector_mode MODE         Detector, allowed values: COR, AVG, MAX, MIN. Default: AVG
        -d, --std STANDARD               Wireless Standard, allowed values: WLAN_G, ZIGBEE, BLUETOOTH
        -o, --output OUTPUT              A varialbe to indicate the way to collect data, allowed values: STDOUT, OML
        -e, --expid expid                A variable to store experiment ID of OMF (ignored when @output_mode != 'OML')
    $ examples (adapt the ip address to one of the USRP's you reserved)
      To measure all the zigbee channels in averaging mode, and store it in the database test_crew on the wilabt OML server, run:
        ./usrpse_wrap.rb --std ZIGBEE -o OML -e test_crew --args 192.168.30.2  
      To measure the channel occupation ratio of the 13th channel in IEEE802.11g standard, run:
        ./usrpse_wrap.rb --std WLAN_G --args 192.168.30.2 -m COR --first_channel 13
      To measure the 80 Bluetooth channels in the 2.4 GHz using the maxhold filtering, run:
        ./usrpse_wrap.rb --std BLUETOOTH --args 192.168.30.2 -m MAX
 
Additional remarks: 
    * For the maxhold and averaging detector mode, all channels for the given se_mode are measured:
        in case of Zigbee this is 16 channels from 11 to 26, WLAN_G this is 1 to 13, Bluetooth this is 1 to 79
        The output unit is dBm
    * For the COR mode, the output unit is %, stands for how much time the channel is occupied
        in this mode USRP is not sweeping, hence only channels within the maximum sample rate (25 MHz range) can be covered and it is obligated to specify the chanel to start measurements using the --first_channel option. 
        in case of Zigbee, this means 4 channels, starting from the first_channel you specified
        in case of WLAN_G, this is only 1 channel, the first channel you specified
        in case of BLUETOOTH, this is 20 channel, starting from the first channel you specified

