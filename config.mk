ROOT_DIR	?= $(realpath $(dir $(lastword $(MAKEFILE_LIST))))

BIN_DIR		:= $(ROOT_DIR)/bin
LIB_DIR		:= $(ROOT_DIR)/lib
SRC_DIR		:= $(ROOT_DIR)/src

CC			:= gcc
CFLAGS		:= -Wall -Wextra -O2 -MMD -MP
