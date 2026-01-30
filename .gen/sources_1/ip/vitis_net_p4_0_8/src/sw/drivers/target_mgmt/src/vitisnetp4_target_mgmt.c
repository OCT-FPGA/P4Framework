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
-- Revision       : $Revision: #2 $
-- Date           : $DateTime: 2021/03/24 09:23:48 $
-- Last Author    : $Author: pfanning $
--
--------------------------------------------------------------------------------
-- Description : Implementation of the VitisNetP4target management drivers
--
--
--------------------------------------------------------------------------------
*/
/****************************************************************************************************************************************************/
/* SECTION: Header includes */
/****************************************************************************************************************************************************/
#include "vitisnetp4_target_mgmt.h"
#include "vitisnetp4_common.h"
#include <stdio.h>
#include <string.h>

/****************************************************************************************************************************************************/
/* SECTION: Constants/macros */
/****************************************************************************************************************************************************/

/* Instance information driver register map */
/* IpVersion Register */
#define XIL_VITIS_NET_P4_BUILD_INFO_VERSION_REG (0x00)
#define XIL_VITIS_NET_P4_BUILD_INFO_VERSION_MAJOR_MASK (0x000000FF)
#define XIL_VITIS_NET_P4_BUILD_INFO_VERSION_MAJOR_SHIFT (0)
#define XIL_VITIS_NET_P4_BUILD_INFO_VERSION_MINOR_MASK (0x0000FF00)
#define XIL_VITIS_NET_P4_BUILD_INFO_VERSION_MINOR_SHIFT (8)
#define XIL_VITIS_NET_P4_BUILD_INFO_VERSION_REVISION_MASK (0x00FF0000)
#define XIL_VITIS_NET_P4_BUILD_INFO_VERSION_REVISON_SHIFT (16)

/* Parameters(Configuration) Register */
#define XIL_VITIS_NET_P4_BUILD_INFO_SETTINGS_REG (0x04)
#define XIL_VITIS_NET_P4_BUILD_INFO_SETTINGS_PKT_RATE_MASK (0x0000003FF)
#define XIL_VITIS_NET_P4_BUILD_INFO_SETTINGS_PKT_RATE_SHIFT (0)
#define XIL_VITIS_NET_P4_BUILD_INFO_SETTINGS_CAM_MEM_CLK_MASK (0x000FFC00)
#define XIL_VITIS_NET_P4_BUILD_INFO_SETTINGS_CAM_MEM_CLK_SHIFT (10)
#define XIL_VITIS_NET_P4_BUILD_INFO_SETTINGS_AXIS_CLK_MASK (0x3FF00000)
#define XIL_VITIS_NET_P4_BUILD_INFO_SETTINGS_AXIS_CLK_SHIFT (20)

/* Target driver register map */
#define XIL_VITIS_NET_P4_INTERRUPT_FIFO_STATUS_REG (0x00)
#define XIL_VITIS_NET_P4_INTERRUPT_ELEMENT_STATUS_REG (0x04)

#define XIL_VITIS_NET_P4_INTERRUPT_FIFO_ENABLE_REG (XIL_VITIS_NET_P4_INTERRUPT_FIFO_STATUS_REG + XIL_VITIS_NET_P4_TARGET_MGMT_REGISTER_STRIDE)
#define XIL_VITIS_NET_P4_INTERRUPT_ELEMENT_ENABLE_REG (XIL_VITIS_NET_P4_INTERRUPT_ELEMENT_STATUS_REG + XIL_VITIS_NET_P4_TARGET_MGMT_REGISTER_STRIDE)

#define XIL_VITIS_NET_P4_INTERRUPT_FIFO_CLEAR_REG (XIL_VITIS_NET_P4_INTERRUPT_FIFO_ENABLE_REG + XIL_VITIS_NET_P4_TARGET_MGMT_REGISTER_STRIDE)
#define XIL_VITIS_NET_P4_INTERRUPT_ELEMENT_CLEAR_REG (XIL_VITIS_NET_P4_INTERRUPT_ELEMENT_ENABLE_REG + XIL_VITIS_NET_P4_TARGET_MGMT_REGISTER_STRIDE)

/* Target driver constants */
#define XIL_VITIS_NET_P4_TARGET_MGMT_MAX_NUM_ELEMENTS (128)
#define XIL_VITIS_NET_P4_TARGET_MGMT_MAX_NUM_FIFOS (16)

#define XIL_VITIS_NET_P4_TARGET_MGMT_NUM_ECC_INTR_TYPES_PER_OBJECT (2)
#define XIL_VITIS_NET_P4_TARGET_MGMT_MAX_NUM_ECC_INTERRUPTS \
    ((XIL_VITIS_NET_P4_TARGET_MGMT_MAX_NUM_ELEMENTS + XIL_VITIS_NET_P4_TARGET_MGMT_MAX_NUM_FIFOS) * XIL_VITIS_NET_P4_TARGET_MGMT_NUM_ECC_INTR_TYPES_PER_OBJECT)
/* This calculation OK as long as MAX_NUM_ECC_INTERRUPTS  is a multiple of 32 */
#define XIL_VITIS_NET_P4_TARGET_MGMT_REGISTER_STRIDE (XIL_VITIS_NET_P4_TARGET_MGMT_MAX_NUM_ECC_INTERRUPTS / XIL_VITIS_NET_P4_BITS_PER_BYTE)

/* Parameters(Configuration) Register */
#define XIL_VITIS_NET_P4_CONTROL_BASE_REG (0x0E8) /* Base register offset for the control IP*/
#define XIL_VITIS_NET_P4_CONTROL_IP_IEBE_REG (0x0E8 - XIL_VITIS_NET_P4_CONTROL_BASE_REG)  /*IP Specific Element Inject single / double bit ECC error */
#define XIL_VITIS_NET_P4_CONTROL_P4_IEBE_REG (0x0EC - XIL_VITIS_NET_P4_CONTROL_BASE_REG)  /*P4 Element Inject single / double bit ECC error */

/* PRLM Reset Engine */
#define XIL_VITIS_NET_P4_CONTROL_PRLM_REG  (0x10C - XIL_VITIS_NET_P4_CONTROL_BASE_REG)	/* Packet rate limiter margin */
#define XIL_VITIS_NET_P4_CONTROL_PRLM_MASK  (0xFFFF)
#define XIL_VITIS_NET_P4_TARGET_CTRL_MAX_SOF_PER_INTV (0x3E8) /* R4-17	PRIW shall have a value of 1000, which represents the interval window in clock cycles */

/*R4-15	PRLM shall support additional PR drops from 0.1% to 10%.*/
#define XIL_VITIS_NET_P4_CONTROL_PRLM_MAX_VALUE  (1000) /* equates to 10%*/
#define XIL_VITIS_NET_P4_CONTROL_PRLM_MIN_VALUE  (10) /* equates to 0.1%*/

/* Soft Reset Engine */
#define XIL_VITIS_NET_P4_CONTROL_SRE_REG  (0x110 - XIL_VITIS_NET_P4_CONTROL_BASE_REG)	/*	SRE	Soft Reset Engine */
#define XIL_VITIS_NET_P4_TARGET_CTRL_SRE_ALL_ENGINES (0x0010)
#define XIL_VITIS_NET_P4_TARGET_CTRL_SRE_DEPARSER_ENGINE (0x0008)
#define XIL_VITIS_NET_P4_TARGET_CTRL_SRE_COMPONENT_ENGINE (0x0004)
#define XIL_VITIS_NET_P4_TARGET_CTRL_SRE_MA_ENGINE (0x0002)
#define XIL_VITIS_NET_P4_TARGET_CTRL_SRE_PARSER_ENGINE (0x0001)

#define XIL_VITIS_NET_P4_MAX_PR2CLK_RATIO (25) /* R4-9	The maximum supported PR2CLK ratio shall be 25. */


/****************************************************************************************************************************************************/
/* SECTION: Type definitions */
/****************************************************************************************************************************************************/
struct XilVitisNetP4TargetBuildInfoPrivateCtx
{
    XilVitisNetP4EnvIf EnvIf;
    XilVitisNetP4AddressType BaseAddr;
};

