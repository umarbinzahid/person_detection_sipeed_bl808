
# Person Detection on M1s Dock using Sipeed SDK
This example utilizes the BL808 C906 (D0) core to implement the TensorFlow person detection example using Sipeed SDK. There are two projects: one is vectorized, and the other is non-vectorized. In the vectorized example, only the integer convolution function (tensorflow/lite/kernels/internal/refrence/integer_ops/conv.h) is vectorized, offering approximately 4 to 5 times the performance boost.

## Getting Started

### Prerequisites
1. OS: Linux
2. Make
3. Git
4. Python
5. Visual Studio Code (Extensions: C/C++ Extension Pack by Microsoft, Python by Microsoft)
6. Install the Sipeed Firmware in M1s Dock. Follow the Guidelines on [Sipeed Wiki](https://wiki.sipeed.com/hardware/en/maix/m1s/other/start.html#Burn-firmware).

### Setup
1. Create a directory for sdk.

    ```bash
    cd ~
    ```
    ```bash
    mkdir -p ~/apps/sipeed
    ```
    ```bash
    cd ~/apps/sipeed
    ```

2. Clone the Sipeed SDK in newly created "apps" folder.
    ```bash
    git clone https://github.com/sipeed/M1s_BL808_SDK.git
    ```
3. Download the t-head gcc toolchain (Xuantie-900-gcc-elf-newlib-x86_64-V2.2.4-20211227.tar.gz) from [Sipeed Website](https://dl.sipeed.com/shareURL/others/toolchain).
4. Move and Extract the downloaded toolchain file to "M1s_BL808_SDK" folder. Update the `{your_download_path}` to your downloaded file location.
    ```bash
    mkdir ~/apps/sipeed/M1s_BL808_SDK/toolchain
    ```
    ```bash
    cd ~/apps/sipeed/M1s_BL808_SDK/toolchain
    ```
    ```bash
    mv {your_download_path}/Xuantie-900-gcc-elf-newlib-x86_64-V2.2.4-20211227.tar.gz .
    ```
    ```bash
    tar -zxvf Xuantie-900-gcc-elf-newlib-x86_64-V2.2.4-20211227.tar.gz -C .
    ```
    ```bash
    mv Xuantie-900-gcc-elf-newlib-x86_64-V2.2.4/ Linux_x86_64
    ```
5. Configure environment variables.
    ```bash
    cat <<EOF>> ~/.bashrc
    export RISCV_TOOLCHAIN=~/apps/sipeed/M1s_BL808_SDK/toolchain/Linux_x86_64/bin
    export BL_SDK_BASE=~/apps/sipeed/M1s_BL808_SDK
    export PATH=$PATH:$RISCV_TOOLCHAIN
    EOF
    ```
    ```bash
    $ source ~/.bashrc
    ```
6. Update the toolchain to fix POSIX error definitions for TFLite. Open the file `error_constants.h` in a text editor located at the paths below: (Both files should be updated)
    ```
    ~/apps/sipeed/M1s_BL808_SDK/toolchain/Linux_x86_64//riscv64-unknown-elf/include/c++/10.2.0/riscv64-unknown-elf/rv64imafdcv_zfh_zvamo_zvlsseg_xtheadc/lp64d/bits/error_constants.h
    ```
    ```
    ~/apps/sipeed/M1s_BL808_SDK/toolchain/Linux_x86_64//riscv64-unknown-elf/include/c++/10.2.0/riscv64-unknown-elf/rv64imafdc_v0p7_zfh_zvamo0p7_zvlsseg0p7_xtheadc/lp64d/bits/error_constants.h
    ```
    
    Uncomment the `#include <cerrno>` and add the `#include <sys/errno.h>` in next line.
    ```C
    #include <bits/c++config.h>
    //#include <cerrno>
    #include <sys/errno.h>
    ```
7.  Update the `MARCH` compile option for the c906 (D0) core; the default is`rv64gcvxthead`. With this option, 32-bit vector operations don't work, and it crashes the firmware. For 32-bit vector operations to work, the `MARCH` option should be changed to `rv64imafdcv0p7_zfh_xtheadc`. Update the `MARCH` option in following files:
    ```
    Line 6: ~/apps/M1s_BL808_SDK/components/platform/soc/bl808/bl808_e907_std/bl808_bsp_driver/startup/d0/BL808_BSP.mk
    ```
    ```
    Line 238, 351: ~/apps/M1s_BL808_SDK/make_scripts_thead_riscv/project.mk
    ```
    ```
    Line 3, 9: ~/apps/M1s_BL808_SDK/components/platform/soc/bl808/bl808_e907_std/bl808_bsp_driver/startup/d0/cpu_flags.cmake
    ```
8. Clone the "person_detection_m1sdock" repository.
    ```bash
    cd ~/ && mkdir workspace && cd workspace
    ```
    ```bash
    git clone https://github.com/umarbinzahid/person_detection_m1sdock
    ```

### Compiling
Open the downloaded "person_detection_m1sdock" folder in vs-code. Press `Ctrl+Shift+B` and it will a prompt to build tasks "Build Non-Vectorized Person Detection" and "Build Vectorized Person Detection". Select the required projetc to compile and it should start building it.

Alternatively, you can open terminal in root of repository and execute `bash build.sh {folder name}` where `{folder name}` can be "person_detection_non_rvv" and "person_detection_rvv" and this should start compiling the example. If everything is followed then it will compile successfully.

### Flashing
When compilation is done. The ouput binary file will be generated in `build_out` folder in root of repository folder.
1. Connect the M1s Dock with OTG interface.
2. Place the device in booloader mode by holding both side button and then press RESET button.
3. A Removable disk drive of small capcity (~ 6 MB) will be shown in the file explorer, Copy the `d0fw.bin` file from `build_out` folder to this drive.
4. Device will auto reboot and new firmware will be flashed.
5. Connect the UART interface and See the output on serial port. See UART Section on [Sipeed WiKi](https://wiki.sipeed.com/hardware/en/maix/m1s/other/start.html#UART-PORT).

See flashing details on [Sipeed WiKi](https://wiki.sipeed.com/hardware/en/maix/m1s/other/start.html#Burn-via-u-disk).

### Debugging
First follow the setups to install t-head debug server from [Sipeed WiKi](https://wiki.sipeed.com/hardware/en/maix/m1s/other/start.html#Use-Jtag)

Prerequisites for Ubuntu 24.04 or higher:
```
sudo ln -s /usr/lib/x86_64-linux-gnu/libncursesw.so.6.4 /usr/lib/x86_64-linux-gnu/libncursesw.so.5
```
```
sudo ln -s /usr/lib/x86_64-linux-gnu/libncurses.so.6.4 /usr/lib/x86_64-linux-gnu/libncurses.so.5
```

1. Flash the binary file of the code you want to debug.
2. Open the UART interface in putty or minicom `minicom -D /dev/ttyUSB1 -b 2000000` for M0 core (Serial Port with big number).
3. Enter command on cli `jtag_cpu0` and it will output something `Enable CPU1 (D0/C906) ... `
4. Start the debug Server by executing command `DebugServerConsole` and it will be connected to D0 core.
5. Open the repository in vs code and update the `program` entry in `.vscode/launch.json` to required program elf file.
6. Click Debug Button on sidebar of vscode and at top you should see `Debug RISC-V` configuration loaded. Click the run button and it should start debugging. Alternately press `Ctrl+Shift+D` and then `F5` to start the debugging.

**Limitations:** If the device is reset during the process, you will need to start from step 2. Also, you will not be able to place a breakpoint everywhere, especially at the start of the program.
