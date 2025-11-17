# Path to Rack SDK
RACK_DIR ?= /c/Users/maxpa/Documents/RackDev/rack-sdk

 # (Removed custom default goal; use upstream plugin.mk defaults)

# Plugin slug
SLUG = OBF

# Source files - must be set before including plugin.mk
SOURCES += src/plugin.cpp
SOURCES += src/OBFModule.cpp

# Include Rack build system
include $(RACK_DIR)/plugin.mk

# Phony meta targets
.PHONY: all plugin devinstall reinstall

DEV_PLUGIN_DIR := $(LOCALAPPDATA)/Rack2/plugins-win-x64/$(SLUG)

# Development install: copy raw files directly for live debugging (Rack must be closed)
devinstall: all
	mkdir -p "$(DEV_PLUGIN_DIR)"
	rm -f "$(DEV_PLUGIN_DIR)/plugin.dll"
	cp -f plugin.dll "$(DEV_PLUGIN_DIR)/plugin.dll"
	cp -f plugin.json "$(DEV_PLUGIN_DIR)/"
	cp -rf res "$(DEV_PLUGIN_DIR)/"
	@echo "Dev-installed $(SLUG) to $(DEV_PLUGIN_DIR)"

# Convenience target: clean then devinstall
reinstall: clean devinstall
	@echo "Reinstalled $(SLUG) (clean build + dev copy)"

# (Removed custom default goal; use upstream plugin.mk defaults)
