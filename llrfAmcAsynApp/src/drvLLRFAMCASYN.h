/**
 *-----------------------------------------------------------------------------
 * Title      : LLRF AMC Driver EPICS Module
 * ----------------------------------------------------------------------------
 * File       : drvLLRFAMCASYN.h
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

#include <stdio.h>
#include <string.h>
#include <asynPortDriver.h>
#include <cpsw_api_builder.h>
#include <cpsw_api_user.h>
#include <yaml-cpp/yaml.h>
#include <LlrfAmc.h>

class LLRFAMCASYN : public asynPortDriver {
    public:
        // Constructor
        LLRFAMCASYN(const std::string& pn);

        // Methods that we override from asynPortDriver
        virtual asynStatus writeUInt32Digital(asynUser *pasynUser, epicsUInt32 value, epicsUInt32 mask);

    private:
        std::string driverName; // This driver name
        std::string portName;   // Port name (passed from st.cmd)

        // llrfAmc object
        LlrfAmc llrfAmc;

        // Parameter names
        const std::string paramInitName;        // INIT
        const std::string paraminitStatName;    // INIT_STAT

        // Parameter indexes
        int paramInitIndex;                     // INIT
        int paramInitStatIndex;                 // INIT_STAT

        // Parameter masks
        const uint32_t paramInitMask;           // INIT
        const uint32_t paramInitStatMask;       // INIT_STAT

        // Values for the INIT_STAT parameter
        enum paramInitStatStates
        {
            INIT_STAT_FAILED = 0,
            INIT_STAT_SUCCEED,
            INIT_STAT_INPROGRESS
        };
};
