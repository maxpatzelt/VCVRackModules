RACK_DIR ?= ../Rack-SDK

FLAGS +=
CFLAGS +=
CXXFLAGS +=
LDFLAGS +=

SOURCES += $(wildcard src/*.cpp)

DISTRIBUTABLES += res
DISTRIBUTABLES += $(wildcard LICENSE*)
DISTRIBUTABLES += $(wildcard presets)

include $(RACK_DIR)/plugin.mk

# Custom dev install target
DEV_PLUGIN_DIR = "C:\Users\maxpa\AppData\Local/Rack2/plugins-win-x64/NEOFILTRE"

devinstall: all
	mkdir -p $(DEV_PLUGIN_DIR)
	rm -f $(DEV_PLUGIN_DIR)/plugin.dll
	cp -f plugin.dll $(DEV_PLUGIN_DIR)/plugin.dll
	cp -f plugin.json $(DEV_PLUGIN_DIR)/
	cp -rf res $(DEV_PLUGIN_DIR)/
	@echo "Dev-installed NEOFILTRE to $(DEV_PLUGIN_DIR)"

reinstall: clean devinstall
