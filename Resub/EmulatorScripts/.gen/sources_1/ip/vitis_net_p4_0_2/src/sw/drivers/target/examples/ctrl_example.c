/*
-- (c) Copyright 2020 Xilinx, Inc. All rights reserved.
--
-- This file contains confidential and proprietary information
-- of Xilinx, Inc. and is protected under U.S. and
-- international copyright and other intellectual property
-- laws.
--
-- DISCLAIMER
-- This disclaimer is not a license and does not grant any
-- rights to the materials distributed herewith. Except as
-- otherwise provided in a valid license issued to you by
-- Xilinx, and to the maximum extent permitted by applicable
-- law: (1) THESE MATERIALS ARE MADE AVAILABLE "AS IS" AND
-- WITH ALL FAULTS, AND XILINX HEREBY DISCLAIMS ALL WARRANTIES
-- AND CONDITIONS, EXPRESS, IMPLIED, OR STATUTORY, INCLUDING
-- BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, NON-
-- INFRINGEMENT, OR FITNESS FOR ANY PARTICULAR PURPOSE; and
-- (2) Xilinx shall not be liable (whether in contract or tort,
-- including negligence, or under any other theory of
-- liability) for any loss or damage of any kind or nature
-- related to, arising under or in connection with these
-- materials, including for any direct, or any indirect,
-- special, incidental, or consequential loss or damage
-- (including loss of data, profits, goodwill, or any type of
-- loss or damage suffered as a result of any action brought
-- by a third party) even if such damage or loss was
-- reasonably foreseeable or Xilinx had been advised of the
-- possibility of the same.
--
-- CRITICAL APPLICATIONS
-- Xilinx products are not designed or intended to be fail-
-- safe, or for use in any application requiring fail-safe
-- performance, such as life-support or safety devices or
-- systems, Class III medical devices, nuclear facilities,
-- applications related to the deployment of airbags, or any
-- other applications that could lead to death, personal
-- injury, or severe property or environmental damage
-- (individually and collectively, "Critical
-- Applications"). Customer assumes the sole risk and
-- liability of any use of Xilinx products in Critical
-- Applications, subject only to applicable laws and
-- regulations governing limitations on product liability.
--
-- THIS COPYRIGHT NOTICE AND DISCLAIMER MUST BE RETAINED AS
-- PART OF THIS FILE AT ALL TIMES.
--------------------------------------------------------------------------------
--
-- Vendor         : Xilinx
-- Revision       : $Revision: #1 $
-- Date           : $DateTime: 2021/03/10 09:21:01 $
-- Last Author    : $Author: jaimeh $
--
--------------------------------------------------------------------------------
-- Description : An example C program  example design that demonstrates how to
--               setup and control the target control driver.
--
--------------------------------------------------------------------------------
*/

/*
 * Target driver sample for the VITISNETP4 that
 * illustrates how some basic control table operations that can be achieved using the
 * target driver.
 *
 * To port this example to a user platform see instructions in the VitisNetP4 User
 * Guide.
 */

/****************************************************************************************************************************************************/
/* SECTION: Header includes */
/****************************************************************************************************************************************************/

/*
 * The example designs include file should be present in the target/inc directory
 * NOTE: This file that gives access to the generated configuration file
 */

#include <stdio.h>
#include <stdlib.h>

#include "vitisnetp4_target.h"
#include "chip_dc_250_09_vitisnetp4_0_0_defs.c"


/****************************************************************************************************************************************************/
/* SECTION: Constants/macros */
/****************************************************************************************************************************************************/

#define DISPLAY_ERROR(ErrorCode)  printf("Error Code is value %s", XilVitisNetP4ReturnTypeToString(ErrorCode))

/****************************************************************************************************************************************************/
/* SECTION: Type definitions */
/****************************************************************************************************************************************************/

/****************************************************************************************************************************************************/
/* SECTION: Local function declarations */
/****************************************************************************************************************************************************/
static void DisplayVitisNetP4Versions(XilVitisNetP4TargetCtx *CtxPtr);

/****************************************************************************************************************************************************/
/* SECTION: Global function definitions */
/****************************************************************************************************************************************************/