typedef struct XilVitisNetP4TargetInterruptComponentName
{
    char *NameStringPtr;
    uint32_t NameSizeBytes;
} XilVitisNetP4TargetInterruptComponentName;

struct XilVitisNetP4TargetInterruptPrivateCtx
{
    XilVitisNetP4EnvIf EnvIf;
    XilVitisNetP4AddressType BaseAddr;
    uint32_t NumP4Elements;
    uint32_t NumComponents;
    XilVitisNetP4TargetInterruptComponentName *ComponentNameListPtr;
};

struct XilVitisNetP4TargetCtrlPrivateCtx
{
    XilVitisNetP4EnvIf EnvIf;
    XilVitisNetP4AddressType BaseAddr;
    uint32_t            NumP4Elements;        /**< Number of P4 Elements present in the VitisNetP4instance */
    uint32_t            NumComponents;      /**< Number of internal Components present in the VitisNetP4instance */
    uint32_t            ClkInHz;            /* Clock in Hz */
    uint32_t            PktRatePerSec;      /* Packet rate in packets per second */
    float               Pr2ClkRatio;       /* Clock to packet ration ratio */
};
/****************************************************************************************************************************************************/
/* SECTION: Local function declarations */
/****************************************************************************************************************************************************/
static XilVitisNetP4ReturnType XilVitisNetP4TargetBuildInfoValidateCtx(XilVitisNetP4TargetBuildInfoPrivateCtx *CtxPtr);
static XilVitisNetP4ReturnType XilVitisNetP4TargetInterruptValidateCtx(XilVitisNetP4TargetInterruptPrivateCtx *CtxPtr);
static XilVitisNetP4ReturnType XilVitisNetP4TargetInterruptValidateConfig(XilVitisNetP4TargetInterruptConfig *ConfigPtr);
static XilVitisNetP4ReturnType XilVitisNetP4CovertRegValueToEccStatus(uint32_t RegValue, uint32_t ElementId, XilVitisNetP4TargetInterruptEccErrorStatus *StatusPtr);
static XilVitisNetP4ReturnType XilVitisNetP4GetElementRegOffset(uint32_t ElementId, uint32_t* ElementRegOffsetPtr);
static XilVitisNetP4ReturnType XilVitisNetP4TargetInterruptValidateP4ElementId(uint32_t NumP4Elements, uint32_t ElementId);
static XilVitisNetP4ReturnType XilVitisNetP4TargetInterruptValidateEccErrorType(XilVitisNetP4TargetInterruptEccErrorType EccErrorType);
static XilVitisNetP4ReturnType XilVitisNetP4GetElementSetRegValue(uint32_t ElementId, XilVitisNetP4TargetInterruptEccErrorType EccErrorType, uint32_t* ElementRegValuePtr);
static XilVitisNetP4ReturnType XilVitisNetP4GetElementClearRegMask(uint32_t ElementId, XilVitisNetP4TargetInterruptEccErrorType EccErrorType, uint32_t* ElementRegMaskPtr);
static XilVitisNetP4ReturnType XilVitisNetP4TargetCtrlValidatePrlm(uint32_t PacketRateMargin);
static XilVitisNetP4ReturnType XilVitisNetP4TargetCtrlValidateConfig(XilVitisNetP4TargetCtrlConfig* ConfigPtr);
static XilVitisNetP4ReturnType XilVitisNetP4TargetCtrlValidateCtx(XilVitisNetP4TargetCtrlPrivateCtx* CtxPtr);

/***************************************************************************************************************************************************/
/* SECTION: Global function definitions for Inst Info driver*/
/****************************************************************************************************************************************************/

XilVitisNetP4ReturnType XilVitisNetP4TargetBuildInfoInit(XilVitisNetP4TargetBuildInfoCtx *CtxPtr, XilVitisNetP4EnvIf *EnvIfPtr, XilVitisNetP4TargetBuildInfoConfig *ConfigPtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    XilVitisNetP4TargetBuildInfoPrivateCtx *PrivateCtxPtr = NULL;

    if ((CtxPtr == NULL) || (ConfigPtr == NULL))
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    Result = XilVitisNetP4ValidateEnvIf(EnvIfPtr);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }

    PrivateCtxPtr = calloc(1, sizeof(XilVitisNetP4TargetBuildInfoPrivateCtx));
    if (PrivateCtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_BUILD_INFO_ERR_MALLOC_FAILED;
    }

    memcpy(&(PrivateCtxPtr->EnvIf), EnvIfPtr, sizeof(XilVitisNetP4EnvIf));
    PrivateCtxPtr->BaseAddr = ConfigPtr->BaseAddr;

    CtxPtr->PrivateCtxPtr = PrivateCtxPtr;

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4TargetBuildInfoGetIpVersion(XilVitisNetP4TargetBuildInfoCtx *CtxPtr, XilVitisNetP4Version *IpVersionPtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    XilVitisNetP4TargetBuildInfoPrivateCtx *PrivateCtxPtr = NULL;
    uint32_t IpVersionRegOffset;
    uint32_t IpVersionRegValue;

    if ((CtxPtr == NULL) || (IpVersionPtr == NULL))
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    Result = XilVitisNetP4TargetBuildInfoValidateCtx(CtxPtr->PrivateCtxPtr);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }

    PrivateCtxPtr = CtxPtr->PrivateCtxPtr;

    IpVersionRegOffset = PrivateCtxPtr->BaseAddr + XIL_VITIS_NET_P4_BUILD_INFO_VERSION_REG;
    Result = PrivateCtxPtr->EnvIf.WordRead32(&(PrivateCtxPtr->EnvIf), IpVersionRegOffset, &IpVersionRegValue);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }

    IpVersionPtr->Major = (IpVersionRegValue & XIL_VITIS_NET_P4_BUILD_INFO_VERSION_MAJOR_MASK) >> XIL_VITIS_NET_P4_BUILD_INFO_VERSION_MAJOR_SHIFT;
    IpVersionPtr->Minor = (IpVersionRegValue & XIL_VITIS_NET_P4_BUILD_INFO_VERSION_MINOR_MASK) >> XIL_VITIS_NET_P4_BUILD_INFO_VERSION_MINOR_SHIFT;
    IpVersionPtr->Revision = (IpVersionRegValue & XIL_VITIS_NET_P4_BUILD_INFO_VERSION_REVISION_MASK) >> XIL_VITIS_NET_P4_BUILD_INFO_VERSION_REVISON_SHIFT;

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4TargetBuildInfoGetIpSettings(XilVitisNetP4TargetBuildInfoCtx *CtxPtr, XilVitisNetP4TargetBuildInfoIpSettings *IpSettingsPtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    XilVitisNetP4TargetBuildInfoPrivateCtx *PrivateCtxPtr = NULL;
    uint32_t RegOffset;
    uint32_t RegValue;

    if ((CtxPtr == NULL) || (IpSettingsPtr == NULL))
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    Result = XilVitisNetP4TargetBuildInfoValidateCtx(CtxPtr->PrivateCtxPtr);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }

    PrivateCtxPtr = CtxPtr->PrivateCtxPtr;

    RegOffset = PrivateCtxPtr->BaseAddr + XIL_VITIS_NET_P4_BUILD_INFO_SETTINGS_REG;
    Result = PrivateCtxPtr->EnvIf.WordRead32(&(PrivateCtxPtr->EnvIf), RegOffset, &RegValue);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }

    IpSettingsPtr->PacketRateMps = (RegValue & XIL_VITIS_NET_P4_BUILD_INFO_SETTINGS_PKT_RATE_MASK) >> XIL_VITIS_NET_P4_BUILD_INFO_SETTINGS_PKT_RATE_SHIFT;
    IpSettingsPtr->CamMemoryClockMhz = (RegValue & XIL_VITIS_NET_P4_BUILD_INFO_SETTINGS_CAM_MEM_CLK_MASK) >> XIL_VITIS_NET_P4_BUILD_INFO_SETTINGS_CAM_MEM_CLK_SHIFT;
    IpSettingsPtr->AxiStreamClockMhz = (RegValue & XIL_VITIS_NET_P4_BUILD_INFO_SETTINGS_AXIS_CLK_MASK) >> XIL_VITIS_NET_P4_BUILD_INFO_SETTINGS_AXIS_CLK_SHIFT;

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4TargetBuildInfoExit(XilVitisNetP4TargetBuildInfoCtx *CtxPtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;

    if (CtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    Result = XilVitisNetP4TargetBuildInfoValidateCtx(CtxPtr->PrivateCtxPtr);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }

    free(CtxPtr->PrivateCtxPtr);
    CtxPtr->PrivateCtxPtr = NULL;

    return Result;
}

