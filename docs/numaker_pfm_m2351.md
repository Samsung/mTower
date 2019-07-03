# mTower on NuMaker-PFM-M2351

[![NuMaker-PFM-M2351](images/platforms/numaker_pfm_m2351/numaker_pfm_m2351.png)](http://www.nuvoton.com/resource-files/UM_NuMaker-PFM-M2351_EN_Rev1.00.pdf)

# Contents
1. [Introduction](#1-introduction)
2. [Regular build](#2-regular-build)
3. [Nuvoton NuMicro ICP Programming Tool](#3-nuvoton-numicro-icp-programming-tool)
4. [Flash mTower on the device and run](#4-flash-mtower-on-the-device-and-run)

## 1. Introduction
The instructions here will tell how to run mTower on the [NuMaker-PFM-M2351] board.

## 2. Regular build
Start out by following the "Get and build the solution" in the [build.md] file.
> Warning: Need to download the toolchain (exec. `make toolchain`).

## 3. Nuvoton NuMicro ICP Programming Tool
Please note that Nuvoton NuMicro ICP Programming Tool is used under Windows OS.
It can be downloaded from [here]:(https://m2351.nuvoton.com/resource/)

![Where is NuMicro ICP Programming Tool](images/platforms/numaker_pfm_m2351/numicro_icp_download.png)

Once you download the installer and start it, follow the instructions shown in installer window.

## 4. Flash mTower on the device and run
1. Start NuMicro ICP Programming Tool. A window will open where you must select M2351 target chip and click Continue button:
![Run NuMicro ICP Programming Tool](images/platforms/numaker_pfm_m2351/numicro_icp_start.png)

2. Connect the board to your PC via USB ICE connector and click Connect button in NuMicro ICP Programming Tool window.
![Connect to device](images/platforms/numaker_pfm_m2351/numicro_icp_connect.png)

3. Once board successfully connects, chip ID and info will show in corresponding fields of window. Next you must select images to flash onto board. Click APROM button and select previously compiled file mtower_s.hex (secure world image), and click APROM_NS button and select mtower_ns.hex file (normal world image). Check the APROM and APROM_NS checkboxes in the bottom of window, in Programming group. Click the Start button to start flashing process.
![Select image files and program start](images/platforms/numaker_pfm_m2351/numicro_icp_open_prg.png)

4. We use 2 consoles for debugging: one will connect via USB ICE (for Secure input/output), and one will connect via UART (for Nonsecure input/output).
![NuMaker-PFM-M2351 pins](images/platforms/numaker_pfm_m2351/numicro_pin.png)

5. Open 2 console/terminal windows (e.g., Putty) using 115200 speed setting with 8 bits with no parity check. Press Reset button on M2351 board, and you should see output in terminal windows, like one shown below. Note that test tasks are running in infinite loop, so the output shall not stop.
![Start mTower](images/platforms/numaker_pfm_m2351/putty_output.png)

[build.md]: build.md
[NuMaker-PFM-M2351]: http://www.nuvoton.com/resource-files/UM_NuMaker-PFM-M2351_EN_Rev1.00.pdf
