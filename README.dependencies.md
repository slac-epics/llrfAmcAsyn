# llrfAmcAsyn dependencies

This module depends on the following external packages:
- llrfAmc
- CPSW (framework)
- Boost
- Yaml-cpp

and on the following EPICS modules:
- Asyn
- yamlLoader

In order to use this module you need to add the following definitions into your IOC application:

## configure/CONFIG_SITE

```
CPSW_FRAMEWORK_PACKAGE_NAME = cpsw/framework
CPSW_FRAMEWORK_VERSION      = R4.4.1
CPSW_FRAMEWORK_TOP          =$(PACKAGE_SITE_TOP)/$(CPSW_FRAMEWORK_PACKAGE_NAME)/$(CPSW_FRAMEWORK_VERSION)
CPSW_FRAMEWORK_LIB          = $(CPSW_FRAMEWORK_TOP)/$(PKG_ARCH)/lib
CPSW_FRAMEWORK_INCLUDE      = $(CPSW_FRAMEWORK_TOP)/$(PKG_ARCH)/include

YAML_PACKAGE_NAME           = yaml-cpp
YAML_VERSION                = yaml-cpp-0.5.3_boost-1.64.0
YAML_TOP                    = $(PACKAGE_SITE_TOP)/$(YAML_PACKAGE_NAME)/$(YAML_VERSION)
YAML_LIB                    = $(YAML_TOP)/$(PKG_ARCH)/lib
YAML_INCLUDE                = $(YAML_TOP)/$(PKG_ARCH)/include

BOOST_PACKAGE_NAME           = boost
BOOST_VERSION                = 1.64.0
BOOST_TOP                    = $(PACKAGE_SITE_TOP)/$(BOOST_PACKAGE_NAME)/$(BOOST_VERSION)
BOOST_LIB                    = $(BOOST_TOP)/$(PKG_ARCH)/lib
BOOST_INCLUDE                = $(BOOST_TOP)/$(PKG_ARCH)/include

LLRFAMC_PACKAGE_NAME         = llrfAmc
LLRFAMC_VERSION              = master
LLRFAMC_TOP                  = $(PACKAGE_SITE_TOP)/$(LLRFAMC_PACKAGE_NAME)/$(LLRFAMC_VERSION)
LLRFAMC_LIB                  = $(LLRFAMC_TOP)/$(PKG_ARCH)/lib
LLRFAMC_INCLUDE              = $(LLRFAMC_TOP)/$(PKG_ARCH)/include
```

## configure/CONFIG_SITE.Common.linuxRT-x86_64

```
PKG_ARCH=$(LINUXRT_BUILDROOT_VERSION)-x86_64
```

## configure/CONFIG_SITE.Common.rhel6-x86_64

```
PKG_ARCH=$(LINUX_VERSION)-x86_64
```

## configure/RELEASE

```
ASYN_MODULE_VERSION=R4.32-1.0.0
YAMLLOADER_MODULE_VERSION=R2.1.0
LLRFAMCASYN_MODULE_VERSION=R1.0.0

ASYN=$(EPICS_MODULES)/asyn/$(ASYN_MODULE_VERSION)
YAMLLOADER=$(EPICS_MODULES)/yamlLoader/$(YAMLLOADER_MODULE_VERSION)
LLRFAMCASYN=$(EPICS_MODULES)/llrfAmcAsyn/$(LLRFAMCASYN_MODULE_VERSION)
```

## xxxApp/src/Makefile

```
# =====================================================
# Path to "NON EPICS" External PACKAGES: USER INCLUDES
# =====================================================
USR_INCLUDES = $(addprefix -I,$(BOOST_INCLUDE) $(CPSW_FRAMEWORK_INCLUDE) $(YAML_INCLUDE) $(LLRFAMC_INCLUDE))
# =====================================================

# ======================================================
#PATH TO "NON EPICS" EXTERNAL PACKAGES: USER LIBRARIES
# ======================================================
cpsw_DIR = $(CPSW_FRAMEWORK_LIB)
yaml-cpp_DIR = $(YAML_LIB)
llrfAmc_DIR = $(LLRFAMC_LIB)
# ======================================================

# ======================================================
# LINK "NON EPICS" EXTERNAL PACKAGE LIBRARIES STATICALLY
# ======================================================
USR_LIBS_Linux += llrfAmc cpsw yaml-cpp
# ======================================================

# llrfAmcAsyn and yamlLoader DBD
xxx_DBD += asyn.dbd
xxx_DBD += llrfAmcAsyn.dbd
xxx_DBD += yamlLoader.dbd

# =====================================================
# Link in the libraries from other EPICS modules
# =====================================================
xxx_LIBS += yamlLoader
xxx_LIBS += llrfAmcAsyn

xxx_LIBS += asyn
```

## xxxApp/Db/Makefile

```
# ==========================================
# llrfAmcAsyn databases
# ==========================================

DB_INSTALLS += $(LLRFAMCASYN)/db/llrfAmcAsyn.db
```