/****************************************************************************************************************************************************/
/* SECTION: Global function definitions for Intr driver*/
/****************************************************************************************************************************************************/

XilVitisNetP4ReturnType XilVitisNetP4TargetInterruptInit(XilVitisNetP4TargetInterruptCtx *CtxPtr, XilVitisNetP4EnvIf *EnvIfPtr, XilVitisNetP4TargetInterruptConfig *ConfigPtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    XilVitisNetP4TargetInterruptPrivateCtx *PrivateCtxPtr = NULL;
    XilVitisNetP4TargetInterruptComponentName *ComponentNameListPtr = NULL;
    XilVitisNetP4TargetInterruptComponentName *ComponentNamePtr = NULL;
    char *ComponentNameStringPtr = NULL;
    uint32_t NumComponents;
    uint32_t ComponentIndex;

    if (CtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    Result = XilVitisNetP4ValidateEnvIf(EnvIfPtr);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }

    Result = XilVitisNetP4TargetInterruptValidateConfig(ConfigPtr);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }

    PrivateCtxPtr = calloc(1, sizeof(XilVitisNetP4TargetInterruptPrivateCtx));
    if (PrivateCtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_INTERRUPT_ERR_MALLOC_FAILED;
    }

    NumComponents = ConfigPtr->NumComponents;
    
    /* Allocating array to hold Component names */
    ComponentNameListPtr = calloc(ConfigPtr->NumComponents, sizeof(XilVitisNetP4TargetInterruptComponentName));
    if (ComponentNameListPtr == NULL)
    {
        Result = XIL_VITIS_NET_P4_INTERRUPT_ERR_MALLOC_FAILED;
        goto fail_component_name_list_alloc;
    }

    for (ComponentIndex = 0; ComponentIndex < NumComponents; ComponentIndex++)
    {
        ComponentNameStringPtr = ConfigPtr->ComponentNameList[ComponentIndex].NameStringPtr;
        ComponentNamePtr = &(ComponentNameListPtr[ComponentIndex]);

        ComponentNamePtr->NameSizeBytes = strlen(ComponentNameStringPtr) + 1;
        if (ComponentNamePtr->NameSizeBytes == 1)
        {
            Result = XIL_VITIS_NET_P4_INTERRUPT_ERR_CONFIG_INVALID_COMPONENT_NAME;
            goto fail_component_name_list_init_copy;
        }

        //ComponentNamePtr->NameStringPtr = calloc(1, ComponentNamePtr->NameSizeBytes);
        (ComponentNameListPtr[ComponentIndex]).NameStringPtr = calloc(1, ComponentNamePtr->NameSizeBytes);
        if (ComponentNamePtr->NameStringPtr == NULL)
        {
            Result = XIL_VITIS_NET_P4_INTERRUPT_ERR_MALLOC_FAILED;
            goto fail_component_name_list_init_copy;
        }

        memcpy(ComponentNamePtr->NameStringPtr, ComponentNameStringPtr, ComponentNamePtr->NameSizeBytes);
    }

    memcpy(&(PrivateCtxPtr->EnvIf), EnvIfPtr, sizeof(XilVitisNetP4EnvIf));

    PrivateCtxPtr->BaseAddr = ConfigPtr->BaseAddr;
    PrivateCtxPtr->NumP4Elements = ConfigPtr->NumP4Elements;

    PrivateCtxPtr->NumComponents = ConfigPtr->NumComponents;
    PrivateCtxPtr->ComponentNameListPtr = ComponentNameListPtr;

    CtxPtr->PrivateCtxPtr = PrivateCtxPtr;

    return Result;

fail_component_name_list_init_copy:
    while (ComponentIndex-- != 0)
    {
        ComponentNamePtr = &(ComponentNameListPtr[ComponentIndex]);
        ;
        free(ComponentNamePtr->NameStringPtr);
    }
    free(ComponentNameListPtr);
