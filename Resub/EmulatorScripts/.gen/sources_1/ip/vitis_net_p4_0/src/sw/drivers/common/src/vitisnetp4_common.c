/*
-- (c) Copyright 2018 Xilinx, Inc. All rights reserved.
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
-- Description : This file implements some utility functions for working with
-- the device access interface used by Xilinx VitisNetP4extern SW drivers.
--
--------------------------------------------------------------------------------
*/

/****************************************************************************************************************************************************/
/* SECTION: Header includes */
/****************************************************************************************************************************************************/

#include "vitisnetp4_common.h"
#include <stdarg.h>
#include <stdio.h>

/****************************************************************************************************************************************************/
/* SECTION: Local function prototypes */
/****************************************************************************************************************************************************/

XilVitisNetP4ReturnType XilVitisNetP4WordWrite32Stub(XilVitisNetP4EnvIf *EnvIfPtr, XilVitisNetP4AddressType Address, uint32_t WriteValue);
XilVitisNetP4ReturnType XilVitisNetP4WordRead32Stub(XilVitisNetP4EnvIf *EnvIfPtr, XilVitisNetP4AddressType Address, uint32_t *ReadValuePtr);
XilVitisNetP4ReturnType XilVitisNetP4WordLogStub(XilVitisNetP4EnvIf *EnvIfPtr, const char *MessagePtr);

/****************************************************************************************************************************************************/
/* SECTION: Private data definitions */
/****************************************************************************************************************************************************/

