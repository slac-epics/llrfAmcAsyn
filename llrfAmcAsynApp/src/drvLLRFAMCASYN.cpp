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
#include <asynPortDriver.h>
#include <epicsExport.h>

#include <cpsw_api_builder.h>
#include <cpsw_api_user.h>
#include <yaml-cpp/yaml.h>
#include <LlrfAmcLogger.h>
#include <yamlLoader.h>

#include "drvLLRFAMCASYN.h"

LLRFAMCASYN::LLRFAMCASYN(const std::string& pn)
:
    asynPortDriver(
        pn.c_str(),                                 // Port name
        1,                                          // Max number of address
        asynDrvUserMask | asynUInt32DigitalMask,    // Interface Mask
        asynUInt32DigitalMask,                      // Interrupt Mask
        ASYN_CANBLOCK,                              // asynFlags
        1,                                          // Autoconnect
        0,                                          // Default priority
        0),                                         // Default stack size
    driverName("LlrfAmcAsyn"),                  // Driver name
    portName(pn),                               // Port name
    llrfAmc(ILlrfAmc::create(cpswGetRoot())),   // llrfAmc object
    paramInitName("INIT"),                      // INIT parameter name
    paramInitStatName("INIT_STAT"),             // INIT_STAT parameter name
    paramCheckName("CHECK"),                    // CHECK parameter name
    paramDCStatName("DC_STAT"),                 // DC_STAT parameter name
    paramUCStatName("UC_STAT"),                 // UC_STAT parameter name
    paramInitMask(0x01),                        // INIT parameter mask
    paramInitStatMask(0x03),                    // INIT_STAT parameter mask
    paramCheckMask(0x01),                       // CHECK parameter mask
    paramXCStatMask(0x03)                       // xC_STAT parameter mask
{
    // Create asyn parameters
    createParam(paramInitName.c_str(),     asynParamUInt32Digital, &paramInitIndex);
    createParam(paramInitStatName.c_str(), asynParamUInt32Digital, &paramInitStatIndex);
    createParam(paramCheckName.c_str(),    asynParamUInt32Digital, &paramCheckIndex);
    createParam(paramDCStatName.c_str(),   asynParamUInt32Digital, &paramDCStatIndex);
    createParam(paramUCStatName.c_str(),   asynParamUInt32Digital, &paramUCStatIndex);

    // Print the down and up converter module names
    std::cout << driverName << " : Down converter module name : " << llrfAmc->getDownConv()->getModuleName() << std::endl;
    std::cout << driverName << " : Up converter module name   : " << llrfAmc->getUpConv()->getModuleName() << std::endl;

    // Initialize the LlrfAmc object
    std::cout << driverName << " : Initializing the LLRF AMC cards..." << std::endl;
    bool success { llrfAmc->init() };

    // Check if the initialization succeed and update the INIT_STAT and xC_STAT parameters
    if (success) {
        std::cout << driverName << " : Initialization succeed!" << std::endl;

        // If 'llrfAmc->init()' succeed, the both up and down converter cards are locked.
        setUIntDigitalParam(paramInitStatIndex, INIT_STAT_SUCCEED, paramInitStatMask);
        setUIntDigitalParam(paramDCStatIndex,   XC_STAT_LOCKED,    paramXCStatMask);
        setUIntDigitalParam(paramUCStatIndex,   XC_STAT_LOCKED,    paramXCStatMask);
    } else {
        std::cerr << driverName << " : Initialization failed!" << std::endl;

        // If 'llrfAmc->init()' failed, then we need to check if the up and down converter
        // status individually.
        setUIntDigitalParam(paramInitStatIndex, INIT_STAT_FAILED,            paramInitStatMask);
        setUIntDigitalParam(paramDCStatIndex,   llrfAmc->isDownConvInited(), paramXCStatMask);
        setUIntDigitalParam(paramUCStatIndex,   llrfAmc->isUpConvInited(),   paramXCStatMask);
    }
}