fail_component_name_list_alloc:
    free(PrivateCtxPtr);

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4TargetInterruptEnableP4ElementEccErrorById(XilVitisNetP4TargetInterruptCtx *CtxPtr,
                                                                    uint32_t ElementId,
                                                                    XilVitisNetP4TargetInterruptEccErrorType EccErrorType)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    XilVitisNetP4TargetInterruptPrivateCtx *PrivateCtxPtr = NULL;
    uint32_t ElementRegOffset;
    uint32_t ElementRegValue;
    uint32_t RegValue;
    uint32_t RegAddr;

    if (CtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    Result = XilVitisNetP4TargetInterruptValidateCtx(CtxPtr->PrivateCtxPtr);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }
    PrivateCtxPtr = CtxPtr->PrivateCtxPtr;

    Result = XilVitisNetP4TargetInterruptValidateP4ElementId(PrivateCtxPtr->NumP4Elements, ElementId);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }

    Result = XilVitisNetP4TargetInterruptValidateEccErrorType(EccErrorType);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        Result = XilVitisNetP4GetElementRegOffset(ElementId, &ElementRegOffset);
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        Result = XilVitisNetP4GetElementSetRegValue(ElementId, EccErrorType, &ElementRegValue);
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        RegAddr = PrivateCtxPtr->BaseAddr + XIL_VITIS_NET_P4_INTERRUPT_ELEMENT_ENABLE_REG + ElementRegOffset;
        Result = PrivateCtxPtr->EnvIf.WordRead32(&(PrivateCtxPtr->EnvIf), RegAddr, &RegValue);
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        RegValue = RegValue | ElementRegValue;
        Result = PrivateCtxPtr->EnvIf.WordWrite32(&(PrivateCtxPtr->EnvIf), RegAddr, RegValue);
    }

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4TargetInterruptDisableP4ElementEccErrorById(XilVitisNetP4TargetInterruptCtx *CtxPtr,
                                                                     uint32_t ElementId,
                                                                     XilVitisNetP4TargetInterruptEccErrorType EccErrorType)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    XilVitisNetP4TargetInterruptPrivateCtx *PrivateCtxPtr = NULL;
    uint32_t ElementRegOffset;
    uint32_t ElementRegMask;
    uint32_t RegValue;
    uint32_t RegAddr;

    if (CtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    Result = XilVitisNetP4TargetInterruptValidateCtx(CtxPtr->PrivateCtxPtr);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }
    PrivateCtxPtr = CtxPtr->PrivateCtxPtr;

    Result = XilVitisNetP4TargetInterruptValidateP4ElementId(PrivateCtxPtr->NumP4Elements, ElementId);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }

    Result = XilVitisNetP4TargetInterruptValidateEccErrorType(EccErrorType);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        Result = XilVitisNetP4GetElementRegOffset(ElementId, &ElementRegOffset);
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        Result = XilVitisNetP4GetElementClearRegMask(ElementId, EccErrorType, &ElementRegMask);
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        RegAddr = PrivateCtxPtr->BaseAddr + XIL_VITIS_NET_P4_INTERRUPT_ELEMENT_ENABLE_REG + ElementRegOffset;
        Result = PrivateCtxPtr->EnvIf.WordRead32(&(PrivateCtxPtr->EnvIf), RegAddr, &RegValue);
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        RegValue = RegValue & ElementRegMask;
        Result = PrivateCtxPtr->EnvIf.WordWrite32(&(PrivateCtxPtr->EnvIf), RegAddr, RegValue);
    }

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4TargetInterruptGetP4ElementEccErrorStatusById(XilVitisNetP4TargetInterruptCtx *CtxPtr,
                                                                       uint32_t ElementId,
                                                                       XilVitisNetP4TargetInterruptEccErrorStatus *StatusPtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    XilVitisNetP4TargetInterruptPrivateCtx *PrivateCtxPtr = NULL;
    uint32_t ElementRegOffset;
    uint32_t RegAddr;
    uint32_t RegValue;

    if ((CtxPtr == NULL) || (StatusPtr == NULL))
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    Result = XilVitisNetP4TargetInterruptValidateCtx(CtxPtr->PrivateCtxPtr);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }
    PrivateCtxPtr = CtxPtr->PrivateCtxPtr;

    Result = XilVitisNetP4TargetInterruptValidateP4ElementId(PrivateCtxPtr->NumP4Elements, ElementId);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        Result = XilVitisNetP4GetElementRegOffset(ElementId, &ElementRegOffset);
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        RegAddr = PrivateCtxPtr->BaseAddr + XIL_VITIS_NET_P4_INTERRUPT_ELEMENT_STATUS_REG + ElementRegOffset;
        Result = PrivateCtxPtr->EnvIf.WordRead32(&(PrivateCtxPtr->EnvIf), RegAddr, &RegValue);
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        Result = XilVitisNetP4CovertRegValueToEccStatus(RegValue, ElementId, StatusPtr);
    }

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4TargetInterruptClearP4ElementEccErrorStatusById(XilVitisNetP4TargetInterruptCtx *CtxPtr,
                                                                         uint32_t ElementId,
                                                                         XilVitisNetP4TargetInterruptEccErrorType EccErrorType)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    XilVitisNetP4TargetInterruptPrivateCtx *PrivateCtxPtr = NULL;
    uint32_t ElementRegOffset;
    uint32_t ElementRegValue;
    uint32_t RegAddr;

    if (CtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    Result = XilVitisNetP4TargetInterruptValidateCtx(CtxPtr->PrivateCtxPtr);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }
    PrivateCtxPtr = CtxPtr->PrivateCtxPtr;

    Result = XilVitisNetP4TargetInterruptValidateP4ElementId(PrivateCtxPtr->NumP4Elements, ElementId);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }

    Result = XilVitisNetP4TargetInterruptValidateEccErrorType(EccErrorType);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        Result = XilVitisNetP4GetElementRegOffset(ElementId, &ElementRegOffset);
    }

    /* To clearing Status means writing a one to the bit so to clear we get the set value */
    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        Result = XilVitisNetP4GetElementSetRegValue(ElementId, EccErrorType, &ElementRegValue);
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        RegAddr = PrivateCtxPtr->BaseAddr + XIL_VITIS_NET_P4_INTERRUPT_ELEMENT_CLEAR_REG + ElementRegOffset;
        Result = PrivateCtxPtr->EnvIf.WordWrite32(&(PrivateCtxPtr->EnvIf), RegAddr, ElementRegValue);
    }

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4TargetInterruptGetP4ElementCount(XilVitisNetP4TargetInterruptCtx *CtxPtr, uint32_t *NumP4ElementsPtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    XilVitisNetP4TargetInterruptPrivateCtx *PrivateCtxPtr = NULL;

    if ((CtxPtr == NULL) || (NumP4ElementsPtr == NULL))
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    Result = XilVitisNetP4TargetInterruptValidateCtx(CtxPtr->PrivateCtxPtr);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }

    PrivateCtxPtr = CtxPtr->PrivateCtxPtr;

    *NumP4ElementsPtr = PrivateCtxPtr->NumP4Elements;

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4TargetInterruptGetComponentCount(XilVitisNetP4TargetInterruptCtx *CtxPtr, uint32_t *NumComponentPtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    XilVitisNetP4TargetInterruptPrivateCtx *PrivateCtxPtr = NULL;

    if ((CtxPtr == NULL) || (NumComponentPtr == NULL))
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    Result = XilVitisNetP4TargetInterruptValidateCtx(CtxPtr->PrivateCtxPtr);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }

    PrivateCtxPtr = CtxPtr->PrivateCtxPtr;

    *NumComponentPtr = PrivateCtxPtr->NumComponents;

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4TargetInterruptGetComponentIndexByName(XilVitisNetP4TargetInterruptCtx *CtxPtr, char *ComponentNamePtr, uint32_t *IndexPtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    XilVitisNetP4TargetInterruptPrivateCtx *PrivateCtxPtr = NULL;
    uint32_t ComponentIndex;
    XilVitisNetP4TargetInterruptComponentName *ListComponentNamePtr = NULL;
    int StrcmpResult = 0;

    if ((CtxPtr == NULL) || (ComponentNamePtr == NULL) || (IndexPtr == NULL))
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    Result = XilVitisNetP4TargetInterruptValidateCtx(CtxPtr->PrivateCtxPtr);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }

    PrivateCtxPtr = CtxPtr->PrivateCtxPtr;

    /* Assumes no match by default - overriden if a match is found */
    Result = XIL_VITIS_NET_P4_INTERRUPT_ERR_COMPONENT_NOT_FOUND;
    for (ComponentIndex = 0; ComponentIndex < PrivateCtxPtr->NumComponents; ComponentIndex++)
    {
        ListComponentNamePtr = &(PrivateCtxPtr->ComponentNameListPtr[ComponentIndex]);
        StrcmpResult = strncmp(ComponentNamePtr, ListComponentNamePtr->NameStringPtr, ListComponentNamePtr->NameSizeBytes + 2);
        if (StrcmpResult == 0)
        {
            /* Assumes TableIndex and ElementID are the same, which is valid until more P4 elements are supportted */
            *IndexPtr = ComponentIndex;
            Result = XIL_VITIS_NET_P4_SUCCESS;
            break;
        }
    }

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4TargetInterruptGetComponentNameByIndex(XilVitisNetP4TargetInterruptCtx *CtxPtr,
                                                             uint32_t *IndexPtr,
                                                             char *ComponentNamePtr,
                                                             uint32_t ComponentNameNumBytes)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    XilVitisNetP4TargetInterruptPrivateCtx *PrivateCtxPtr = NULL;
    XilVitisNetP4TargetInterruptComponentName *ListComponentNamePtr = NULL;

    if ((CtxPtr == NULL) || (IndexPtr == NULL) || (ComponentNamePtr == NULL))
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    // Add Check for NumBytes > 0

    Result = XilVitisNetP4TargetInterruptValidateCtx(CtxPtr->PrivateCtxPtr);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }

    PrivateCtxPtr = CtxPtr->PrivateCtxPtr;

    if (*IndexPtr >= PrivateCtxPtr->NumComponents)
    {
        return XIL_VITIS_NET_P4_INTERRUPT_ERR_COMPONENT_NOT_FOUND;
    }

    ListComponentNamePtr = &(PrivateCtxPtr->ComponentNameListPtr[*IndexPtr]);

    if (ComponentNameNumBytes < ListComponentNamePtr->NameSizeBytes)
    {
        return XIL_VITIS_NET_P4_INTERRUPT_ERR_NAME_BUFFER_TOO_SMALL;
    }

    memcpy(ComponentNamePtr, ListComponentNamePtr->NameStringPtr, ListComponentNamePtr->NameSizeBytes);
    (*IndexPtr)++;

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4TargetInterruptEnableComponentEccErrorByIndex(XilVitisNetP4TargetInterruptCtx *CtxPtr,
                                                                         uint32_t ComponentIndex,
                                                                         XilVitisNetP4TargetInterruptEccErrorType EccErrorType)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    XilVitisNetP4TargetInterruptPrivateCtx *PrivateCtxPtr = NULL;
    uint32_t ValueForIndex;
    uint32_t RegValue;
    uint32_t RegAddr;

    if (CtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    Result = XilVitisNetP4TargetInterruptValidateCtx(CtxPtr->PrivateCtxPtr);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }

    PrivateCtxPtr = CtxPtr->PrivateCtxPtr;

    if (ComponentIndex >= PrivateCtxPtr->NumComponents)
    {
        return XIL_VITIS_NET_P4_INTERRUPT_ERR_INVALID_COMPONENT_INDEX;
    }

    Result = XilVitisNetP4TargetInterruptValidateEccErrorType(EccErrorType);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        Result = XilVitisNetP4GetElementSetRegValue(ComponentIndex, EccErrorType, &ValueForIndex);
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        RegAddr = PrivateCtxPtr->BaseAddr + XIL_VITIS_NET_P4_INTERRUPT_FIFO_ENABLE_REG;
        Result = PrivateCtxPtr->EnvIf.WordRead32(&(PrivateCtxPtr->EnvIf), RegAddr, &RegValue);
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        RegValue = RegValue | ValueForIndex;
        Result = PrivateCtxPtr->EnvIf.WordWrite32(&(PrivateCtxPtr->EnvIf), RegAddr, RegValue);
    }

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4TargetInterruptDisableComponentEccErrorByIndex(XilVitisNetP4TargetInterruptCtx *CtxPtr,
                                                                          uint32_t ComponentIndex,
                                                                          XilVitisNetP4TargetInterruptEccErrorType EccErrorType)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    XilVitisNetP4TargetInterruptPrivateCtx *PrivateCtxPtr = NULL;
    uint32_t IndexRegMask;
    uint32_t RegValue;
    uint32_t RegAddr;

    if (CtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    Result = XilVitisNetP4TargetInterruptValidateCtx(CtxPtr->PrivateCtxPtr);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }

    PrivateCtxPtr = CtxPtr->PrivateCtxPtr;

    if (ComponentIndex >= PrivateCtxPtr->NumComponents)
    {
        return XIL_VITIS_NET_P4_INTERRUPT_ERR_INVALID_COMPONENT_INDEX;
    }

    Result = XilVitisNetP4TargetInterruptValidateEccErrorType(EccErrorType);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        Result = XilVitisNetP4GetElementClearRegMask(ComponentIndex, EccErrorType, &IndexRegMask);
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        RegAddr = PrivateCtxPtr->BaseAddr + XIL_VITIS_NET_P4_INTERRUPT_FIFO_ENABLE_REG;
        Result = PrivateCtxPtr->EnvIf.WordRead32(&(PrivateCtxPtr->EnvIf), RegAddr, &RegValue);
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        RegValue = RegValue & IndexRegMask;
        Result = PrivateCtxPtr->EnvIf.WordWrite32(&(PrivateCtxPtr->EnvIf), RegAddr, RegValue);
    }

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4TargetInterruptGetComponentEccErrorStatusByIndex(XilVitisNetP4TargetInterruptCtx *CtxPtr,
                                                                            uint32_t ComponentIndex,
                                                                            XilVitisNetP4TargetInterruptEccErrorStatus *StatusPtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    XilVitisNetP4TargetInterruptPrivateCtx *PrivateCtxPtr = NULL;
    uint32_t RegAddr;
    uint32_t RegValue;

    if ((CtxPtr == NULL) || (StatusPtr == NULL))
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    Result = XilVitisNetP4TargetInterruptValidateCtx(CtxPtr->PrivateCtxPtr);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }

    PrivateCtxPtr = CtxPtr->PrivateCtxPtr;

    if (ComponentIndex >= PrivateCtxPtr->NumComponents)
    {
        return XIL_VITIS_NET_P4_INTERRUPT_ERR_INVALID_COMPONENT_INDEX;
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        RegAddr = PrivateCtxPtr->BaseAddr + XIL_VITIS_NET_P4_INTERRUPT_FIFO_STATUS_REG;
        Result = PrivateCtxPtr->EnvIf.WordRead32(&(PrivateCtxPtr->EnvIf), RegAddr, &RegValue);
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        Result = XilVitisNetP4CovertRegValueToEccStatus(RegValue, ComponentIndex, StatusPtr);
    }

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4TargetInterruptClearComponentEccErrorStatusByIndex(XilVitisNetP4TargetInterruptCtx *CtxPtr,
                                                                              uint32_t ComponentIndex,
                                                                              XilVitisNetP4TargetInterruptEccErrorType EccErrorType)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    XilVitisNetP4TargetInterruptPrivateCtx *PrivateCtxPtr = NULL;
    uint32_t ValueForIndex;
    uint32_t RegAddr;

    if (CtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    Result = XilVitisNetP4TargetInterruptValidateCtx(CtxPtr->PrivateCtxPtr);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }

    PrivateCtxPtr = CtxPtr->PrivateCtxPtr;

    Result = XilVitisNetP4TargetInterruptValidateEccErrorType(EccErrorType);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }

    if (ComponentIndex >= PrivateCtxPtr->NumComponents)
    {
        return XIL_VITIS_NET_P4_INTERRUPT_ERR_INVALID_COMPONENT_INDEX;
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        Result = XilVitisNetP4GetElementSetRegValue(ComponentIndex, EccErrorType, &ValueForIndex);
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        RegAddr = PrivateCtxPtr->BaseAddr + XIL_VITIS_NET_P4_INTERRUPT_FIFO_CLEAR_REG;
        Result = PrivateCtxPtr->EnvIf.WordWrite32(&(PrivateCtxPtr->EnvIf), RegAddr, ValueForIndex);
    }

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4TargetInterruptExit(XilVitisNetP4TargetInterruptCtx *CtxPtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    XilVitisNetP4TargetInterruptPrivateCtx *PrivateCtxPtr = NULL;
    uint32_t ComponentIndex;
    XilVitisNetP4TargetInterruptComponentName *ComponentNamePtr = NULL;

    if (CtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    Result = XilVitisNetP4TargetInterruptValidateCtx(CtxPtr->PrivateCtxPtr);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }

    PrivateCtxPtr = CtxPtr->PrivateCtxPtr;

    for (ComponentIndex = 0; ComponentIndex < PrivateCtxPtr->NumComponents; ComponentIndex++)
    {
        ComponentNamePtr = &(PrivateCtxPtr->ComponentNameListPtr[ComponentIndex]);
        free(ComponentNamePtr->NameStringPtr);
        ComponentNamePtr->NameSizeBytes = 0;
    }
    free(PrivateCtxPtr->ComponentNameListPtr);

    free(CtxPtr->PrivateCtxPtr);
    CtxPtr->PrivateCtxPtr = NULL;

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4TargetCtrlInit(XilVitisNetP4TargetCtrlCtx* CtxPtr, XilVitisNetP4EnvIf* EnvIfPtr, XilVitisNetP4TargetCtrlConfig* ConfigPtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    XilVitisNetP4TargetCtrlPrivateCtx* PrivateCtxPtr = NULL;

    if (CtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    Result = XilVitisNetP4ValidateEnvIf(EnvIfPtr);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }

    Result = XilVitisNetP4TargetCtrlValidateConfig(ConfigPtr);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }

    PrivateCtxPtr = calloc(1, sizeof(XilVitisNetP4TargetCtrlPrivateCtx));
    if (PrivateCtxPtr == NULL)
    {
        free(PrivateCtxPtr);
        return XIL_VITIS_NET_P4_INTERRUPT_ERR_MALLOC_FAILED;
    }
    memcpy(&(PrivateCtxPtr->EnvIf), EnvIfPtr, sizeof(XilVitisNetP4EnvIf));

    PrivateCtxPtr->BaseAddr = ConfigPtr->BaseAddr;
    PrivateCtxPtr->NumP4Elements = ConfigPtr->NumP4Elements;
    PrivateCtxPtr->NumComponents = ConfigPtr->NumComponents;
    PrivateCtxPtr->ClkInHz = ConfigPtr->ClkInHz;
    PrivateCtxPtr->PktRatePerSec = ConfigPtr->PktRatePerSec;
    PrivateCtxPtr->Pr2ClkRatio = ((float)(PrivateCtxPtr->PktRatePerSec) / (float)(PrivateCtxPtr->ClkInHz));

    if (PrivateCtxPtr->Pr2ClkRatio > XIL_VITIS_NET_P4_MAX_PR2CLK_RATIO)
    {
        Result = XIL_VITIS_NET_P4_CONTROL_ERR_INVALID_PR2CLK_RATIO_ERROR_TYPE;
    }

    CtxPtr->PrivateCtxPtr = PrivateCtxPtr;

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4TargetCtrlExit(XilVitisNetP4TargetCtrlCtx* CtxPtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;

    if (CtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    Result = XilVitisNetP4TargetCtrlValidateCtx(CtxPtr->PrivateCtxPtr);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }

    free(CtxPtr->PrivateCtxPtr);
    CtxPtr->PrivateCtxPtr = NULL;

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4TargetCtrlGetP4ElementCount(XilVitisNetP4TargetCtrlCtx* CtxPtr, uint32_t* NumElementsPtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    XilVitisNetP4TargetCtrlPrivateCtx* PrivateCtxPtr = NULL;

    if ((CtxPtr == NULL) || (NumElementsPtr == NULL))
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    Result = XilVitisNetP4TargetCtrlValidateCtx(CtxPtr->PrivateCtxPtr);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }

    PrivateCtxPtr = CtxPtr->PrivateCtxPtr;

    *NumElementsPtr = PrivateCtxPtr->NumP4Elements;

    return Result;
}


XilVitisNetP4ReturnType XilVitisNetP4TargetCtrlGetClkInHz(XilVitisNetP4TargetCtrlCtx* CtxPtr, uint32_t* ClkInHzPtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    XilVitisNetP4TargetCtrlPrivateCtx* PrivateCtxPtr = NULL;

    if ((CtxPtr == NULL) || (ClkInHzPtr == NULL))
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    Result = XilVitisNetP4TargetCtrlValidateCtx(CtxPtr->PrivateCtxPtr);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }

    PrivateCtxPtr = CtxPtr->PrivateCtxPtr;

    *ClkInHzPtr = PrivateCtxPtr->ClkInHz;

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4TargetCtrlGetNumComponents(XilVitisNetP4TargetCtrlCtx* CtxPtr, uint32_t* NumComponentsPtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    XilVitisNetP4TargetCtrlPrivateCtx* PrivateCtxPtr = NULL;

    if ((CtxPtr == NULL) || (NumComponentsPtr == NULL))
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    Result = XilVitisNetP4TargetCtrlValidateCtx(CtxPtr->PrivateCtxPtr);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }

    PrivateCtxPtr = CtxPtr->PrivateCtxPtr;

    *NumComponentsPtr = PrivateCtxPtr->NumComponents;

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4TargetCtrlGetPktRatePerSec(XilVitisNetP4TargetCtrlCtx* CtxPtr, uint32_t* PktRatePerSecPtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    XilVitisNetP4TargetCtrlPrivateCtx* PrivateCtxPtr = NULL;

    if ((CtxPtr == NULL) || (PktRatePerSecPtr == NULL))
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    Result = XilVitisNetP4TargetCtrlValidateCtx(CtxPtr->PrivateCtxPtr);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }

    PrivateCtxPtr = CtxPtr->PrivateCtxPtr;

    *PktRatePerSecPtr = PrivateCtxPtr->PktRatePerSec;

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4TargetCtrlGetPr2ClkRatio(XilVitisNetP4TargetCtrlCtx* CtxPtr, float* Pr2ClkRatioPtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    XilVitisNetP4TargetCtrlPrivateCtx* PrivateCtxPtr = NULL;

    if ((CtxPtr == NULL) || (Pr2ClkRatioPtr == NULL))
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    Result = XilVitisNetP4TargetCtrlValidateCtx(CtxPtr->PrivateCtxPtr);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }

    PrivateCtxPtr = CtxPtr->PrivateCtxPtr;

    *Pr2ClkRatioPtr = PrivateCtxPtr->Pr2ClkRatio;

    return Result;
}