/* Error code definition print messages for Xilinx VitisNetP4 library */
/* Changes to XilVitisNet P4ReturnType definition must be reflected here */
static const char * XilVitisNetP4ReturnTypeStrings[XIL_VITIS_NET_P4_NUM_RETURN_CODES] =
{
    /* General/common return codes */
    "XIL_VITIS_NET_P4_SUCCESS",
    "XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM",
    "XIL_VITIS_NET_P4_GENERAL_ERR_INVALID_CONTEXT",
    "XIL_VITIS_NET_P4_GENERAL_ERR_INVALID_ENVIRONMENT_INTERFACE",
    "XIL_VITIS_NET_P4_GENERAL_ERR_INTERNAL_ASSERTION",

    /* Build Info driver return codes */
    "XIL_VITIS_NET_P4_BUILD_INFO_ERR_MALLOC_FAILED",

    /* Control registers return codes */
    "XIL_VITIS_NET_P4_CONTROL_ERR_INVALID_ENGINE_ID_ERROR_TYPE",
    "XIL_VITIS_NET_P4_CONTROL_ERR_INVALID_PR2CLK_RATIO_ERROR_TYPE",
    "XIL_VITIS_NET_P4_CONTROL_ERR_INVALID_PRM_VALUE_ERROR_TYPE",

    /* Interrupt driver return codes */
    "XIL_VITIS_NET_P4_INTERRUPT_ERR_MALLOC_FAILED",
    "XIL_VITIS_NET_P4_INTERRUPT_ERR_CONFIG_MAX_NUM_ELEMENTS_EXCEED",
    "XIL_VITIS_NET_P4_INTERRUPT_ERR_CONFIG_MAX_NUM_COMPONENTS_EXCEED",
    "XIL_VITIS_NET_P4_INTERRUPT_ERR_CONFIG_INVALID_COMPONENT_NAME",
    "XIL_VITIS_NET_P4_INTERRUPT_ERR_COMPONENT_NOT_FOUND",
    "XIL_VITIS_NET_P4_INTERRUPT_ERR_NAME_BUFFER_TOO_SMALL",
    "XIL_VITIS_NET_P4_INTERRUPT_ERR_INVALID_ECC_ERROR_TYPE",
    "XIL_VITIS_NET_P4_INTERRUPT_ERR_INVALID_ELEMENT_ID",
    "XIL_VITIS_NET_P4_INTERRUPT_ERR_INVALID_COMPONENT_INDEX",

    /* Random extern driver return codes */
    "XIL_VITIS_NET_P4_RANDOM_ERR_INVALID_MODE",
    "XIL_VITIS_NET_P4_RANDOM_ERR_ARRAY_INVALID_SIZE",
    "XIL_VITIS_NET_P4_RANDOM_ERR_INVALID_SEED",

    /* Register extern driver return codes */
    "XIL_VITIS_NET_P4_REGISTER_ERR_INVALID_DATA_SIZE",
    "XIL_VITIS_NET_P4_REGISTER_ERR_INVALID_NUM_REGS",
    "XIL_VITIS_NET_P4_REGISTER_ERR_INVALID_NUM_BITS",
    "XIL_VITIS_NET_P4_REGISTER_ERR_BUFFER_SIZE_MISMATCH",
    "XIL_VITIS_NET_P4_REGISTER_ERR_INVALID_INDEX",

    /* Counter extern driver return codes */
    "XIL_VITIS_NET_P4_COUNTER_ERR_CONFIG_INVALID_MODE",
    "XIL_VITIS_NET_P4_COUNTER_ERR_CONFIG_UNSUPPORTED_OPERATION",
    "XIL_VITIS_NET_P4_COUNTER_ERR_CONFIG_INVALID_NUM_COUNTERS",
    "XIL_VITIS_NET_P4_COUNTER_ERR_INVALID_INDEX",
    "XIL_VITIS_NET_P4_COUNTER_ERR_INVALID_PACKET_COUNT",
    "XIL_VITIS_NET_P4_COUNTER_ERR_INVALID_BYTE_COUNT",

    /* Meter extern driver return codes */
    "XIL_VITIS_NET_P4_METER_ERR_CONFIG_INVALID_MODE",
    "XIL_VITIS_NET_P4_METER_ERR_CONFIG_UNSUPPORTED_OPERATION",
    "XIL_VITIS_NET_P4_METER_ERR_CONFIG_INVALID_NUM_METERS",
    "XIL_VITIS_NET_P4_METER_ERR_CONFIG_INVALID_CLOCK_FREQ",
    "XIL_VITIS_NET_P4_METER_ERR_INVALID_INDEX",
    "XIL_VITIS_NET_P4_METER_ERR_INVALID_PROFILE_COLOUR_AWARE",
    "XIL_VITIS_NET_P4_METER_ERR_INVALID_PROFILE_DATA_RATE",
    "XIL_VITIS_NET_P4_METER_ERR_INVALID_PROFILE_BURST_SIZE",
    "XIL_VITIS_NET_P4_METER_ERR_INVALID_NUM_BITS",

    /* Tiny TCAM/BCAM drivers return codes */
    "XIL_VITIS_NET_P4_TINY_CAM_ERR_INVALID_MODE",

    /* CAM driver return codes */
    "XIL_VITIS_NET_P4_CAM_ERR_INVALID_NUM_ENTRIES",
    "XIL_VITIS_NET_P4_CAM_ERR_LOOKUP_NOT_FOUND",
    "XIL_VITIS_NET_P4_CAM_ERR_INVALID_KEY",
    "XIL_VITIS_NET_P4_CAM_ERR_INVALID_ENDIAN",
    "XIL_VITIS_NET_P4_CAM_ERR_FULL",
    "XIL_VITIS_NET_P4_CAM_ERR_NO_OPEN",
    "XIL_VITIS_NET_P4_CAM_ERR_INVALID_ARG",
    "XIL_VITIS_NET_P4_CAM_ERR_WRONG_KEY_WIDTH",
    "XIL_VITIS_NET_P4_CAM_ERR_TOO_MANY_INSTANCES",
    "XIL_VITIS_NET_P4_CAM_ERR_WRONG_BIT_FIELD_MASK",
    "XIL_VITIS_NET_P4_CAM_ERR_WRONG_CONST_FIELD_MASK",
    "XIL_VITIS_NET_P4_CAM_ERR_WRONG_UNUSED_FIELD_MASK",
    "XIL_VITIS_NET_P4_CAM_ERR_INVALID_TERNARY_FIELD_LEN",
    "XIL_VITIS_NET_P4_CAM_ERR_WRONG_PRIO_WIDTH",
    "XIL_VITIS_NET_P4_CAM_ERR_WRONG_MAX",
    "XIL_VITIS_NET_P4_CAM_ERR_DUPLICATE_FOUND",
    "XIL_VITIS_NET_P4_CAM_ERR_WRONG_PREFIX",
    "XIL_VITIS_NET_P4_CAM_ERR_WRONG_PREFIX_MASK",
    "XIL_VITIS_NET_P4_CAM_ERR_WRONG_RANGE",
    "XIL_VITIS_NET_P4_CAM_ERR_WRONG_RANGE_MASK",
    "XIL_VITIS_NET_P4_CAM_ERR_KEY_NOT_FOUND",
    "XIL_VITIS_NET_P4_CAM_ERR_WRONG_MIN",
    "XIL_VITIS_NET_P4_CAM_ERR_WRONG_PRIO",
    "XIL_VITIS_NET_P4_CAM_ERR_WRONG_LIST_LENGTH",
    "XIL_VITIS_NET_P4_CAM_ERR_WRONG_NUMBER_OF_SLOTS",
    "XIL_VITIS_NET_P4_CAM_ERR_INVALID_MEM_TYPE",
    "XIL_VITIS_NET_P4_CAM_ERR_TOO_HIGH_FREQUENCY",
    "XIL_VITIS_NET_P4_CAM_ERR_WRONG_TERNARY_MASK",
    "XIL_VITIS_NET_P4_CAM_ERR_MASKED_KEY_BIT_IS_SET",
    "XIL_VITIS_NET_P4_CAM_ERR_INVALID_MODE",
    "XIL_VITIS_NET_P4_CAM_ERR_WRONG_RESPONSE_WIDTH",
    "XIL_VITIS_NET_P4_CAM_ERR_FORMAT_SYNTAX",
    "XIL_VITIS_NET_P4_CAM_ERR_TOO_MANY_FIELDS",
    "XIL_VITIS_NET_P4_CAM_ERR_TOO_MANY_RANGES",
    "XIL_VITIS_NET_P4_CAM_ERR_INVALID_RANGE_LEN",
    "XIL_VITIS_NET_P4_CAM_ERR_INVALID_RANGE_START",
    "XIL_VITIS_NET_P4_CAM_ERR_INVALID_PREFIX_LEN",
    "XIL_VITIS_NET_P4_CAM_ERR_INVALID_PREFIX_START",
    "XIL_VITIS_NET_P4_CAM_ERR_INVALID_PREFIX_KEY",
    "XIL_VITIS_NET_P4_CAM_ERR_INVALID_BIT_FIELD_LEN",
    "XIL_VITIS_NET_P4_CAM_ERR_INVALID_BIT_FIELD_START",
    "XIL_VITIS_NET_P4_CAM_ERR_INVALID_CONST_FIELD_LEN",
    "XIL_VITIS_NET_P4_CAM_ERR_INVALID_CONST_FIELD_START",
    "XIL_VITIS_NET_P4_CAM_ERR_INVALID_UNUSED_FIELD_LEN",
    "XIL_VITIS_NET_P4_CAM_ERR_INVALID_UNUSED_FIELD_START",
    "XIL_VITIS_NET_P4_CAM_ERR_MAX_KEY_LEN_EXCEED",
    "XIL_VITIS_NET_P4_CAM_ERR_INVALID_PRIO_AND_INDEX_WIDTH",
    "XIL_VITIS_NET_P4_CAM_ERR_TOO_MANY_UNITS",
    "XIL_VITIS_NET_P4_CAM_ERR_NO_MASK",
    "XIL_VITIS_NET_P4_CAM_ERR_INVALID_MEMORY_WIDTH",
    "XIL_VITIS_NET_P4_CAM_ERR_UNSUPPORTED_COMMAND",
    "XIL_VITIS_NET_P4_CAM_ERR_ENVIRONMENT",
    "XIL_VITIS_NET_P4_CAM_ERR_UNSUPPORTED_CAM_TYPE",
    "XIL_VITIS_NET_P4_CAM_ERR_NULL_POINTER",
    "XIL_VITIS_NET_P4_CAM_ERR_TOO_MANY_PCS",
    "XIL_VITIS_NET_P4_CAM_ERR_CONFIGURATION",
    "XIL_VITIS_NET_P4_CAM_ERR_ENVIRONMENT_FSMBUSY",
    "XIL_VITIS_NET_P4_CAM_ERR_ENVIRONMENT_POLLED_OUT",
    "XIL_VITIS_NET_P4_CAM_ERR_SEGMENT_COUNT_ZERO",
    "XIL_VITIS_NET_P4_CAM_ERR_VC_UNSUPPORTED_COMMAND",
    "XIL_VITIS_NET_P4_CAM_ERR_WRONG_VIRTUAL_CAM_ID",
    "XIL_VITIS_NET_P4_CAM_ERR_WRONG_SEGMENT_NUMBER",
    "XIL_VITIS_NET_P4_CAM_ERR_TOO_MANY_RAMS",
    "XIL_VITIS_NET_P4_CAM_ERR_TOO_SMALL_HEAP",
    "XIL_VITIS_NET_P4_CAM_ERR_RAM_TOO_SMALL",
    "XIL_VITIS_NET_P4_CAM_ERR_NOMEM",
    "XIL_VITIS_NET_P4_CAM_ERR_MALLOC_FAILED",
    "XIL_VITIS_NET_P4_CAM_ERR_OPTIMIZATION_TYPE_UNKNOWN",
    "XIL_VITIS_NET_P4_CAM_ERR_UNKNOWN",

    /* Table driver return codes */
    "XIL_VITIS_NET_P4_TABLE_ERR_INVALID_TABLE_HANDLE_DRV",
    "XIL_VITIS_NET_P4_TABLE_ERR_INVALID_TABLE_MODE",
    "XIL_VITIS_NET_P4_TABLE_ERR_INVALID_ACTION_ID",
    "XIL_VITIS_NET_P4_TABLE_ERR_PARAM_NOT_REQUIRED",
    "XIL_VITIS_NET_P4_TABLE_ERR_ACTION_NOT_FOUND",
    "XIL_VITIS_NET_P4_TABLE_ERR_FUNCTION_NOT_SUPPORTED",
    "XIL_VITIS_NET_P4_TABLE_ERR_MALLOC_FAILED",
    "XIL_VITIS_NET_P4_TABLE_ERR_NAME_BUFFER_TOO_SMALL",

    /* Target driver return codes */
    "XIL_VITIS_NET_P4_TARGET_ERR_INCOMPATIBLE_SW_HW",
    "XIL_VITIS_NET_P4_TARGET_ERR_MALLOC_FAILED",
    "XIL_VITIS_NET_P4_TARGET_ERR_TABLE_NOT_FOUND",
    "XIL_VITIS_NET_P4_TARGET_ERR_NAME_BUFFER_TOO_SMALL",
    "XIL_VITIS_NET_P4_TINY_CAM_ENTRY_OCCUPIED"
};

