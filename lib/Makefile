include $(ROOT_DIR)/config.mk

LIBNAME	:= libutils.so
VER		:= 1
SONAME	:= $(LIBNAME).$(VER)
LIB		:= $(LIBNAME).$(VER).0

SUBDIRS :=
SUBDIRS += log
SUBDIRS += utils

C_SOURCES	:= $(wildcard $(SUBDIRS:=/*.c))
OBJECTS		:= $(C_SOURCES:%.c=%.o)
DEPENDS		:= $(OBJECTS:%.o=%.d)

CFLAGS		+= -fPIC -Iinclude/

LDFLAGS		+= -shared -Wl,-soname,$(SONAME)

all: $(LIB)

$(LIB): $(OBJECTS)
	echo $(OBJECTS)
	gcc $(LDFLAGS) -o $@ $^
	ln -sf $@ $(SONAME)
	ln -sf $@ $(LIBNAME)

%.o: %.c
	gcc $(CFLAGS) -c $< -o $@

clangd:

clean:
	rm -rf $(OBJECTS) $(DEPENDS) $(LIBNAME)*

-include $(DEPENDS)

.PHONY: all clean clangd