XilVitisNetP4ReturnType XilVitisNetP4TargetCtrlSoftResetEngine(XilVitisNetP4TargetCtrlCtx* CtxPtr, XilVitisNetP4TargetCtrlEngineId EngineId)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    XilVitisNetP4TargetCtrlPrivateCtx* PrivateCtxPtr = NULL;
    uint32_t RegValue;
    uint32_t RegAddr;

    if (CtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    Result = XilVitisNetP4TargetCtrlValidateCtx(CtxPtr->PrivateCtxPtr);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }
    else
    {
        PrivateCtxPtr = CtxPtr->PrivateCtxPtr;
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        switch (EngineId)
        {
            case XIL_VITIS_NET_P4_TARGET_CTRL_ALL_ENGINES:
                RegValue = XIL_VITIS_NET_P4_TARGET_CTRL_SRE_ALL_ENGINES;
                break;

            case XIL_VITIS_NET_P4_TARGET_CTRL_DEPARSER_ENGINE:
                RegValue = XIL_VITIS_NET_P4_TARGET_CTRL_SRE_DEPARSER_ENGINE;
                break;

            case XIL_VITIS_NET_P4_TARGET_CTRL_FIFO_ENGINE:
                RegValue = XIL_VITIS_NET_P4_TARGET_CTRL_SRE_COMPONENT_ENGINE;
                break;

            case XIL_VITIS_NET_P4_TARGET_CTRL_MA_ENGINE:
                RegValue = XIL_VITIS_NET_P4_TARGET_CTRL_SRE_MA_ENGINE;
                break;

            case XIL_VITIS_NET_P4_TARGET_CTRL_PARSER_ENGINE:
                RegValue = XIL_VITIS_NET_P4_TARGET_CTRL_SRE_PARSER_ENGINE;
                break;

            default:
                Result = XIL_VITIS_NET_P4_CONTROL_ERR_INVALID_ENGINE_ID_ERROR_TYPE;
                break;
        }
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        RegAddr = PrivateCtxPtr->BaseAddr + XIL_VITIS_NET_P4_CONTROL_SRE_REG;
        Result = PrivateCtxPtr->EnvIf.WordWrite32(&(PrivateCtxPtr->EnvIf), RegAddr, RegValue);
    }

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4TargetCtrlSetPacketRateLimitMargin(XilVitisNetP4TargetCtrlCtx* CtxPtr, uint32_t PacketRateMargin)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    XilVitisNetP4TargetCtrlPrivateCtx* PrivateCtxPtr = NULL;
    uint32_t RegValue;
    uint32_t RegAddr;
    uint32_t MaxSofPerIntvl;

    if (CtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    Result = XilVitisNetP4TargetCtrlValidateCtx(CtxPtr->PrivateCtxPtr);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }
    PrivateCtxPtr = CtxPtr->PrivateCtxPtr;

    Result = XilVitisNetP4TargetCtrlValidatePrlm(PacketRateMargin);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }

    MaxSofPerIntvl = (uint32_t)((1000 * PrivateCtxPtr->Pr2ClkRatio) - (float)(PacketRateMargin));
    if (MaxSofPerIntvl > XIL_VITIS_NET_P4_TARGET_CTRL_MAX_SOF_PER_INTV)
    {
        Result = XIL_VITIS_NET_P4_CONTROL_ERR_INVALID_PRM_VALUE_ERROR_TYPE;
    }

   
    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        RegAddr = PrivateCtxPtr->BaseAddr + XIL_VITIS_NET_P4_CONTROL_PRLM_REG;
        RegValue = MaxSofPerIntvl;
        Result = PrivateCtxPtr->EnvIf.WordWrite32(&(PrivateCtxPtr->EnvIf), RegAddr, RegValue);
    }

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4TargetCtrlGetPacketRateLimitMargin(XilVitisNetP4TargetCtrlCtx* CtxPtr, uint32_t* PacketRateMarginPtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    XilVitisNetP4TargetCtrlPrivateCtx* PrivateCtxPtr = NULL;
    uint32_t RegValue;
    uint32_t RegAddr;
    uint32_t MaxSofPerIntvl;

    if ((CtxPtr == NULL) || (PacketRateMarginPtr == NULL))
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    Result = XilVitisNetP4TargetCtrlValidateCtx(CtxPtr->PrivateCtxPtr);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }
    PrivateCtxPtr = CtxPtr->PrivateCtxPtr;

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        RegAddr = PrivateCtxPtr->BaseAddr + XIL_VITIS_NET_P4_CONTROL_PRLM_REG;
        Result = PrivateCtxPtr->EnvIf.WordRead32(&(PrivateCtxPtr->EnvIf), RegAddr, &RegValue);
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        MaxSofPerIntvl = RegValue & XIL_VITIS_NET_P4_CONTROL_PRLM_MASK;
        *PacketRateMarginPtr = (uint32_t)((1000 * PrivateCtxPtr->Pr2ClkRatio) - (float)(MaxSofPerIntvl));
    }

    return Result;
}


