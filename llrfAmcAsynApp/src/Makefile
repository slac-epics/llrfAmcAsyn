TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

USR_CXXFLAGS += -std=c++0x

# <name>.dbd will be created from <name>Include.dbd
DBD += llrfAmcAsyn.dbd

INC += drvLLRFAMCASYN.h

LIBRARY_IOC += llrfAmcAsyn
LIB_SRCS += drvLLRFAMCASYN.cpp
LIB_LIBS += asyn
LIB_LIBS += yamlLoader


#=====================================================
# Path to "NON EPICS" External PACKAGES: USER INCLUDES
#======================================================
USR_INCLUDES = $(addprefix -I,$(BOOST_INCLUDE) $(CPSW_FRAMEWORK_INCLUDE) $(YAML_INCLUDE) $(LLRFAMC_INCLUDE))
cpsw_DIR = $(CPSW_FRAMEWORK_LIB)
yaml-cpp_DIR = $(YAML_LIB)
llrfAmc_DIR = $(LLRFAMC_LIB)
USR_LIBS_Linux += llrfAmc cpsw yaml-cpp
#======================================================

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE
