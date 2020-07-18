/**
 *-----------------------------------------------------------------------------
 * Title      : LLRF AMC Driver EPICS Module
 * ----------------------------------------------------------------------------
 * File       : drvLLRFAMCASYN.cpp
 * Author     : Jesus Vasquez, jvasquez@slac.stanford.edu
 * Created    : 2020-07-16
 * ----------------------------------------------------------------------------
 * Description:
 * EPICS module for the LLRF AMC card low level driver llrfAmc.
 * ----------------------------------------------------------------------------
 * This file is part of llrfAmcAsyn. It is subject to
 * the license terms in the LICENSE.txt file found in the top-level directory
 * of this distribution and at:
    * https://confluence.slac.stanford.edu/display/ppareg/LICENSE.html.
 * No part of llrfAmcAsyn, including this file, may be
 * copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE.txt file.
 * ----------------------------------------------------------------------------
**/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <getopt.h>
#include <iostream>
#include <arpa/inet.h>
#include <cstdlib>
#include <epicsTypes.h>
#include <epicsTime.h>
#include <epicsThread.h>
#include <epicsString.h>
#include <epicsTimer.h>
#include <epicsMutex.h>
#include <epicsEvent.h>
#include <iocsh.h>

#include <dbAccess.h>
#include <dbStaticLib.h>

#include "drvLLRFAMCASYN.h"
#include "asynPortDriver.h"
#include <epicsExport.h>

#include <cpsw_api_builder.h>
#include <cpsw_api_user.h>
#include <yaml-cpp/yaml.h>

#include "yamlLoader.h"

LLRFAMCASYN::LLRFAMCASYN(const std::string& pn)
:
    asynPortDriver(
        pn.c_str(),             // Port name
        1,                      // Max number of address
        asynUInt32DigitalMask,  // Interface Mask
        asynUInt32DigitalMask,  // Interrupt Mask
        ASYN_CANBLOCK,          // asynFlags
        1,                      // Autoconnect
        0,                      // Default priority
        0),                     // Default stack size
    driverName("LlrfAmcAsyn"),                  // Driver name
    portName(pn),                               // Port name
    llrfAmc(ILlrfAmc::create(cpswGetRoot())),   // llrfAmc object
    paramInitName("INIT"),                      // INIT parameter name
    paraminitStatName("INIT_STAT"),             // INIT_STAT parameter name
    paramInitMask(0x01),                        // INIT parameter mask
    paramInitStatMask(0x03)                     // INIT_STAT parameter mask
{
    // Create asyn parameters
    createParam(paramInitName.c_str(),     asynParamUInt32Digital, &paramInitIndex);
    createParam(paraminitStatName.c_str(), asynParamUInt32Digital, &paramInitStatIndex);

    // Print the down and up converter module names
    std::cout << driverName << " : Down converter module name : " << llrfAmc->getDownConv()->getModuleName() << std::endl;
    std::cout << driverName << " : Up converter module name   : " << llrfAmc->getUpConv()->getModuleName() << std::endl;

    // Initialize the LlrfAmc object
    std::cout << driverName << " : Initializing the LLRF AMC cards..." << std::endl;
    bool success { llrfAmc->init() };
    if (success)
        success &= llrfAmc->isInited();

    // Check if the initialization succeed and update the INIT_STAT parameter
    if (!success) {
        std::cerr << driverName << " : Initialization failed!" << std::endl;
        setUIntDigitalParam(paramInitStatIndex, INIT_STAT_FAILED, paramInitStatMask);
    } else {
        std::cout << driverName << " : Initialization succeed!" << std::endl;
        setUIntDigitalParam(paramInitStatIndex, INIT_STAT_SUCCEED, paramInitStatMask);
    }
}

// + LlrfAmcAsynConfig //
extern "C" int LlrfAmcAsynConfig(const char *portName)
{
    new LLRFAMCASYN(portName);

    return asynSuccess;
}

static const iocshArg LlrfAmcAsynConfigArg0 = { "portName", iocshArgString };

static const iocshArg * const LlrfAmcAsynConfigArgs[] = {
    &LlrfAmcAsynConfigArg0
};

static const iocshFuncDef LlrfAmcAsynConfigFuncDef = { "LlrfAmcAsynConfig", 1, LlrfAmcAsynConfigArgs };

static void LlrfAmcAsynConfigCallFunc(const iocshArgBuf *args)
{
    LlrfAmcAsynConfig(args[0].sval);
}
// - LlrfAmcAsynConfig //

// + LlrfAmcAsynInit //
extern "C" int LlrfAmcAsynInit()
{
    return asynSuccess;
}

static const iocshFuncDef LlrfAmcAsynInitFuncDef = { "LlrfAmcAsynInit", 0 };

static void LlrfAmcAsynInitCallFunc(const iocshArgBuf *args)
{
    LlrfAmcAsynInit();
}
// - LlrfAmcAsynInit //

void drvLLRFAMCASYNRegister(void)
{
    iocshRegister( &LlrfAmcAsynConfigFuncDef, LlrfAmcAsynConfigCallFunc );
    iocshRegister( &LlrfAmcAsynInitFuncDef,   LlrfAmcAsynInitCallFunc   );
}

extern "C" {
    epicsExportRegistrar(drvLLRFAMCASYNRegister);
}
