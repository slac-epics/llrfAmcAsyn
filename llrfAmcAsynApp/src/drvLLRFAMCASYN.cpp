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
        portName.c_str(),
        1,
        asynInt32Mask | asynDrvUserMask | asynOctetMask | \
        asynUInt32DigitalMask | asynFloat64Mask,                                    // Interface Mask
        asynInt32Mask | asynUInt32DigitalMask | asynFloat64Mask | asynOctetMask,    // Interrupt Mask
        ASYN_MULTIDEVICE | ASYN_CANBLOCK,                                           // asynFlags
        1,                                                                          // Autoconnect
        0,                                                                          // Default priority
        0),                                                                         // Default stack size
    driverName("LlrfAmcAsyn"), // Driver name
    portName(pn),              // Port name
    llrfAmc(cpswGetRoot())
{
    // Initialize the LlrfAmc object
    std::cout << driverName << " : Initializing the LLRF AMC cards..." << std::endl;
    bool success;
    if ( success =  llrfAmc->init() )
    {
        success &= llrfAmc->isInited();
    }
    std::cout << std::endl;

    // Check if the initialization succeed
    if ( !success )
        std::cerr << driverName << " : Initialization failed!" << std::endl;
    else
        std::cout << driverName << " : Initialization succeed!" << std::endl;
    std::cout << std::endl;
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