int main(void)
{
    XilVitisNetP4EnvIf EnvIf;
    XilVitisNetP4TargetCtx FiveTupleTargetCtx;
    uint32_t NumElements;

    XilVitisNetP4ReturnType Result;
    //XilVitisNetP4TargetInterruptCtx *InterruptCtxPtr = NULL;
    XilVitisNetP4TargetCtrlCtx* CtrlCtxPtr = NULL;
    XilVitisNetP4TargetCtrlEngineId EngineId;
    uint32_t PacketRateMargin;
    uint32_t ComponentIndex;
    XilVitisNetP4TargetInterruptEccErrorType EccErrorType;
    uint32_t P4ElementId;

    Result = XilVitisNetP4StubEnvIf(&EnvIf);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        DISPLAY_ERROR(Result);
        goto exit_example;
    }

    printf("Initialise the Ctrl Driver\n\r");
    Result = XilVitisNetP4TargetInit(&FiveTupleTargetCtx, &EnvIf, &(XilVitisNetP4TargetConfig_chip_dc_250_09_vitisnetp4_0_0));
    if (Result == XIL_VITIS_NET_P4_TARGET_ERR_INCOMPATIBLE_SW_HW)
    {
        /* DisplayVitisNetP4IP and Software versions to determine the differences */
        DisplayVitisNetP4Versions(&FiveTupleTargetCtx);
        goto exit_example;
    }
    else if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        DISPLAY_ERROR(Result);
        goto exit_example;
    }

    printf("GetVitisNetP4Instance Interrupt Driver\n\r");
    Result = XilVitisNetP4TargetGetCtrlDrv(&FiveTupleTargetCtx, &CtrlCtxPtr);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        DISPLAY_ERROR(Result);
        goto target_exit;
    }

    printf("Get number of p4 elements\n\r");
    Result = XilVitisNetP4TargetCtrlGetP4ElementCount(CtrlCtxPtr, &NumElements);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        DISPLAY_ERROR(Result);
        goto target_exit;
    }

    printf("Reset all engines\n\r");
    EngineId = XIL_VITIS_NET_P4_TARGET_CTRL_ALL_ENGINES;
    Result = XilVitisNetP4TargetCtrlSoftResetEngine(CtrlCtxPtr, EngineId);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        DISPLAY_ERROR(Result);
        goto target_exit;
    }


    printf("Set packet rate limit margin\n\r");
    PacketRateMargin = 100;
    Result = XilVitisNetP4TargetCtrlSetPacketRateLimitMargin(CtrlCtxPtr, PacketRateMargin);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        DISPLAY_ERROR(Result);
        goto target_exit;
    }

    printf("Enable/Disable inject Component ECC error\n\r");
    ComponentIndex = 1;
    EccErrorType = XIL_VITIS_NET_P4_INTERRUPT_ECC_ERROR_SINGLE_BIT;
    Result = XilVitisNetP4TargetCtrlIpComponentEnableInjectEccError(CtrlCtxPtr, ComponentIndex, EccErrorType);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        DISPLAY_ERROR(Result);
        goto target_exit;
    }
    
    Result = XilVitisNetP4TargetCtrlIpComponentDisableInjectEccError(CtrlCtxPtr, ComponentIndex, EccErrorType);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        DISPLAY_ERROR(Result);
        goto target_exit;
    }

    printf("Inject/Disable inject P4 element ECC error\n\r");
    P4ElementId = 1;
    EccErrorType = XIL_VITIS_NET_P4_INTERRUPT_ECC_ERROR_SINGLE_BIT;
    Result = XilVitisNetP4TargetCtrlP4ElementEnableInjectEccError(CtrlCtxPtr, P4ElementId, EccErrorType);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        DISPLAY_ERROR(Result);
        goto target_exit;
    }

    Result = XilVitisNetP4TargetCtrlP4ElementDisableInjectEccError(CtrlCtxPtr, P4ElementId, EccErrorType);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        DISPLAY_ERROR(Result);
        goto target_exit;
    }

target_exit:
    Result = XilVitisNetP4TargetExit(&FiveTupleTargetCtx);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        DISPLAY_ERROR(Result);
    }

exit_example:
    return Result;
}
/****************************************************************************************************************************************************/
/* SECTION: Local function definitions */
/****************************************************************************************************************************************************/

static void DisplayVitisNetP4Versions(XilVitisNetP4TargetCtx *CtxPtr)
{
    XilVitisNetP4ReturnType Result;
    XilVitisNetP4Version SwVersion;
    XilVitisNetP4Version IpVersion;
    XilVitisNetP4TargetBuildInfoCtx *BuildInfoCtxPtr;


    Result =  XilVitisNetP4TargetGetSwVersion(CtxPtr, &SwVersion);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return;
    }

    /* The BuildInfo Driver provides access to the IP Version if present */
    Result = XilVitisNetP4TargetGetBuildInfoDrv(CtxPtr, &BuildInfoCtxPtr);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return;
    }

    Result = XilVitisNetP4TargetBuildInfoGetIpVersion(BuildInfoCtxPtr, &IpVersion);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return;
    }

    printf("----VitisNetP4Runtime Software Version\n");
    printf("\t\t Major = %d\n", SwVersion.Major);
    printf("\t\t Minor = %d\n", SwVersion.Minor);
    printf("\n");

    printf("----VitisNetP4IP Version\n");
    printf("\t\t Major = %d\n", IpVersion.Major);
    printf("\t\t Minor = %d\n", IpVersion.Minor);
}

