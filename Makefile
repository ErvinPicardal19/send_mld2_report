include config.mk

APP:=main
export ROOT_DIR
export APP

all: $(BIN_DIR)/$(APP)
	@echo "\033[32mDone.\033[0m"

$(BIN_DIR)/$(APP): prepare
	$(MAKE) -C lib/
	$(MAKE) -C src/

prepare: $(BIN_DIR)

$(BIN_DIR):
	@mkdir -pv $@

clean:
	make -C src/ clean
	make -C lib/ clean
	rm -rf $(BIN_DIR)

.PHONY: $(SUBDIRS) all clean prepare
