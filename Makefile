CC = g++
PROJ_PATH := $(shell pwd)
TARGET_CFLAGS += -g -Wall -DDEBUG -I$(PROJ_PATH)  --std=c++11
TARGET_LDFLAGS := -lpthread -levent 

MODULES := mod_base
MODULES += mod_network
MODULES += mod_server
MODULES += mod_ftp
TARGET_NAME = ftp_server
OBJ_DIR = $(PROJ_PATH)/objs

all:$(MODULES) $(TARGET_NAME)
	@echo "compile done!"

$(TARGET_NAME):
	$(MAKE) -C $(PROJ_PATH)/objs LDFLAGS="$(TARGET_LDFLAGS)" TARGET="$(TARGET_NAME)" all

mod_base :
	$(MAKE) -C $(PROJ_PATH)/base CFLAGS="$(TARGET_CFLAGS)" OBJ_DIR="$(OBJ_DIR)" all

mod_network :
	$(MAKE) -C $(PROJ_PATH)/network CFLAGS="$(TARGET_CFLAGS)" OBJ_DIR="$(OBJ_DIR)" all

mod_server :
	$(MAKE) -C $(PROJ_PATH)/server CFLAGS="$(TARGET_CFLAGS)" OBJ_DIR="$(OBJ_DIR)" all

mod_ftp :
	$(MAKE) -C $(PROJ_PATH)/ftp CFLAGS="$(TARGET_CFLAGS)" OBJ_DIR="$(OBJ_DIR)" all

.PHONY: clean
clean:
	make -C ./base clean
	make -C ./network clean
	make -C ./server clean
	make -C ./ftp clean
	make -C ./objs TARGET="$(TARGET_NAME)"  clean
	@rm $(TARGET_NAME)
