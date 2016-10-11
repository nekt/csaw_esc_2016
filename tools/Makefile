SHELL:=/bin/bash
.DEFAULT_GOAL := help
.PHONY: help exploits

help:
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | sort | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-22s\033[0m %s\n", $$1, $$2}'

prerequisites: ## Install the required OS packages
	sudo apt-get update
	sudo apt-get install git aria2 python-yaml libusb-dev libftdi-dev putty iverilog gtkwave

fpga-tools-download: openocd-download quartus-lite-download quartus-configure ## Download OpenOCD and Quartus software for FPGA targets

openocd-download:
	rm -rf openocd
	aria2c -x 16 https://github.com/openrisc/tutorials/releases/download/2016.1/openocd.tgz
	tar -xzf openocd.tgz
	rm -f openocd.tgz

quartus-lite-download:
	rm -rf quartus
	mkdir -p quartus
	aria2c -x 16 http://download.altera.com/akdlm/software/acdsinst/16.0/211/ib_tar/Quartus-lite-16.0.0.211-linux.tar
	tar -xf Quartus-lite-16.0.0.211-linux.tar -C quartus
	rm -f Quartus-lite-16.0.0.211-linux.tar
	./quartus/setup.sh
	
quartus-configure:
	sudo rm -f /etc/udev/rules.d/*usbblaster.rules
	sudo cp 92-usbblaster.rules /etc/udev/rules.d/
	sudo udevadm control --reload
	sudo mkdir -p /etc/jtagd/
	sudo cp $(HOME)/altera_lite/16.0/quartus/linux64/pgm_parts.txt /etc/jtagd/jtagd.pgm_parts
	
fix-jtag: ## Fix JTAG connectivity for FPGA targets
	@echo "Disconnect the JTAG cable for a few seconds and reconnect"
	@read -n 1 -s -p "Press any key when ready to proceed..."
	@echo
	sudo killall -9 jtagd
	sudo $(HOME)/altera_lite/16.0/quartus/bin/jtagconfig

basic-tools-download: baremetal-compiler linux-compiler fusesoc-compiled or1ksim-compiled linux-kernel-source ## Download Linux, compilers, or1ksim and source manager 

baremetal-compiler:
	rm -rf baremetal-compiler
	mkdir -p baremetal-compiler
	aria2c -x 16 https://github.com/openrisc/newlib/releases/download/v2.3.0-1/or1k-elf_gcc5.3.0_binutils2.26_newlib2.3.0-1_gdb7.11.tgz
	tar -xzf or1k-elf_gcc5.3.0_binutils2.26_newlib2.3.0-1_gdb7.11.tgz -C baremetal-compiler
	rm -f or1k-elf_gcc5.3.0_binutils2.26_newlib2.3.0-1_gdb7.11.tgz

linux-compiler:
	rm -rf linux-compiler
	mkdir -p linux-compiler
	aria2c -x 16 https://github.com/openrisc/musl-cross/releases/download/gcc5.3.0-musl1.1.14/or1k-linux-musl_gcc5.3.0_binutils2.26_musl1.1.14.tgz
	tar -xzf or1k-linux-musl_gcc5.3.0_binutils2.26_musl1.1.14.tgz -C linux-compiler
	rm -f or1k-linux-musl_gcc5.3.0_binutils2.26_musl1.1.14.tgz

fusesoc-compiled:
	rm -rf fusesoc
	aria2c -x 16 https://github.com/openrisc/tutorials/releases/download/2016.1/fusesoc.tgz
	tar -xzf fusesoc.tgz
	rm -f fusesoc.tgz

or1ksim-compiled:
	rm -rf or1ksim
	aria2c -x 16 https://github.com/openrisc/tutorials/releases/download/2016.1/or1ksim.tgz
	tar -xzf or1ksim.tgz
	rm -f or1ksim.tgz

linux-kernel-source:
	rm -rf linux
	git clone --depth 1 -b master https://github.com/openrisc/linux.git
	
fpga-bitstream: ## Generate an FPGA bitstream for OpenRISC SoC
	fusesoc build de0_nano

program-fpga: ## Program the FPGA using JTAG
	fusesoc pgm de0_nano
	
simulate-linux: ## Run Linux with the OpenRISC simulator
	or1k-sim -f or1ksim.cfg linux/vmlinux_sim

run-openocd: ## Start the OpenOCD service
	openocd -s ${OPENOCD}/share/openocd/scripts/ -f interface/altera-usb-blaster.cfg -f or1k-dev.tcl

connect-openocd: ## Connect to the OpenOCD service using telnet
	@echo "To run Linux type the FGPA type the following (ensure that you use the correct path to vmlinux_de0):"
	@echo
	@echo "halt; init; reset; halt; load_image $(PWD)/linux/vmlinux_de0; reg r3 0; reg npc 0x100; resume"
	@echo 
	telnet localhost 4444

exploits: ## Compile sample exploitation programs for OpenRISC Linux
	or1k-linux-musl-gcc -o exploits/stack exploits/stack.c
	or1k-linux-musl-gcc -o exploits/format exploits/format.c
	or1k-linux-musl-gcc -o exploits/ptr exploits/ptr.c
	or1k-linux-musl-gcc -o exploits/priv exploits/priv.c
