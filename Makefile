.PHONY: all clean

EXAMPLE_OBJ=forward calc advCalc checksum addhdr rmvhdr
QSPI=false
SHELL := /bin/bash
VIVADO_TARGET_VER=2021.2
VIVADO_VER=$(shell vivado -version | grep -o '[^v][0-9]*\.[0-9]')
DIST_DIR=dist


# Define default implement arguments and default values
TCL_ARGS_LIST=board tag build_timestamp impl synth_ip post_impl user_plugin user_build_dir num_cmac_port num_phys_func
SYN_ARGS_LIST=impl synth_ip post_impl
if_synth=1
board=au280
build_timestamp=$(shell date +%y%m%d%H%M)
num_cmac_port=2
num_phys_func=2

# user_build_dir must be full path. Use build_dir to give the relative path 
build_dir=build
cur_dir=$(shell pwd)
user_build_dir=$(cur_dir)/$(build_dir)


all: $(EXAMPLE_OBJ) 
$(EXAMPLE_OBJ): CHECK_VIVADO_VER
	$(eval tag=$@_$(build_timestamp))
	$(eval user_plugin=../../Examples/$@)
	$(eval DIST_APP_DIR=$(DIST_DIR)/$(board)_$(tag)_dist)
	#$(eval post_impl=$(if_synth))
	$(foreach arg, $(SYN_ARGS_LIST), $(eval $(arg)=$(if_synth)))
	$(eval TCL_ARGS=$(foreach arg, $(TCL_ARGS_LIST), -$(arg) $($(arg))))
	$(info TCL_ARGS=$(TCL_ARGS))

	#build
	@[ -d '$(user_build_dir)' ] || mkdir $(user_build_dir) 
	cd open-nic-shell/script && vivado -mode batch -source build.tcl -tclargs $(TCL_ARGS) | tee $(cur_dir)/build_$(tag).log

	@if [ $(if_synth) = 1 ]; then { \
		echo "Condition is true"; \
		@[ -d '$(DIST_APP_DIR)' ] || mkdir -p $(DIST_APP_DIR); \
		cp -r $(build_dir)/$(board)_$(tag)/open_nic_shell/open_nic_shell.gen/sources_1/ip/vitis_net_p4_0/src/sw/drivers $(DIST_APP_DIR)/; \
		cd $(DIST_APP_DIR)/drivers && make; \
		cp -r Examples/$@/c-driver/* $(DIST_APP_DIR)/drivers/install/; \
		cd $(DIST_APP_DIR)/drivers/install && make; \
		cp $(build_dir)/$(board)_$(tag)/open_nic_shell/open_nic_shell.runs/impl_1/open_nic_shell.mcs $(DIST_APP_DIR)/$@.mcs; \
	}; \
	fi

shell: CHECK_VIVADO_VER
	$(eval tag=$@_$(build_timestamp))
	$(eval DIST_APP_DIR=$(DIST_DIR)/$(board)_$(tag)_dist)
	$(if $(strip $(custom_plugin)),$(eval user_plugin=$(cur_dir)/$(custom_plugin)))
	$(eval TCL_ARGS=$(foreach arg, $(TCL_ARGS_LIST), $(if $(strip $($(arg))), -$(arg) $($(arg))))) 
	$(info TCL_ARGS=$(TCL_ARGS))
	#build
	@[ -d '$(user_build_dir)' ] || mkdir $(user_build_dir) 
	cd open-nic-shell/script && vivado -mode batch -source build.tcl -tclargs $(TCL_ARGS) | tee $(cur_dir)/build_$(tag).log

CHECK_VIVADO_VER:
ifeq ($(VIVADO_VER), $(VIVADO_TARGET_VER))
	@echo "Current Vivado Verion is $(VIVADO_VER)"
else
	@echo "This Makefile requires VIVADO_VER as 2021.2"
	@echo "Please make sure your have source your VIVADO_ROOT/settings64.sh"
	exit 1;
endif
		

	
clean:
	rm -rf $(build_dir)
	rm -rf $(DIST_DIR)
	rm -rf *.log
