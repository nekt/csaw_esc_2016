OpenRISC Quickstart Guide (ESC 2016)
====================================

**[Click here](https://github.com/nekt/csaw_esc_2016/releases/latest) for the latest release and a copy of ESC 2016 pre-configured Virtual Machine.**

To setup a working OpenRISC environment (instead of using the [pre-configured Virtual Machine](http://tinyurl.com/csaw-esc16-vm)), several components and programs are necessary:
-   A Linux OS, with root/sudo privileges, for hosting the OpenRISC environment;
-   The `fusesoc` package manager, to easily retrieve the OpenRISC system on chip HDL source code;
-   The Linux kernel source code for running an OS over OpenRISC targets;
-   The `or1k-sim` architectural simulator, which can run OpenRISC programs by emulating a complete OpenRISC system at the instruction level;
-   C/C++ toolchains for cross-compiling baremetal and Linux programs to be able to run on an OpenRISC processor.

To implement OpenRISC on an actual FPGA target, the following components are also needed:
-   The Altera Quartus Prime Lite Edition (free), to compile OpenRISC system on chip bitstream for FPGA targets (e.g., the DE0-nano);
-   The `openocd` on-chip debug interface program, to control the deployed target system.

Here, we assume that the host OS is Ubuntu 16.04 LTS. First, we install pre-requisite packages using:
```Shell
sudo apt-get update
sudo apt-get install git aria2 python-yaml libusb-dev libftdi-dev putty iverilog gtkwave
```
Then, we download the ESC 2016 repository using:
```Shell
git clone https://github.com/nekt/csaw_esc_2016.git
```
From within the `csaw_esc_2016/tools` directory, we can type:
```Shell
make basic-tools-download
```
to download `fusesoc`, the Linux kernel source, `or1k-sim`, and the C/C++ toolchains (i.e., baremetal and linux compilers). We can also type:
```Shell
make fpga-tools-download
```
to download `openocd` and Quartus. Finally, to setup all environmental variables, it is sufficient to type:
```Shell
source set-or1k-env
```
from within the `csaw_esc_2016/tools` directory (this is required in every active terminal window). To download all OpenRISC sources, we execute:
```Shell
fusesoc init
```

To build the Linux kernel in ELF format for the OpenRISC processor, we execute the following from within the `linux` directory, depending on the target platform (i.e., FPGA or or1k simulator):
-   To build a kernel compatible with the **or1k simulator**, we type
```Shell
export ARCH=openrisc
export CROSS_COMPILE=or1k-linux-musl-
make distclean
make or1ksim_defconfig
make
mv vmlinux vmlinux_sim
```
-   To build a kernel compatible with the **DE0-nano FPGA target**, we type
```Shell
export ARCH=openrisc
export CROSS_COMPILE=or1k-linux-musl-
make distclean
make de0_nano_defconfig
make
mv vmlinux vmlinux_de0
```
The compiled kernel image will be saved in the `vmlinux_***` file. To add user programs inside the kernel image, the program binaries should be placed within `arch/openrisc/support/initramfs` folder. To compile a simple Hello World program and add it to the kernel, we can create a valid `helloworld.c` within the `linux` folder and then run:
```Shell
or1k-linux-musl-gcc helloworld.c -o arch/openrisc/support/initramfs/hello.elf
make
```
followed by `mv vmlinux vmlinux_de0` or `mv vmlinux vmlinux_sim` depending on **current target configuration** (i.e., `or1ksim_defconfig` or `de0_nano_defconfig`).

To simulate the OpenRISC processor with Linux using `or1k-sim`, run the following from within the `csaw_esc_2016/tools` directory:
```Shell
make simulate-linux
```
As soon as Linux boots, we can type commands such as `ls` and execute our Hello World program typing `./hello.elf` in the new terminal opened by `or1k-sim`.

To build the OpenRISC System on Chip for the DE0-nano, we type `make fpga-bitstream` from within the `csaw_esc_2016/tools` directory. To program the FPGA using Quartus, we type `make program-fpga`. To interface with the FPGA, we need a **3.3Volt** FTDI USB to TTL serial cable connected to the UART [pins of the board](https://sites.google.com/site/fpgaandco/de0-nano-pinout) (default: pins 5 and 6 on port JP3).  

**Connecting any 5.0Volt signal to the FPGA pins must be avoided at all times, as this will permanently destroy (burn) the pins; there is no over-voltage protection on the DE0-nano board. The FTDI adapter must have 3.3Volt I/O, and only those signals can be connected to the FPGA; never connect a 5.0Volt VCC wire to any FPGA pin.** 

To change the default pin assignment for UART, we need to edit `~/.local/share/orpsoc-cores/systems/de0_nano/data/pinmap.tcl` and re-generate the bitstream using `make fpga-bitstream`.

After programming the bitstream to the FPGA, we can connect to the SoC using three (3) terminal windows, as follows: 
-   In the first window, we run OpenOCD using `make run-openocd`; 
-   In the second window, we connect through telnet using `make openocd-connect`, 
-   In the third terminal window, we run `sudo putty` and connect to `dev/ttyUSB0` using the serial connection type and `115200` speed (baud rate). 

To transfer the `vmlinux_de0` kernel image and boot Linux on the FPGA, we type 
```Shell
halt; init; reset; halt; load_image (_path_to_)/linux/vmlinux_de0; reg r3 0; reg npc 0x100; resume
``` 
in the second terminal window (i.e., in the `telnet` prompt).