XilVitisNetP4ReturnType XilVitisNetP4TargetCtrlIpComponentEnableInjectEccError(XilVitisNetP4TargetCtrlCtx* CtxPtr,
                                                                     uint32_t ComponentIndex,
                                                                     XilVitisNetP4TargetInterruptEccErrorType EccErrorType)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    XilVitisNetP4TargetCtrlPrivateCtx* PrivateCtxPtr = NULL;
    uint32_t ValueForIndex;
    uint32_t RegValue;
    uint32_t RegAddr;

    /* Check input parameters */
    if (CtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    Result = XilVitisNetP4TargetCtrlValidateCtx(CtxPtr->PrivateCtxPtr);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }

    PrivateCtxPtr = CtxPtr->PrivateCtxPtr;

    if (ComponentIndex >= PrivateCtxPtr->NumComponents)
    {
        return XIL_VITIS_NET_P4_INTERRUPT_ERR_INVALID_COMPONENT_INDEX;
    }

    Result = XilVitisNetP4TargetInterruptValidateEccErrorType(EccErrorType);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        Result = XilVitisNetP4GetElementSetRegValue(ComponentIndex, EccErrorType, &ValueForIndex);
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        RegAddr = PrivateCtxPtr->BaseAddr + XIL_VITIS_NET_P4_CONTROL_IP_IEBE_REG;
        Result = PrivateCtxPtr->EnvIf.WordRead32(&(PrivateCtxPtr->EnvIf), RegAddr, &RegValue);
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        RegValue = RegValue | ValueForIndex;
        Result = PrivateCtxPtr->EnvIf.WordWrite32(&(PrivateCtxPtr->EnvIf), RegAddr, RegValue);
    }

    return Result;
}


