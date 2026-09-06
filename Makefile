# TI C2000 CGT 25.11.1.LTS; install separately and set C2000_CGT_ROOT.
C2000_CGT_ROOT ?= tools/ti-cgt-c2000_25.11.1.LTS
CL2000 := $(C2000_CGT_ROOT)/bin/cl2000
OFD2000 := $(C2000_CGT_ROOT)/bin/ofd2000
FLAGS := --abi=coffabi --float_support=fpu32 --tmu_support=tmu0 --cla_support=cla1 --disable_pcd
OBJECTS := build/patches.obj build/features.obj build/features_rhythm.obj
HEADERS := $(wildcard src/*.h)
.PHONY: all check clean
all: build/subharmonicon.subhpatch
build:
	mkdir -p $@
build/patches.obj: src/patches.asm src/stock_abi.inc src/features_bridge.asm src/features_rhythm_bridge.asm | build
	$(CL2000) $(FLAGS) --compile_only --obj_directory=build $<
build/%.obj: src/%.c $(HEADERS) | build
	$(CL2000) $(FLAGS) --include_path=$(C2000_CGT_ROOT)/include --fp_mode=relaxed --opt_level=2 --opt_for_speed=5 --compile_only --obj_directory=build $<
build/extension.out: $(OBJECTS) src/extension_link.cmd
	$(CL2000) --abi=coffabi --run_linker --entry_point=feature_startup_hook --output_file=$@ --map_file=build/extension.map $(OBJECTS) src/extension_link.cmd
build/extension.xml: build/extension.out
	$(OFD2000) -x --xml_indent=0 $< > $@
build/subharmonicon.subhpatch: build/extension.xml tools/make_patch.py patcher.py firmware_sysex.py src/patch_layout.json
	python3 tools/make_patch.py $< $@
CHECK_NAMES := harmony velocity glide config panel controls voices math clock schedule
CHECKS := $(addprefix build/check-,$(CHECK_NAMES))
build/check-%: tests/test_features_%.c $(HEADERS) | build
	$(CC) -std=c11 -Wall -Wextra -Werror -Isrc $< -lm -o $@
check: $(CHECKS)
	@for check in $(CHECKS); do ./$$check || exit $$?; done
	python3 -m unittest discover -s tests -p 'test_firmware_sysex.py'
clean:
	rm -rf build

.PHONY: release
release: all
	python3 tools/package_release.py
