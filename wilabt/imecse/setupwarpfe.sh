#!/bin/csh
#

if ($#argv != 1) then
echo "You must specify how many Spider devices to configure "
else
echo "Configuring $1 Spider device(s)..."
@ i = 0
while ($i < $1)
    ./ztex/FWLoader -c -v 0x4b4 0x8613 -f -uu ./hardware/main_firmware.ihx
    @ i += 1
end
@ i = 0
echo "Configuring firmware for WARP frontend..."
while ($i < $1)
    ./ztex/FWLoader -d $i -f -uf ./hardware/spider_fpga_lx45.v2_warp
#    ./ztex/FWLoader -d $i -f -uf ./hardware/spider_fpga_lx45.v2_2b
    @ i += 1
end
endif