XilVitisNetP4ReturnType XilVitisNetP4TargetCtrlIpComponentDisableInjectEccError(XilVitisNetP4TargetCtrlCtx* CtxPtr,
                                                                      uint32_t ComponentIndex,
                                                                      XilVitisNetP4TargetInterruptEccErrorType EccErrorType)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    XilVitisNetP4TargetCtrlPrivateCtx* PrivateCtxPtr = NULL;
    uint32_t IndexRegMask;
    uint32_t RegValue;
    uint32_t RegAddr;

    /* Check input parameters */
    if (CtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    Result = XilVitisNetP4TargetCtrlValidateCtx(CtxPtr->PrivateCtxPtr);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }

    PrivateCtxPtr = CtxPtr->PrivateCtxPtr;

    if (ComponentIndex >= PrivateCtxPtr->NumComponents)
    {
        return XIL_VITIS_NET_P4_INTERRUPT_ERR_INVALID_COMPONENT_INDEX;
    }

    Result = XilVitisNetP4TargetInterruptValidateEccErrorType(EccErrorType);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        Result = XilVitisNetP4GetElementClearRegMask(ComponentIndex, EccErrorType, &IndexRegMask);
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        RegAddr = PrivateCtxPtr->BaseAddr + XIL_VITIS_NET_P4_CONTROL_IP_IEBE_REG;
        Result = PrivateCtxPtr->EnvIf.WordRead32(&(PrivateCtxPtr->EnvIf), RegAddr, &RegValue);
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        RegValue = RegValue & IndexRegMask;
        Result = PrivateCtxPtr->EnvIf.WordWrite32(&(PrivateCtxPtr->EnvIf), RegAddr, RegValue);
    }

    return Result;
}


XilVitisNetP4ReturnType XilVitisNetP4TargetCtrlP4ElementEnableInjectEccError(XilVitisNetP4TargetCtrlCtx* CtxPtr, uint32_t P4ElementId, XilVitisNetP4TargetInterruptEccErrorType EccErrorType)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    XilVitisNetP4TargetCtrlPrivateCtx* PrivateCtxPtr = NULL;
    uint32_t ElementRegOffset;
    uint32_t ElementRegValue;
    uint32_t RegValue;
    uint32_t RegAddr;

    if (CtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    Result = XilVitisNetP4TargetCtrlValidateCtx(CtxPtr->PrivateCtxPtr);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }
    PrivateCtxPtr = CtxPtr->PrivateCtxPtr;

    Result = XilVitisNetP4TargetInterruptValidateP4ElementId(PrivateCtxPtr->NumP4Elements, P4ElementId);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }

    Result = XilVitisNetP4TargetInterruptValidateEccErrorType(EccErrorType);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        Result = XilVitisNetP4GetElementRegOffset(P4ElementId, &ElementRegOffset);
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        Result = XilVitisNetP4GetElementSetRegValue(P4ElementId, EccErrorType, &ElementRegValue);
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        RegAddr = PrivateCtxPtr->BaseAddr + XIL_VITIS_NET_P4_CONTROL_P4_IEBE_REG + ElementRegOffset;
        Result = PrivateCtxPtr->EnvIf.WordRead32(&(PrivateCtxPtr->EnvIf), RegAddr, &RegValue);
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        RegValue = RegValue | ElementRegValue;
        Result = PrivateCtxPtr->EnvIf.WordWrite32(&(PrivateCtxPtr->EnvIf), RegAddr, RegValue);
    }

    return Result;
}


XilVitisNetP4ReturnType XilVitisNetP4TargetCtrlP4ElementDisableInjectEccError(XilVitisNetP4TargetCtrlCtx* CtxPtr, uint32_t P4ElementId, XilVitisNetP4TargetInterruptEccErrorType EccErrorType)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    XilVitisNetP4TargetCtrlPrivateCtx* PrivateCtxPtr = NULL;
    uint32_t ElementRegOffset;
    uint32_t ElementRegMask;
    uint32_t RegValue;
    uint32_t RegAddr;

    if (CtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    Result = XilVitisNetP4TargetCtrlValidateCtx(CtxPtr->PrivateCtxPtr);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }
    PrivateCtxPtr = CtxPtr->PrivateCtxPtr;

    Result = XilVitisNetP4TargetInterruptValidateP4ElementId(PrivateCtxPtr->NumP4Elements, P4ElementId);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }

    Result = XilVitisNetP4TargetInterruptValidateEccErrorType(EccErrorType);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        Result = XilVitisNetP4GetElementRegOffset(P4ElementId, &ElementRegOffset);
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        Result = XilVitisNetP4GetElementClearRegMask(P4ElementId, EccErrorType, &ElementRegMask);
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        RegAddr = PrivateCtxPtr->BaseAddr + XIL_VITIS_NET_P4_CONTROL_P4_IEBE_REG + ElementRegOffset;
        Result = PrivateCtxPtr->EnvIf.WordRead32(&(PrivateCtxPtr->EnvIf), RegAddr, &RegValue);
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        RegValue = RegValue & ElementRegMask;
        Result = PrivateCtxPtr->EnvIf.WordWrite32(&(PrivateCtxPtr->EnvIf), RegAddr, RegValue);
    }

    return Result;
}

/****************************************************************************************************************************************************/
/* SECTION: Local function definitions */
/****************************************************************************************************************************************************/
static XilVitisNetP4ReturnType XilVitisNetP4TargetBuildInfoValidateCtx(XilVitisNetP4TargetBuildInfoPrivateCtx *CtxPtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;

    if (CtxPtr == NULL)
    {
        Result = XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        Result = XilVitisNetP4ValidateEnvIf(&(CtxPtr->EnvIf));
        if (Result != XIL_VITIS_NET_P4_SUCCESS)
        {
            Result = XIL_VITIS_NET_P4_GENERAL_ERR_INVALID_ENVIRONMENT_INTERFACE;
        }
    }

    return Result;
}

static XilVitisNetP4ReturnType XilVitisNetP4TargetInterruptValidateCtx(XilVitisNetP4TargetInterruptPrivateCtx *CtxPtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;

    if (CtxPtr == NULL)
    {
        Result = XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        Result = XilVitisNetP4ValidateEnvIf(&(CtxPtr->EnvIf));
        if (Result != XIL_VITIS_NET_P4_SUCCESS)
        {
            Result = XIL_VITIS_NET_P4_GENERAL_ERR_INVALID_ENVIRONMENT_INTERFACE;
        }
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        if (CtxPtr->NumP4Elements > XIL_VITIS_NET_P4_TARGET_MGMT_MAX_NUM_ELEMENTS)
        {
            Result = XIL_VITIS_NET_P4_INTERRUPT_ERR_CONFIG_MAX_NUM_ELEMENTS_EXCEED;
        }
    }

    return Result;
}