/****************************************************************************************************************************************************/
/* SECTION: Global function definitions */
/****************************************************************************************************************************************************/

const char *XilVitisNetP4ReturnTypeToString(XilVitisNetP4ReturnType Value)
{
    if ((uint32_t)Value < XIL_VITIS_NET_P4_NUM_RETURN_CODES)
    {
        return XilVitisNetP4ReturnTypeStrings[Value];
    }

    return "XIL_VITIS_NET_P4_UNKNOWN_RETURN_CODE";
}

XilVitisNetP4ReturnType XilVitisNetP4StubEnvIf(XilVitisNetP4EnvIf *EnvIfPtr)
{
    /* Initial parameter validation */
    if (EnvIfPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    /* Platform-specific contexts can legitimately be set to NULL if the corresponding functions don't need a context, hence the statement below */
    EnvIfPtr->UserCtx = NULL;

    /* Install stubs for each platform-specific function */
    EnvIfPtr->WordWrite32 = XilVitisNetP4WordWrite32Stub;
    EnvIfPtr->WordRead32 = XilVitisNetP4WordRead32Stub;
    EnvIfPtr->LogError = XilVitisNetP4WordLogStub;
    EnvIfPtr->LogInfo = XilVitisNetP4WordLogStub;

    return XIL_VITIS_NET_P4_SUCCESS;
}

XilVitisNetP4ReturnType XilVitisNetP4ValidateEnvIf(const XilVitisNetP4EnvIf *EnvIfPtr)
{
    /* Initial parameter validation */
    if (EnvIfPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_INVALID_ENVIRONMENT_INTERFACE;
    }

    /* Validate that each platform-specific function points somewhere - the context structure is omitted since NULL is potentially valid for it */
    if (EnvIfPtr->WordWrite32 == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_INVALID_ENVIRONMENT_INTERFACE;
    }

    if (EnvIfPtr->WordRead32 == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_INVALID_ENVIRONMENT_INTERFACE;
    }

    if (EnvIfPtr->LogError == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_INVALID_ENVIRONMENT_INTERFACE;
    }

    if (EnvIfPtr->LogInfo == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_INVALID_ENVIRONMENT_INTERFACE;
    }

    /* Checking EnvIfPtr->UserCtx against NULL is deliberately omitted since NULL may/may not be valid */

    return XIL_VITIS_NET_P4_SUCCESS;
}

XilVitisNetP4ReturnType XilVitisNetP4LogError(XilVitisNetP4EnvIf *EnvIfPtr, const char *FormatStringPtr, ...)
{
    va_list VaArgs;
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    char Buf[XIL_VITIS_NET_P4_LOG_BUF_SIZE_BYTES] = "";
    int MessageLength = 0;

    /* Validate received parameters */
    if ((EnvIfPtr == NULL) || (FormatStringPtr == NULL))
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    /* Construct the message string from the provided argument list */
    va_start(VaArgs, FormatStringPtr);
    MessageLength = vsnprintf(Buf, sizeof(Buf), FormatStringPtr, VaArgs);
    va_end(VaArgs);

    /* Emit the message string if it was constructed successfully */
    if ((MessageLength >= 0) && (size_t)MessageLength < sizeof(Buf))
    {
        EnvIfPtr->LogError(EnvIfPtr, Buf);
    }
    else
    {
        Result = XIL_VITIS_NET_P4_GENERAL_ERR_INTERNAL_ASSERTION;
    }

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4LogInfo(XilVitisNetP4EnvIf *EnvIfPtr, const char *FormatStringPtr, ...)
{
    va_list VaArgs;
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    char Buf[XIL_VITIS_NET_P4_LOG_BUF_SIZE_BYTES] = "";
    int MessageLength = 0;

    /* Validate received parameters */
    if ((EnvIfPtr == NULL) || (FormatStringPtr == NULL))
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    /* Construct the message string from the provided argument list */
    va_start(VaArgs, FormatStringPtr);
    MessageLength = vsnprintf(Buf, sizeof(Buf), FormatStringPtr, VaArgs);
    va_end(VaArgs);

    /* Emit the message string if it was constructed successfully */
    if ((MessageLength >= 0) && (size_t)MessageLength < sizeof(Buf))
    {
        EnvIfPtr->LogInfo(EnvIfPtr, Buf);
    }
    else
    {
        Result = XIL_VITIS_NET_P4_GENERAL_ERR_INTERNAL_ASSERTION;
    }

    return Result;
}

/****************************************************************************************************************************************************/
/* SECTION: Local function definitions */
/****************************************************************************************************************************************************/

XilVitisNetP4ReturnType XilVitisNetP4WordWrite32Stub(XilVitisNetP4EnvIf *EnvIfPtr, XilVitisNetP4AddressType Address, uint32_t WriteValue)
{
    if (EnvIfPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    /* Unused parameters */
    (void)Address;
    (void)WriteValue;

    return XIL_VITIS_NET_P4_SUCCESS;
}

XilVitisNetP4ReturnType XilVitisNetP4WordRead32Stub(XilVitisNetP4EnvIf *EnvIfPtr, XilVitisNetP4AddressType Address, uint32_t *ReadValuePtr)
{
    if (EnvIfPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    if (ReadValuePtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    /* Unused parameter */
    (void)Address;

    return XIL_VITIS_NET_P4_SUCCESS;
}

XilVitisNetP4ReturnType XilVitisNetP4WordLogStub(XilVitisNetP4EnvIf *EnvIfPtr, const char *MessagePtr)
{
    if (EnvIfPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    if (MessagePtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    return XIL_VITIS_NET_P4_SUCCESS;
}
