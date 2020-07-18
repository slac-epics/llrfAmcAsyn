# How to configure the driver

## Overview

llrfAmcAsyn is an EPICS module for the LLRF AMC card low level driver [llrfAmc](https://github.com/slaclab/llrfAmc). It uses AsynPortDriver for its integration into EPICS.

Before trying to configure the driver, make sure you included it and its dependencies in your application as described in [README.dependencies.md](README.dependencies.md).

## llrfAmcAsyn Configuration

In order to use `llrfAmcAsyn` in your application you must call `LlrfAmcAsynConfig` on your IOC's `st.cmd` script with the following parameters

```
LlrfAmcAsynConfig(PORT_NAME)
```

| Parameter           | Description
|---------------------|-----------------------------
| PORT_NAME           | The name given to this asyn port driver.

## Use of the yamlLoader Module

You must use the `yamlLoader` module with this module.

Before loading this modules in you `st.cmd` file, you must first:
- Load the YAML file description file to construct the CPSW root, by calling `cpswLoadYamlFile()`, and
- Load the default YAML configuration by calling `cpswLoadConfigFile()`.

So, your `st.cmd` file should look something like this:

```
# ==========================================
# Create the CPSW root using the yamlLoader
# ==========================================
cpswLoadYamlFile("${YAML}", "NetIODev", "", "${FPGA_IP}")

# ==========================================
# Load application specific configurations
# ==========================================
# Load the defautl configuration
cpswLoadConfigFile("${DEFAULTS_FILE}", "mmio")

# ===========================================
#              DRIVER SETUP
# ===========================================

## Configure the llrfAmcAsyn driver
# LlrfAmcAsynConfig(
#    Port Name)     # The name given to this port driver
LlrfAmcAsynConfig("${LLRFAMCASYN_PORT}")
```