asynStatus LLRFAMCASYN::writeUInt32Digital(asynUser *pasynUser, epicsUInt32 value, epicsUInt32 mask)
{
    int function { pasynUser->reason };
    static const char *functionName = "writeUInt32Digital";
    asynStatus status;

    if(function == paramInitIndex)
    {
        // Update the parameter values to "in progress..."
        setUIntDigitalParam(paramInitStatIndex, INIT_STAT_INPROGRESS, paramInitStatMask);
        setUIntDigitalParam(paramDCStatIndex,   XC_STAT_INPROGRESS,   paramXCStatMask);
        setUIntDigitalParam(paramUCStatIndex,   XC_STAT_INPROGRESS,   paramXCStatMask);
        callParamCallbacks();

        asynPrint(pasynUser, ASYN_TRACEIO_DRIVER, \
            "%s::%s, function %d, port %s : Calling llrfAmc->init()\n", \
            driverName.c_str(), functionName, function, (this->portName).c_str());

        bool success = llrfAmc->init();

        if (success)
        {
            asynPrint(pasynUser, ASYN_TRACEIO_DRIVER, \
                "%s::%s, function %d, port %s : Call to llrfAmc->init() succeed!\n", \
                driverName.c_str(), functionName, function, (this->portName).c_str());

            // Update INIT_STAT and xC_STAT parameter values
            setUIntDigitalParam(paramInitStatIndex, INIT_STAT_SUCCEED, paramInitStatMask);
            setUIntDigitalParam(paramDCStatIndex,   XC_STAT_LOCKED,    paramXCStatMask);
            setUIntDigitalParam(paramUCStatIndex,   XC_STAT_LOCKED,    paramXCStatMask);

            status = asynSuccess;
        }
        else
        {
            asynPrint(pasynUser, ASYN_TRACE_ERROR, \
                "%s::%s, function %d, port %s : Call to llrfAmc->init() failed!\n", \
                driverName.c_str(), functionName, function, (this->portName).c_str());

            // Update INIT_STAT and xC_STAT parameter values
            setUIntDigitalParam(paramInitStatIndex, INIT_STAT_FAILED, paramInitStatMask);

            asynPrint(pasynUser, ASYN_TRACEIO_DRIVER, \
                "%s::%s, function %d, port %s : Calling llrfAmc->isDownConvInited()\n", \
                driverName.c_str(), functionName, function, (this->portName).c_str());

            setUIntDigitalParam(paramDCStatIndex,   llrfAmc->isDownConvInited(), paramXCStatMask);

            asynPrint(pasynUser, ASYN_TRACEIO_DRIVER, \
                "%s::%s, function %d, port %s : Calling llrfAmc->isUpConvInited()\n", \
                driverName.c_str(), functionName, function, (this->portName).c_str());

            setUIntDigitalParam(paramUCStatIndex,   llrfAmc->isUpConvInited(),   paramXCStatMask);

            status = asynError;
        }
        callParamCallbacks();
    }
    else if (function == paramCheckIndex)
    {
        // Update the parameter values to "in progress..."
        setUIntDigitalParam(paramDCStatIndex, XC_STAT_INPROGRESS, paramXCStatMask);
        setUIntDigitalParam(paramUCStatIndex, XC_STAT_INPROGRESS, paramXCStatMask);
        callParamCallbacks();

        asynPrint(pasynUser, ASYN_TRACEIO_DRIVER, \
            "%s::%s, function %d, port %s : Calling llrfAmc->isDownConvInited()\n", \
            driverName.c_str(), functionName, function, (this->portName).c_str());

        setUIntDigitalParam(paramDCStatIndex, llrfAmc->isDownConvInited(), paramXCStatMask);

        asynPrint(pasynUser, ASYN_TRACEIO_DRIVER, \
            "%s::%s, function %d, port %s : Calling llrfAmc->isUpConvInited()\n", \
            driverName.c_str(), functionName, function, (this->portName).c_str());

        setUIntDigitalParam(paramUCStatIndex, llrfAmc->isUpConvInited(),   paramXCStatMask);

        callParamCallbacks();

        status = asynSuccess;
    }
    else if ( function == paramInitStatIndex || function == paramDCStatIndex || function == paramUCStatIndex )
    {
        // The 'INIT_STAT' and 'xC_STAT' parameters are write-only, so avoid them to be changed by the user.
        asynPrint(pasynUser, ASYN_TRACE_ERROR, \
            "%s::%s, function %d, port %s : Parameter %s is write-only.\n", \
            driverName.c_str(), functionName, function, (this->portName).c_str(), paramInitStatName.c_str());

        status = asynError;
    }
    else
    {
        status = asynPortDriver::writeUInt32Digital(pasynUser, value, mask);
    }

    return status;
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

// + LlrfAmcAsynSetLogLevel //
extern "C" int LlrfAmcAsynSetLogLevel(int logLevel)
{
    asynStatus status = asynSuccess;
    switch(logLevel) {
        case 0:
            ILogger::setLevel(LoggerLevel::Debug);
            break;
        case 1:
            ILogger::setLevel(LoggerLevel::Warning);
            break;
        case 2:
            ILogger::setLevel(LoggerLevel::Error);
            break;
        default:
            status = asynError;
            break;
    }

    return status;
}

static const iocshArg LlrfAmcAsynSetLogLevelArg0 = { "logLevel (0: Debug, 1: Warning, 2: Error)", iocshArgInt };

static const iocshArg * const LlrfAmcAsynSetLogLevelArgs[] = {
    &LlrfAmcAsynSetLogLevelArg0
};

static const iocshFuncDef LlrfAmcAsynSetLogLevelFuncDef = { "LlrfAmcAsynSetLogLevel", 1, LlrfAmcAsynSetLogLevelArgs };

static void LlrfAmcAsynSetLogLevelCallFunc(const iocshArgBuf *args)
{
    LlrfAmcAsynSetLogLevel(args[0].ival);
}
// - LlrfAmcAsynSetLogLevel //

void drvLLRFAMCASYNRegister(void)
{
    iocshRegister( &LlrfAmcAsynConfigFuncDef,       LlrfAmcAsynConfigCallFunc       );
    iocshRegister( &LlrfAmcAsynSetLogLevelFuncDef,  LlrfAmcAsynSetLogLevelCallFunc  );
}

extern "C" {
    epicsExportRegistrar(drvLLRFAMCASYNRegister);
}