static XilVitisNetP4ReturnType XilVitisNetP4TargetInterruptValidateConfig(XilVitisNetP4TargetInterruptConfig *ConfigPtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    uint32_t ComponentIndex;

    if (ConfigPtr == NULL)
    {
        Result = XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        if (ConfigPtr->NumP4Elements > XIL_VITIS_NET_P4_TARGET_MGMT_MAX_NUM_ELEMENTS)
        {
            Result = XIL_VITIS_NET_P4_INTERRUPT_ERR_CONFIG_MAX_NUM_ELEMENTS_EXCEED;
        }
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        if (ConfigPtr->NumComponents > XIL_VITIS_NET_P4_TARGET_MGMT_MAX_NUM_FIFOS)
        {
            Result = XIL_VITIS_NET_P4_INTERRUPT_ERR_CONFIG_MAX_NUM_COMPONENTS_EXCEED;
        }
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        if (ConfigPtr->NumComponents > 0)
        {
            for (ComponentIndex = 0; ComponentIndex < ConfigPtr->NumComponents; ComponentIndex++)
            {
                if (ConfigPtr->ComponentNameList[ComponentIndex].NameStringPtr == NULL)
                {
                    Result = XIL_VITIS_NET_P4_INTERRUPT_ERR_CONFIG_INVALID_COMPONENT_NAME;
                    break;
                }
            }
        }
    }

    return Result;
}

static XilVitisNetP4ReturnType XilVitisNetP4CovertRegValueToEccStatus(uint32_t RegValue, uint32_t ElementId, XilVitisNetP4TargetInterruptEccErrorStatus *StatusPtr)
{
    uint32_t RShift;
    uint32_t StatusValue;

    RShift = (ElementId * XIL_VITIS_NET_P4_TARGET_MGMT_NUM_ECC_INTR_TYPES_PER_OBJECT) % XIL_VITIS_NET_P4_BITS_PER_REG;
    StatusValue = (RegValue >> RShift) & 3;

    if ((StatusValue & 0x1) == 0x1)
    {
        StatusPtr->SingleBitErrorStatus = true;
    }
    else
    {
        StatusPtr->SingleBitErrorStatus = false;
    }

    if ((StatusValue & 0x2) == 0x2)
    {
        StatusPtr->DoubleBitErrorStatus = true;
    }
    else
    {
        StatusPtr->DoubleBitErrorStatus = false;
    }

    return XIL_VITIS_NET_P4_SUCCESS;
}

static XilVitisNetP4ReturnType XilVitisNetP4TargetInterruptValidateP4ElementId(uint32_t NumP4Elements, uint32_t ElementId)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;

    if (ElementId >= NumP4Elements)
    {
        Result = XIL_VITIS_NET_P4_INTERRUPT_ERR_INVALID_ELEMENT_ID;
    }

    return Result;
}

static XilVitisNetP4ReturnType XilVitisNetP4TargetInterruptValidateEccErrorType(XilVitisNetP4TargetInterruptEccErrorType EccErrorType)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;

    if ((uint32_t)(EccErrorType) >= XIL_VITIS_NET_P4_INTERRUPT_ECC_ERROR_TYPE_MAX)
    {
        Result = XIL_VITIS_NET_P4_INTERRUPT_ERR_INVALID_ECC_ERROR_TYPE;
    }

    return Result;
}

static XilVitisNetP4ReturnType XilVitisNetP4GetElementRegOffset(uint32_t ElementId, uint32_t* ElementRegOffsetPtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    uint32_t NumRegs;

    if (ElementRegOffsetPtr == NULL)
    {
        Result = XIL_VITIS_NET_P4_GENERAL_ERR_INTERNAL_ASSERTION;
        return Result;
    }

    NumRegs = (ElementId * XIL_VITIS_NET_P4_TARGET_MGMT_NUM_ECC_INTR_TYPES_PER_OBJECT) / XIL_VITIS_NET_P4_BITS_PER_REG;
    *ElementRegOffsetPtr = NumRegs * XIL_VITIS_NET_P4_BYTES_PER_REG;

    return Result;
}

static XilVitisNetP4ReturnType XilVitisNetP4GetElementSetRegValue(uint32_t ElementId,
                                                        XilVitisNetP4TargetInterruptEccErrorType EccErrorType,
                                                        uint32_t* ElementRegValuePtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    uint32_t ObjectValue;
    uint32_t ObjectRegShift;

    if (ElementRegValuePtr == NULL)
    {
        Result = XIL_VITIS_NET_P4_GENERAL_ERR_INTERNAL_ASSERTION;
        return Result;
    }

    ObjectRegShift = (ElementId * XIL_VITIS_NET_P4_TARGET_MGMT_NUM_ECC_INTR_TYPES_PER_OBJECT) % XIL_VITIS_NET_P4_BITS_PER_REG;

    switch (EccErrorType)
    {
        case XIL_VITIS_NET_P4_INTERRUPT_ECC_ERROR_SINGLE_BIT:
            ObjectValue = 0x1;
            break;
        case XIL_VITIS_NET_P4_INTERRUPT_ECC_ERROR_DOUBLE_BIT:
            ObjectValue = 0x2;
            break;
        case XIL_VITIS_NET_P4_INTERRUPT_ECC_ERROR_ALL:
            ObjectValue = 0x3; /* Both bits are set */
            break;
        default:
            Result = XIL_VITIS_NET_P4_INTERRUPT_ERR_INVALID_ECC_ERROR_TYPE;
            break;
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        *ElementRegValuePtr = ObjectValue << ObjectRegShift;
    }

    return Result;
}

static XilVitisNetP4ReturnType XilVitisNetP4GetElementClearRegMask(uint32_t ElementId,
                                                         XilVitisNetP4TargetInterruptEccErrorType EccErrorType,
                                                         uint32_t* ElementRegMaskPtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;

    Result = XilVitisNetP4GetElementSetRegValue(ElementId, EccErrorType, ElementRegMaskPtr);
    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        *ElementRegMaskPtr = ~(*ElementRegMaskPtr);
    }

    return Result;
}

static XilVitisNetP4ReturnType XilVitisNetP4TargetCtrlValidatePrlm(uint32_t PacketRateMargin)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;

    if ((PacketRateMargin > XIL_VITIS_NET_P4_CONTROL_PRLM_MAX_VALUE) || (PacketRateMargin < XIL_VITIS_NET_P4_CONTROL_PRLM_MIN_VALUE))
    {
        Result = XIL_VITIS_NET_P4_CONTROL_ERR_INVALID_PRM_VALUE_ERROR_TYPE;
    }

    return Result;
}

static XilVitisNetP4ReturnType XilVitisNetP4TargetCtrlValidateConfig(XilVitisNetP4TargetCtrlConfig* ConfigPtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;

    if (ConfigPtr == NULL)
    {
        Result = XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        if (ConfigPtr->NumP4Elements > XIL_VITIS_NET_P4_TARGET_MGMT_MAX_NUM_ELEMENTS)
        {
            Result = XIL_VITIS_NET_P4_INTERRUPT_ERR_CONFIG_MAX_NUM_ELEMENTS_EXCEED;
        }
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        if (ConfigPtr->NumComponents > XIL_VITIS_NET_P4_TARGET_MGMT_MAX_NUM_FIFOS)
        {
            Result = XIL_VITIS_NET_P4_INTERRUPT_ERR_CONFIG_MAX_NUM_COMPONENTS_EXCEED;
        }
    }
    return Result;
}

static XilVitisNetP4ReturnType XilVitisNetP4TargetCtrlValidateCtx(XilVitisNetP4TargetCtrlPrivateCtx* CtxPtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;

    if (CtxPtr == NULL)
    {
        Result = XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        Result = XilVitisNetP4ValidateEnvIf(&(CtxPtr->EnvIf));
        if (Result != XIL_VITIS_NET_P4_SUCCESS)
        {
            Result = XIL_VITIS_NET_P4_GENERAL_ERR_INVALID_ENVIRONMENT_INTERFACE;
        }
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        if (CtxPtr->NumP4Elements > XIL_VITIS_NET_P4_TARGET_MGMT_MAX_NUM_ELEMENTS)
        {
            Result = XIL_VITIS_NET_P4_INTERRUPT_ERR_CONFIG_MAX_NUM_ELEMENTS_EXCEED;
        }
    }

    return Result;
}


