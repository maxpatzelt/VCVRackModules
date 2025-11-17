# Path to Rack SDK
RACK_DIR ?= ../Rack-SDK

# Plugin slug
SLUG = DubBoite

# Source files
SOURCES += src/plugin.cpp
SOURCES += src/DubBoiteModule.cpp

# Include Rack build system
include $(RACK_DIR)/plugin.mk

# Phony meta targets
.PHONY: all plugin devinstall reinstall

DEV_PLUGIN_DIR := $(LOCALAPPDATA)/Rack2/plugins-win-x64/$(SLUG)

# Development install
devinstall: all
	mkdir -p "$(DEV_PLUGIN_DIR)"
	rm -f "$(DEV_PLUGIN_DIR)/plugin.dll"
	cp -f plugin.dll "$(DEV_PLUGIN_DIR)/plugin.dll"
	cp -f plugin.json "$(DEV_PLUGIN_DIR)/"
	cp -rf res "$(DEV_PLUGIN_DIR)/"
	@echo "Dev-installed $(SLUG) to $(DEV_PLUGIN_DIR)"

# Convenience target: clean then devinstall
reinstall: clean devinstall
	@echo "Reinstalled $(SLUG)"
