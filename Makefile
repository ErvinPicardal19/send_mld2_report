include config.mk

APP:=send_mld2_report
export ROOT_DIR
export APP

CFLAGS:= $(shell $(MAKE) -s -C src/ clangd)
CLANGD_CFLAGS:=$(foreach f,$(CFLAGS),"$(f)",)

all: $(BIN_DIR)/$(APP)
	@/bin/echo -e "\033[32mDone.\033[0m"

$(BIN_DIR)/$(APP): prepare
	$(MAKE) -C lib/
	$(MAKE) -C src/

prepare: $(BIN_DIR)

$(BIN_DIR):
	@mkdir -pv $@

clangd:
	echo "CompileFlags:" > .clangd
	echo "  Add: [ $(CLANGD_CFLAGS) ]" >> .clangd


clean:
	make -C src/ clean
	make -C lib/ clean
	rm -rf $(BIN_DIR)

.PHONY: $(SUBDIRS) all clean prepare clangd
