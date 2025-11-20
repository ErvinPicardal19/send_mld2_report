include $(ROOT_DIR)/config.mk

C_SOURCES	:= $(wildcard *.c)
OBJECTS		:= $(C_SOURCES:%.c=%.o)
DEPENDS		:= $(OBJECTS:%.o=%.d)

CFLAGS		?= -Wall -Wextra -MMD -MP
CFLAGS		+= -I$(LIB_DIR)/ -Iinclude/

LDFLAGS		:= -L$(LIB_DIR)/ -lutils

all: $(APP)

$(APP): $(OBJECTS)
	$(CC) -o $@ $^ $(LDFLAGS)
	install -m 755 -C $@ $(BIN_DIR)/$@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clangd:
	@echo $(CFLAGS)

clean:
	rm -rf *.o *.d $(APP)

-include $(DEPENDS)

.PHONY: all clean clangd
