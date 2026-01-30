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
-- Description : Implementation of top level VitisNetP4 CAM driver
--
*/
/****************************************************************************************************************************************************/
/* SECTION: Header includes */
/****************************************************************************************************************************************************/

#include <string.h>
#include "cam_top.h"
#include "bcam.h"
#include "stcam.h"
#include "tcam.h"
#include "tiny_cam.h"
#include "cam_priv.h"


/****************************************************************************************************************************************************/
/* SECTION: Constants/macros */
/****************************************************************************************************************************************************/

#define XIL_VITIS_NET_P4_DCAM_MAX_RESP_SIZE_BITS (7168)

/* Register map */
#define XIL_VITIS_NET_P4_DCAM_CONTROL_STATUS_REG (0x00)
#define XIL_VITIS_NET_P4_DCAM_ADDRESS_REG (0x04)
#define XIL_VITIS_NET_P4_DCAM_ECC_CONTROL_REG (0x08)
#define XIL_VITIS_NET_P4_DCAM_SINGLE_BIT_ERROR_COUNT_REG (0x18)
#define XIL_VITIS_NET_P4_DCAM_DOUBLE_BIT_ERROR_COUNT_REG (0x1C)
#define XIL_VITIS_NET_P4_DCAM_SINGLE_BIT_LAST_ERROR_ADDR_REG (0x20)
#define XIL_VITIS_NET_P4_DCAM_DOUBLE_BIT_LAST_ERROR_ADDR_REG (0x24)

#define XIL_VITIS_NET_P4_DCAM_DATA_REG_1 (0x40)

#define XIL_VITIS_NET_P4_DCAM_MAX_RESP_SIZE_BYTES (XIL_VITIS_NET_P4_DCAM_MAX_RESP_SIZE_BITS / XIL_VITIS_NET_P4_BITS_PER_BYTE)

/* Control/status register bit masks */
#define XIL_VITIS_NET_P4_DCAM_READ_MASK (1UL << 0)
#define XIL_VITIS_NET_P4_DCAM_WRITE_MASK (1UL << 1)
#define XIL_VITIS_NET_P4_DCAM_HIT_MASK (1UL << 31)

/* Control register command definitions */
#define XIL_VITIS_NET_P4_DCAM_READ (XIL_VITIS_NET_P4_DCAM_READ_MASK)
#define XIL_VITIS_NET_P4_DCAM_WRITE_AND_CLEAR (XIL_VITIS_NET_P4_DCAM_WRITE_MASK)
#define XIL_VITIS_NET_P4_DCAM_WRITE_AND_HIT (XIL_VITIS_NET_P4_DCAM_WRITE_MASK | XIL_VITIS_NET_P4_DCAM_HIT_MASK)
#define XIL_VITIS_NET_P4_DCAM_HIT (XIL_VITIS_NET_P4_DCAM_HIT_MASK)

/* Conversion constants, used to avoid magic numbers in code */
#define XIL_VITIS_NET_P4_NUM_HZ_PER_MHZ  (1000000.0)



/****************************************************************************************************************************************************/
/* SECTION: Type defintions */
/****************************************************************************************************************************************************/

/****************************************************************************************************************************************************/
/* SECTION: Local function declarations */
/****************************************************************************************************************************************************/
static XilVitisNetP4ReturnType CamTopPrivCamInit(XilVitisNetP4CamCtx **CtxPtr, const XilVitisNetP4EnvIf *EnvIfPtr, XilVitisNetP4CamConfig *ConfigPtr, XilVitisNetP4CamType CamType, bool CalcHeapSize, ...);
static XilVitisNetP4ReturnType CamTopPrivDcamInitialise(XilVitisNetP4CamCtx** CtxPtr, const XilVitisNetP4EnvIf* EnvIfPtr, XilVitisNetP4CamConfig* ConfigPtr, bool CalcHeapSize, ...);
static XilVitisNetP4ReturnType CamTopPrivCamCtxCreateCamArg(XilVitisNetP4CamCtx* CtxPtr, bool CalcHeapSize, ...);
static void CamTopPrivCamCtxDestroyCamArg(XilVitisNetP4CamCtx *CtxPtr);
static XilVitisNetP4ReturnType CamTopPrivCamObfDrvSetConfig(XilVitisNetP4CamCtx *CtxPtr, XilVitisNetP4CamConfig *ConfigPtr, XilVitisNetP4CamType CamType, bool CalcHeapSize);
static XilVitisNetP4ReturnType CamTopPrivCamObfDrvCreate(XilVitisNetP4CamCtx *CtxPtr, XilVitisNetP4CamType CamType, bool CalcHeapSize, ...);
static XilVitisNetP4ReturnType CamTopPrivCamExtractNextFieldWidth(char **FormatStringPtr, uint32_t *FieldWidthPtr);
static XilVitisNetP4ReturnType CamTopPrivCamValidateFieldType(char FieldType);
static XilVitisNetP4ReturnType CamTopPrivCamConvertErrorCode(int ErrorCode);
static XilVitisNetP4ReturnType CamTopPrivCamConvertOptTypeToCamOptValue(XilVitisNetP4CamOptimizationType VitisNetP4OptType, uint32_t *CamOptValuePtr);
static XilVitisNetP4ReturnType CamTopPrivDcamValidateConfig(XilVitisNetP4CamConfig *ConfigPtr);
static XilVitisNetP4ReturnType CamTopPrivDcamValidateKey(XilVitisNetP4CamCtx *CtxPtr, uint32_t Key);
static XilVitisNetP4ReturnType CamTopPrivDcamValidateDataBuffer(XilVitisNetP4CamCtx *CtxPtr, uint8_t *DataPtr);
static XilVitisNetP4ReturnType CamTopPrivDcamGetEntryStatus(XilVitisNetP4CamCtx *CtxPtr, uint32_t Key);
static XilVitisNetP4ReturnType CamTopPrivDcamWriteEntry(XilVitisNetP4CamCtx *CtxPtr, uint32_t Key, uint8_t *DataPtr);
static XilVitisNetP4ReturnType CamTopPrivDcamReadEntry(XilVitisNetP4CamCtx *CtxPtr, uint32_t Key, uint8_t *DataPtr);
static XilVitisNetP4ReturnType CamTopPrivDcamSetEntryStatus(XilVitisNetP4CamCtx *CtxPtr, uint32_t Key, bool Status);
static XilVitisNetP4ReturnType CamTopPrivDcamCheckMatch(XilVitisNetP4CamCtx *CtxPtr,
                                                        uint8_t *ResponsePtr,
                                                        uint8_t *ResponseMaskPtr,
                                                        uint8_t *EntryPtr,
                                                        bool    *MatchFoundPtr);
static void CamTopPrivCamHwWrite(void* CamCtxPtr, uint32_t Offset, uint32_t Data);
static void CamTopPrivCamHwRead(void* CamCtxPtr, uint32_t Offset, uint32_t* DataPtr);
static void CamTopPrivCamLogError(void* CamCtxPtr, char* MessagePtr);
static void CamTopPrivCamLogInfo(void* CamCtxPtr, char* MessagePtr);

/****************************************************************************************************************************************************/
/* SECTION: Global function definitions - DCAM API */
/****************************************************************************************************************************************************/

XilVitisNetP4ReturnType XilVitisNetP4DcamInit(XilVitisNetP4DcamCtx *CtxPtr, const XilVitisNetP4EnvIf *EnvIfPtr, XilVitisNetP4CamConfig *ConfigPtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;


    /* Check input parameters */
    if (CtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    Result = XilVitisNetP4ValidateEnvIf(EnvIfPtr);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }

    Result = CamTopPrivDcamValidateConfig(ConfigPtr);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }

    /* Set up common CAM context for direct table */
    Result = CamTopPrivDcamInitialise(&(((XilVitisNetP4DcamCtx*) CtxPtr)->PrivateCtxPtr), EnvIfPtr, ConfigPtr, false);

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4DcamReset(XilVitisNetP4DcamCtx *CtxPtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    XilVitisNetP4CamCtx *PrivateCtxPtr = NULL;
    uint32_t TableEntry = 0;

    /* Check input parameters */
    if (CtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_INVALID_CONTEXT;
    }

    PrivateCtxPtr = CtxPtr->PrivateCtxPtr;

    Result = XilVitisNetP4CamValidateCtx(PrivateCtxPtr);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }

    /* Loop through all tables and set them equal to zero */
    for (TableEntry = 0; TableEntry < PrivateCtxPtr->NumEntries; TableEntry++)
    {
        Result = CamTopPrivDcamSetEntryStatus(PrivateCtxPtr, TableEntry, false);
        if (Result != XIL_VITIS_NET_P4_SUCCESS)
        {
            return Result;
        }
    }

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4DcamInsert(XilVitisNetP4DcamCtx *CtxPtr, uint32_t Key, uint8_t *ResponsePtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    XilVitisNetP4CamCtx *PrivateCtxPtr = NULL;

    /* Check input parameters */
    if (CtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_INVALID_CONTEXT;
    }

    PrivateCtxPtr = CtxPtr->PrivateCtxPtr;

    Result = XilVitisNetP4CamValidateCtx(PrivateCtxPtr);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }

    Result = CamTopPrivDcamValidateKey(PrivateCtxPtr, Key);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }

    Result = CamTopPrivDcamValidateDataBuffer(PrivateCtxPtr, ResponsePtr);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }

    /* Check that direct table entry exists */
    Result = CamTopPrivDcamGetEntryStatus(PrivateCtxPtr, Key);
    if (Result != XIL_VITIS_NET_P4_CAM_ERR_KEY_NOT_FOUND)
    {
        return Result;
    }

    /* Write direct table entry */
    Result = CamTopPrivDcamWriteEntry(PrivateCtxPtr, Key, ResponsePtr);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }

    /* Update entry status */
    Result = CamTopPrivDcamSetEntryStatus(PrivateCtxPtr, Key, true);
    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4DcamUpdate(XilVitisNetP4DcamCtx *CtxPtr, uint32_t Key, uint8_t *ResponsePtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    XilVitisNetP4CamCtx *PrivateCtxPtr = NULL;

    /* Check input parameters */
    if (CtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_INVALID_CONTEXT;
    }

    PrivateCtxPtr = CtxPtr->PrivateCtxPtr;

    Result = XilVitisNetP4CamValidateCtx(PrivateCtxPtr);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }

    Result = CamTopPrivDcamValidateKey(PrivateCtxPtr, Key);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }

    Result = CamTopPrivDcamValidateDataBuffer(PrivateCtxPtr, ResponsePtr);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }

    /* Check that direct table entry exists - if so, update */
    Result = CamTopPrivDcamGetEntryStatus(PrivateCtxPtr, Key);
    if (Result != XIL_VITIS_NET_P4_CAM_ERR_DUPLICATE_FOUND)
    {
        return Result;
    }

    Result = CamTopPrivDcamWriteEntry(PrivateCtxPtr, Key, ResponsePtr);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }

    /* Updating the entry status ensures DCAM entry is written */
    Result = CamTopPrivDcamSetEntryStatus(PrivateCtxPtr, Key, true);
    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4DcamGetByResponse(XilVitisNetP4DcamCtx *CtxPtr,
                                             uint8_t *ResponsePtr,
                                             uint8_t *ResponseMaskPtr,
                                             uint32_t *PositionPtr,
                                             uint32_t *KeyPtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    XilVitisNetP4CamCtx *PrivateCtxPtr = NULL;
    uint32_t Position;
    bool MatchFound = false;
    uint8_t ReadResponse[XIL_VITIS_NET_P4_DCAM_MAX_RESP_SIZE_BYTES] = {0};

    /* Check input parameters */
    if (CtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_INVALID_CONTEXT;
    }

    PrivateCtxPtr = CtxPtr->PrivateCtxPtr;

    Result = XilVitisNetP4CamValidateCtx(PrivateCtxPtr);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }

    Result = CamTopPrivDcamValidateDataBuffer(PrivateCtxPtr, ResponsePtr);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }

    Result = CamTopPrivDcamValidateDataBuffer(PrivateCtxPtr, ResponseMaskPtr);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }

    if (PositionPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    if (KeyPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    Position = *PositionPtr;

    Result = CamTopPrivDcamValidateKey(PrivateCtxPtr, Position);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }

    while (Position < PrivateCtxPtr->NumEntries)
    {
        /* Not checking for failure as some entries may be deleted causing failure (not found), but this may not be the last entry in the table */
        Result = XilVitisNetP4DcamLookup(CtxPtr, Position, ReadResponse);

        /* If table lookup succeeded, check if what was found matches what we're looking for */
        if (Result == XIL_VITIS_NET_P4_SUCCESS)
        {
            Result = CamTopPrivDcamCheckMatch(PrivateCtxPtr, ResponsePtr, ResponseMaskPtr, ReadResponse, &MatchFound);
            if (Result != XIL_VITIS_NET_P4_SUCCESS)
            {
                return Result;
            }
        }

        /* If we have a match - stop searching */
        if (MatchFound == true)
        {
            break;
        }

        Position++;
    }

    if (Result == XIL_VITIS_NET_P4_CAM_ERR_INVALID_KEY)
    {
        *PositionPtr = PrivateCtxPtr->NumEntries;
        Result = XIL_VITIS_NET_P4_CAM_ERR_LOOKUP_NOT_FOUND;
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        if (MatchFound == true)
        {
            *KeyPtr = Position;
            *PositionPtr = Position;
        }
        else
        {
            Result = XIL_VITIS_NET_P4_CAM_ERR_LOOKUP_NOT_FOUND;
        }
    }

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4DcamLookup(XilVitisNetP4DcamCtx *CtxPtr, uint32_t Key, uint8_t *ResponsePtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    XilVitisNetP4CamCtx *PrivateCtxPtr = NULL;

    /* Check input parameters */
    if (CtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_INVALID_CONTEXT;
    }

    PrivateCtxPtr = CtxPtr->PrivateCtxPtr;

    /* Checks */
    Result = XilVitisNetP4CamValidateCtx(PrivateCtxPtr);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }

    Result = CamTopPrivDcamValidateKey(PrivateCtxPtr, Key);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }

    Result = CamTopPrivDcamValidateDataBuffer(PrivateCtxPtr, ResponsePtr);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }

    /* Check that direct table entry exists */
    Result = CamTopPrivDcamGetEntryStatus(PrivateCtxPtr, Key);
    if (Result != XIL_VITIS_NET_P4_CAM_ERR_DUPLICATE_FOUND)
    {
        return Result;
    }

    Result = CamTopPrivDcamReadEntry(PrivateCtxPtr, Key, ResponsePtr);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4DcamDelete(XilVitisNetP4DcamCtx *CtxPtr, uint32_t Key)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    XilVitisNetP4CamCtx *PrivateCtxPtr = NULL;

    /* Check input parameters */
    if (CtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_INVALID_CONTEXT;
    }

    PrivateCtxPtr = CtxPtr->PrivateCtxPtr;

    Result = XilVitisNetP4CamValidateCtx(PrivateCtxPtr);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }

    Result = CamTopPrivDcamValidateKey(PrivateCtxPtr, Key);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }

    /* Check that direct table entry exists - if so, delete it */
    Result = CamTopPrivDcamGetEntryStatus(PrivateCtxPtr, Key);
    if (Result == XIL_VITIS_NET_P4_CAM_ERR_DUPLICATE_FOUND)
    {
        Result = CamTopPrivDcamSetEntryStatus(PrivateCtxPtr, Key, false);
    }

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4DcamGetEccCountersClearOnRead(XilVitisNetP4DcamCtx *CtxPtr,
                                                         uint32_t *CorrectedSingleBitErrorsPtr,
                                                         uint32_t *DetectedDoubleBitErrorsPtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    XilVitisNetP4CamCtx *PrivateCtxPtr = NULL;

    if ((CtxPtr == NULL) || (CorrectedSingleBitErrorsPtr == NULL) || (DetectedDoubleBitErrorsPtr == NULL))
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    if (CtxPtr->PrivateCtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_INVALID_CONTEXT;
    }

    PrivateCtxPtr = CtxPtr->PrivateCtxPtr;

    Result = PrivateCtxPtr->EnvIf.WordRead32(&(PrivateCtxPtr->EnvIf),
                                             (PrivateCtxPtr->BaseAddr + XIL_VITIS_NET_P4_DCAM_SINGLE_BIT_ERROR_COUNT_REG),
                                             CorrectedSingleBitErrorsPtr);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }

    Result = PrivateCtxPtr->EnvIf.WordRead32(&(PrivateCtxPtr->EnvIf),
                                             (PrivateCtxPtr->BaseAddr + XIL_VITIS_NET_P4_DCAM_DOUBLE_BIT_ERROR_COUNT_REG),
                                             DetectedDoubleBitErrorsPtr);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4DcamGetEccAddressesClearOnRead(XilVitisNetP4DcamCtx *CtxPtr,
                                                          uint32_t *FailingAddressSingleBitErrorPtr,
                                                          uint32_t *FailingAddressDoubleBitErrorPtr)
{
    XilVitisNetP4ReturnType Result ;
    XilVitisNetP4CamCtx *PrivateCtxPtr = NULL;

    if ((CtxPtr == NULL) || (FailingAddressSingleBitErrorPtr == NULL) || (FailingAddressDoubleBitErrorPtr == NULL))
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    if (CtxPtr->PrivateCtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_INVALID_CONTEXT;
    }

    PrivateCtxPtr = CtxPtr->PrivateCtxPtr;

    Result = PrivateCtxPtr->EnvIf.WordRead32(&(PrivateCtxPtr->EnvIf),
                                             (PrivateCtxPtr->BaseAddr + XIL_VITIS_NET_P4_DCAM_SINGLE_BIT_LAST_ERROR_ADDR_REG),
                                             FailingAddressSingleBitErrorPtr);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }

    Result = PrivateCtxPtr->EnvIf.WordRead32(&(PrivateCtxPtr->EnvIf),
                                             (PrivateCtxPtr->BaseAddr + XIL_VITIS_NET_P4_DCAM_DOUBLE_BIT_LAST_ERROR_ADDR_REG),
                                             FailingAddressDoubleBitErrorPtr);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4DcamExit(XilVitisNetP4DcamCtx *CtxPtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    XilVitisNetP4CamCtx *PrivateCtxPtr = NULL;

    /* Check input parameters */
    if (CtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_INVALID_CONTEXT;
    }

    PrivateCtxPtr = CtxPtr->PrivateCtxPtr;

    Result = XilVitisNetP4CamValidateCtx(PrivateCtxPtr);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }

    free(CtxPtr->PrivateCtxPtr);
    CtxPtr->PrivateCtxPtr = NULL;

    return Result;
}

/****************************************************************************************************************************************************/
/* SECTION: Global function definitions - BCAM API */
/****************************************************************************************************************************************************/

XilVitisNetP4ReturnType XilVitisNetP4BcamInit(XilVitisNetP4BcamCtx *CtxPtr, const XilVitisNetP4EnvIf *EnvIfPtr, XilVitisNetP4CamConfig *ConfigPtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;

    if ((CtxPtr == NULL) || (EnvIfPtr == NULL) || (ConfigPtr == NULL))
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    Result = CamTopPrivCamInit(&(CtxPtr->PrivateCtxPtr), EnvIfPtr, ConfigPtr, XIL_VITIS_NET_P4_CAM_BCAM, false);

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4BcamReset(XilVitisNetP4BcamCtx *CtxPtr)
{
    int CamResult = CAM_OK;
    uint32_t Position = 0;
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    uint8_t *KeyBufPtr;
    uint8_t *ResponsePtr;
    uint8_t *ResponseMaskPtr;

    if (CtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    if (CtxPtr->PrivateCtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_INVALID_CONTEXT;
    }

    KeyBufPtr = calloc(1, CtxPtr->PrivateCtxPtr->KeyBufSizeBytes);
    if (KeyBufPtr == NULL)
    {
        Result = XIL_VITIS_NET_P4_CAM_ERR_MALLOC_FAILED;
        goto exit_func;
    }

    ResponsePtr = calloc(1, CtxPtr->PrivateCtxPtr->ResponseBufSizeBytes);
    if (ResponsePtr == NULL)
    {
        Result = XIL_VITIS_NET_P4_CAM_ERR_MALLOC_FAILED;
        goto exit_free_key;
    }

    ResponseMaskPtr = calloc(1, CtxPtr->PrivateCtxPtr->ResponseBufSizeBytes);
    if (ResponseMaskPtr == NULL)
    {
        Result = XIL_VITIS_NET_P4_CAM_ERR_MALLOC_FAILED;
        goto exit_free_response;
    }

    /*
     * Using a ResponseMask of all zeros means get_by_response will find all
     *  keys present
     */
    while (1)
    {
        CamResult = bcam_get_by_response(CtxPtr->PrivateCtxPtr->CamUnion.BcamPtr, ResponsePtr, ResponseMaskPtr, &Position, KeyBufPtr);

        if (CamResult == CAM_ERROR_KEY_NOT_FOUND)
        {
            /* Table has been successfully emptied */
            CamResult = CAM_OK;
            break;
        }
        if (CamResult != 0)
        {
            break;
        }

        CamResult = bcam_delete(CtxPtr->PrivateCtxPtr->CamUnion.BcamPtr, KeyBufPtr);
        if (CamResult != 0)
        {
            break;
        }
    }
    Result = CamTopPrivCamConvertErrorCode(CamResult);

    free(ResponseMaskPtr);
exit_free_response:
    free(ResponsePtr);
exit_free_key:
    free(KeyBufPtr);
exit_func:
    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4BcamInsert(XilVitisNetP4BcamCtx *CtxPtr, uint8_t *KeyPtr, uint8_t *ResponsePtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    int CamResult = CAM_OK;
    uint8_t *KeyParamPtr = KeyPtr;
    uint8_t *ResponseParamPtr = ResponsePtr;

    if ((CtxPtr == NULL) || (KeyPtr == NULL) || (ResponsePtr == NULL))
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    if (CtxPtr->PrivateCtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_INVALID_CONTEXT;
    }

    if (CtxPtr->PrivateCtxPtr->Endian == XIL_VITIS_NET_P4_BIG_ENDIAN)
    {
        KeyParamPtr = CtxPtr->PrivateCtxPtr->KeyBufPtr;
        Result = XilVitisNetP4CamByteSwap(CtxPtr->PrivateCtxPtr, KeyPtr, KeyParamPtr, VITISNETP4_CAM_SWAP_KEY);
        if (Result == XIL_VITIS_NET_P4_SUCCESS)
        {
            ResponseParamPtr = CtxPtr->PrivateCtxPtr->ResponseBufPtr;
            Result = XilVitisNetP4CamByteSwap(CtxPtr->PrivateCtxPtr, ResponsePtr, ResponseParamPtr, VITISNETP4_CAM_SWAP_RESPONSE);
        }
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        CamResult = bcam_insert(CtxPtr->PrivateCtxPtr->CamUnion.BcamPtr, KeyParamPtr, ResponseParamPtr);
        Result = CamTopPrivCamConvertErrorCode(CamResult);
    }

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4BcamUpdate(XilVitisNetP4BcamCtx *CtxPtr, uint8_t *KeyPtr, uint8_t *ResponsePtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    int CamResult = CAM_OK;
    uint8_t *KeyParamPtr = KeyPtr;
    uint8_t *ResponseParamPtr = ResponsePtr;

    if ((CtxPtr == NULL) || (KeyPtr == NULL) || (ResponsePtr == NULL))
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    if (CtxPtr->PrivateCtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_INVALID_CONTEXT;
    }

    if (CtxPtr->PrivateCtxPtr->Endian == XIL_VITIS_NET_P4_BIG_ENDIAN)
    {
        KeyParamPtr = CtxPtr->PrivateCtxPtr->KeyBufPtr;
        Result = XilVitisNetP4CamByteSwap(CtxPtr->PrivateCtxPtr, KeyPtr, KeyParamPtr, VITISNETP4_CAM_SWAP_KEY);
        if (Result == XIL_VITIS_NET_P4_SUCCESS)
        {
            ResponseParamPtr = CtxPtr->PrivateCtxPtr->ResponseBufPtr;
            Result = XilVitisNetP4CamByteSwap(CtxPtr->PrivateCtxPtr, ResponsePtr, ResponseParamPtr, VITISNETP4_CAM_SWAP_RESPONSE);
        }
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        CamResult = bcam_update(CtxPtr->PrivateCtxPtr->CamUnion.BcamPtr, KeyParamPtr, ResponseParamPtr);
        Result = CamTopPrivCamConvertErrorCode(CamResult);
    }

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4BcamGetByResponse(XilVitisNetP4BcamCtx *CtxPtr,
                                             uint8_t *ResponsePtr,
                                             uint8_t *ResponseMaskPtr,
                                             uint32_t *PositionPtr,
                                             uint8_t *KeyPtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    int CamResult = CAM_OK;
    uint8_t *ResponseParamPtr = ResponsePtr;
    uint8_t *ResponseMaskParamPtr = ResponseMaskPtr;
    uint8_t *KeyParamPtr = KeyPtr;

    if ((CtxPtr == NULL) || (ResponsePtr == NULL) || (ResponseMaskPtr == NULL) || (PositionPtr == NULL) || (KeyPtr == NULL))
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    if (CtxPtr->PrivateCtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_INVALID_CONTEXT;
    }

    if (CtxPtr->PrivateCtxPtr->Endian == XIL_VITIS_NET_P4_BIG_ENDIAN)
    {
        KeyParamPtr = CtxPtr->PrivateCtxPtr->KeyBufPtr;

        ResponseParamPtr = CtxPtr->PrivateCtxPtr->ResponseBufPtr;
        Result = XilVitisNetP4CamByteSwap(CtxPtr->PrivateCtxPtr, ResponsePtr, ResponseParamPtr, VITISNETP4_CAM_SWAP_RESPONSE);
        if (Result == XIL_VITIS_NET_P4_SUCCESS)
        {
            ResponseMaskParamPtr = CtxPtr->PrivateCtxPtr->ResponseMaskBufPtr;
            Result = XilVitisNetP4CamByteSwap(CtxPtr->PrivateCtxPtr, ResponseMaskPtr, ResponseMaskParamPtr, VITISNETP4_CAM_SWAP_RESPONSE_MASK);
        }
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        CamResult = bcam_get_by_response(CtxPtr->PrivateCtxPtr->CamUnion.BcamPtr, ResponseParamPtr, ResponseMaskParamPtr, PositionPtr, KeyParamPtr);
        Result = CamTopPrivCamConvertErrorCode(CamResult);
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        if (CtxPtr->PrivateCtxPtr->Endian == XIL_VITIS_NET_P4_BIG_ENDIAN)
        {
            Result = XilVitisNetP4CamByteSwap(CtxPtr->PrivateCtxPtr, KeyParamPtr, KeyPtr, VITISNETP4_CAM_SWAP_KEY);
        }
    }

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4BcamGetByKey(XilVitisNetP4BcamCtx *CtxPtr, uint8_t *KeyPtr, uint8_t *ResponsePtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    int CamResult = CAM_OK;
    uint8_t *KeyParamPtr = KeyPtr;
    uint8_t *ResponseParamPtr = ResponsePtr;

    if ((CtxPtr == NULL) || (KeyPtr == NULL) || (ResponsePtr == NULL))
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    if (CtxPtr->PrivateCtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_INVALID_CONTEXT;
    }

    if (CtxPtr->PrivateCtxPtr->Endian == XIL_VITIS_NET_P4_BIG_ENDIAN)
    {
        ResponseParamPtr = CtxPtr->PrivateCtxPtr->ResponseBufPtr;

        KeyParamPtr = CtxPtr->PrivateCtxPtr->KeyBufPtr;
        Result = XilVitisNetP4CamByteSwap(CtxPtr->PrivateCtxPtr, KeyPtr, KeyParamPtr, VITISNETP4_CAM_SWAP_KEY);
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        CamResult = bcam_get_by_key(CtxPtr->PrivateCtxPtr->CamUnion.BcamPtr, KeyParamPtr, ResponseParamPtr);
        Result = CamTopPrivCamConvertErrorCode(CamResult);
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        if (CtxPtr->PrivateCtxPtr->Endian == XIL_VITIS_NET_P4_BIG_ENDIAN)
        {
            Result = XilVitisNetP4CamByteSwap(CtxPtr->PrivateCtxPtr, ResponseParamPtr, ResponsePtr, VITISNETP4_CAM_SWAP_RESPONSE);
        }
    }

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4BcamDelete(XilVitisNetP4BcamCtx *CtxPtr, uint8_t *KeyPtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    int CamResult = CAM_OK;
    uint8_t *KeyParamPtr = KeyPtr;

    if ((CtxPtr == NULL) || (KeyPtr == NULL))
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    if (CtxPtr->PrivateCtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_INVALID_CONTEXT;
    }

    if (CtxPtr->PrivateCtxPtr->Endian == XIL_VITIS_NET_P4_BIG_ENDIAN)
    {
        KeyParamPtr = CtxPtr->PrivateCtxPtr->KeyBufPtr;
        Result = XilVitisNetP4CamByteSwap(CtxPtr->PrivateCtxPtr, KeyPtr, KeyParamPtr, VITISNETP4_CAM_SWAP_KEY);
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        CamResult = bcam_delete(CtxPtr->PrivateCtxPtr->CamUnion.BcamPtr, KeyParamPtr);
        Result = CamTopPrivCamConvertErrorCode(CamResult);
    }

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4BcamGetEccCountersClearOnRead(XilVitisNetP4BcamCtx *CtxPtr,
                                                         uint32_t *CorrectedSingleBitErrorsPtr,
                                                         uint32_t *DetectedDoubleBitErrorsPtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    int CamResult = CAM_OK;

    if ((CtxPtr == NULL) || (CorrectedSingleBitErrorsPtr == NULL) || (DetectedDoubleBitErrorsPtr == NULL))
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    if (CtxPtr->PrivateCtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_INVALID_CONTEXT;
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        CamResult
            = bcam_read_and_clear_ecc_counters(CtxPtr->PrivateCtxPtr->CamUnion.BcamPtr, CorrectedSingleBitErrorsPtr, DetectedDoubleBitErrorsPtr);
        Result = CamTopPrivCamConvertErrorCode(CamResult);
    }

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4BcamGetEccAddressesClearOnRead(XilVitisNetP4BcamCtx *CtxPtr,
                                                          uint32_t *FailingAddressSingleBitErrorPtr,
                                                          uint32_t *FailingAddressDoubleBitErrorPtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    int CamResult = CAM_OK;

    if ((CtxPtr == NULL) || (FailingAddressSingleBitErrorPtr == NULL) || (FailingAddressDoubleBitErrorPtr == NULL))
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    if (CtxPtr->PrivateCtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_INVALID_CONTEXT;
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        CamResult = bcam_read_and_clear_ecc_addresses(CtxPtr->PrivateCtxPtr->CamUnion.BcamPtr,
                                                      FailingAddressSingleBitErrorPtr,
                                                      FailingAddressDoubleBitErrorPtr);
        Result = CamTopPrivCamConvertErrorCode(CamResult);
    }

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4BcamExit(XilVitisNetP4BcamCtx *CtxPtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;

    if (CtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    if (CtxPtr->PrivateCtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_INVALID_CONTEXT;
    }

    bcam_destroy(CtxPtr->PrivateCtxPtr->CamUnion.BcamPtr);
    CtxPtr->PrivateCtxPtr->CamUnion.BcamPtr = NULL;

    CamTopPrivCamCtxDestroyCamArg(CtxPtr->PrivateCtxPtr);
    Result = XilVitisNetP4CamCtxDestroy(CtxPtr->PrivateCtxPtr);

    return Result;
}

/****************************************************************************************************************************************************/
/* SECTION: Global function definitions - TCAM API */
/****************************************************************************************************************************************************/

XilVitisNetP4ReturnType XilVitisNetP4TcamInit(XilVitisNetP4TcamCtx *CtxPtr, const XilVitisNetP4EnvIf *EnvIfPtr, XilVitisNetP4CamConfig *ConfigPtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;

    if ((CtxPtr == NULL) || (EnvIfPtr == NULL) || (ConfigPtr == NULL))
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    Result = CamTopPrivCamInit(&(CtxPtr->PrivateCtxPtr), EnvIfPtr, ConfigPtr, XIL_VITIS_NET_P4_CAM_TCAM, false);

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4TcamReset(XilVitisNetP4TcamCtx *CtxPtr)
{
    int CamResult = CAM_OK;
    uint32_t Position = 0;
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    uint8_t *KeyBufPtr;
    uint8_t *MaskBufPtr;
    uint8_t *ResponsePtr;
    uint8_t *ResponseMaskPtr;

    if (CtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    if (CtxPtr->PrivateCtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_INVALID_CONTEXT;
    }

    KeyBufPtr = calloc(1, CtxPtr->PrivateCtxPtr->KeyBufSizeBytes);
    if (KeyBufPtr == NULL)
    {
        Result = XIL_VITIS_NET_P4_CAM_ERR_MALLOC_FAILED;
        goto exit_func;
    }

    MaskBufPtr = calloc(1, CtxPtr->PrivateCtxPtr->KeyBufSizeBytes);
    if (MaskBufPtr == NULL)
    {
        Result = XIL_VITIS_NET_P4_CAM_ERR_MALLOC_FAILED;
        goto exit_free_key;
    }

    ResponsePtr = calloc(1, CtxPtr->PrivateCtxPtr->ResponseBufSizeBytes);
    if (ResponsePtr == NULL)
    {
        Result = XIL_VITIS_NET_P4_CAM_ERR_MALLOC_FAILED;
        goto exit_free_mask;
    }

    ResponseMaskPtr = calloc(1, CtxPtr->PrivateCtxPtr->ResponseBufSizeBytes);
    if (ResponseMaskPtr == NULL)
    {
        Result = XIL_VITIS_NET_P4_CAM_ERR_MALLOC_FAILED;
        goto exit_free_response;
    }

    /*
     * Using a ResponseMask of all zeros means get_by_response will find all
     *  keys present
     */
    while (1)
    {
        CamResult = tcam_get_by_response(CtxPtr->PrivateCtxPtr->CamUnion.TcamPtr, ResponsePtr, ResponseMaskPtr, &Position, KeyBufPtr, MaskBufPtr);

        if (CamResult == CAM_ERROR_KEY_NOT_FOUND)
        {
            /* Table has been successfully emptied */
            CamResult = CAM_OK;
            break;
        }
        if (CamResult != 0)
        {
            break;
        }

        CamResult = tcam_delete(CtxPtr->PrivateCtxPtr->CamUnion.TcamPtr, KeyBufPtr, MaskBufPtr);
        if (CamResult != 0)
        {
            break;
        }
    }
    Result = CamTopPrivCamConvertErrorCode(CamResult);

    free(ResponseMaskPtr);
exit_free_response:
    free(ResponsePtr);
exit_free_mask:
    free(MaskBufPtr);
exit_free_key:
    free(KeyBufPtr);
exit_func:

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4TcamInsert(XilVitisNetP4TcamCtx *CtxPtr, uint8_t *KeyPtr, uint8_t *MaskPtr, uint32_t Priority, uint8_t *ResponsePtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    int CamResult = CAM_OK;
    uint8_t *KeyParamPtr = KeyPtr;
    uint8_t *MaskParamPtr = MaskPtr;
    uint8_t *ResponseParamPtr = ResponsePtr;

    if ((CtxPtr == NULL) || (KeyPtr == NULL) || (MaskPtr == NULL) || (ResponsePtr == NULL))
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    if (CtxPtr->PrivateCtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_INVALID_CONTEXT;
    }

    if (CtxPtr->PrivateCtxPtr->Endian == XIL_VITIS_NET_P4_BIG_ENDIAN)
    {
        KeyParamPtr = CtxPtr->PrivateCtxPtr->KeyBufPtr;
        Result = XilVitisNetP4CamByteSwap(CtxPtr->PrivateCtxPtr, KeyPtr, KeyParamPtr, VITISNETP4_CAM_SWAP_KEY);
        if (Result == XIL_VITIS_NET_P4_SUCCESS)
        {
            MaskParamPtr = CtxPtr->PrivateCtxPtr->MaskBufPtr;
            Result = XilVitisNetP4CamByteSwap(CtxPtr->PrivateCtxPtr, MaskPtr, MaskParamPtr, VITISNETP4_CAM_SWAP_KEY_MASK);
            if (Result == XIL_VITIS_NET_P4_SUCCESS)
            {
                ResponseParamPtr = CtxPtr->PrivateCtxPtr->ResponseBufPtr;
                Result = XilVitisNetP4CamByteSwap(CtxPtr->PrivateCtxPtr, ResponsePtr, ResponseParamPtr, VITISNETP4_CAM_SWAP_RESPONSE);
            }
        }
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        CamResult = tcam_insert(CtxPtr->PrivateCtxPtr->CamUnion.TcamPtr, KeyParamPtr, MaskParamPtr, Priority, ResponseParamPtr);
        Result = CamTopPrivCamConvertErrorCode(CamResult);
    }

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4TcamUpdate(XilVitisNetP4TcamCtx *CtxPtr, uint8_t *KeyPtr, uint8_t *MaskPtr, uint8_t *ResponsePtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    int CamResult = CAM_OK;
    uint8_t *KeyParamPtr = KeyPtr;
    uint8_t *MaskParamPtr = MaskPtr;
    uint8_t *ResponseParamPtr = ResponsePtr;

    if ((CtxPtr == NULL) || (KeyPtr == NULL) || (MaskPtr == NULL) || (ResponsePtr == NULL))
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    if (CtxPtr->PrivateCtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_INVALID_CONTEXT;
    }

    if (CtxPtr->PrivateCtxPtr->Endian == XIL_VITIS_NET_P4_BIG_ENDIAN)
    {
        KeyParamPtr = CtxPtr->PrivateCtxPtr->KeyBufPtr;
        Result = XilVitisNetP4CamByteSwap(CtxPtr->PrivateCtxPtr, KeyPtr, KeyParamPtr, VITISNETP4_CAM_SWAP_KEY);
        if (Result == XIL_VITIS_NET_P4_SUCCESS)
        {
            MaskParamPtr = CtxPtr->PrivateCtxPtr->MaskBufPtr;
            Result = XilVitisNetP4CamByteSwap(CtxPtr->PrivateCtxPtr, MaskPtr, MaskParamPtr, VITISNETP4_CAM_SWAP_KEY_MASK);
            if (Result == XIL_VITIS_NET_P4_SUCCESS)
            {
                ResponseParamPtr = CtxPtr->PrivateCtxPtr->ResponseBufPtr;
                Result = XilVitisNetP4CamByteSwap(CtxPtr->PrivateCtxPtr, ResponsePtr, ResponseParamPtr, VITISNETP4_CAM_SWAP_RESPONSE);
            }
        }
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        CamResult = tcam_update(CtxPtr->PrivateCtxPtr->CamUnion.TcamPtr, KeyParamPtr, MaskParamPtr, ResponseParamPtr);
        Result = CamTopPrivCamConvertErrorCode(CamResult);
    }

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4TcamGetByResponse(XilVitisNetP4TcamCtx *CtxPtr,
                                             uint8_t *ResponsePtr,
                                             uint8_t *ResponseMaskPtr,
                                             uint32_t *PositionPtr,
                                             uint8_t *KeyPtr,
                                             uint8_t *MaskPtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    int CamResult = CAM_OK;
    uint8_t *KeyParamPtr = KeyPtr;
    uint8_t *MaskParamPtr = MaskPtr;
    uint8_t *ResponseParamPtr = ResponsePtr;
    uint8_t *ResponseMaskParamPtr = ResponseMaskPtr;

    if ((CtxPtr == NULL) || (ResponsePtr == NULL) || (ResponseMaskPtr == NULL) || (PositionPtr == NULL) || (KeyPtr == NULL) || (MaskPtr == NULL))
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    if (CtxPtr->PrivateCtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_INVALID_CONTEXT;
    }

    if (CtxPtr->PrivateCtxPtr->Endian == XIL_VITIS_NET_P4_BIG_ENDIAN)
    {
        KeyParamPtr = CtxPtr->PrivateCtxPtr->KeyBufPtr;
        MaskParamPtr = CtxPtr->PrivateCtxPtr->MaskBufPtr;

        ResponseParamPtr = CtxPtr->PrivateCtxPtr->ResponseBufPtr;
        Result = XilVitisNetP4CamByteSwap(CtxPtr->PrivateCtxPtr, ResponsePtr, ResponseParamPtr, VITISNETP4_CAM_SWAP_RESPONSE);
        if (Result == XIL_VITIS_NET_P4_SUCCESS)
        {
            ResponseMaskParamPtr = CtxPtr->PrivateCtxPtr->ResponseMaskBufPtr;
            Result = XilVitisNetP4CamByteSwap(CtxPtr->PrivateCtxPtr, ResponseMaskPtr, ResponseMaskParamPtr, VITISNETP4_CAM_SWAP_RESPONSE_MASK);
        }
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        CamResult = tcam_get_by_response(CtxPtr->PrivateCtxPtr->CamUnion.TcamPtr,
                                         ResponseParamPtr,
                                         ResponseMaskParamPtr,
                                         PositionPtr,
                                         KeyParamPtr,
                                         MaskParamPtr);
        Result = CamTopPrivCamConvertErrorCode(CamResult);
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        if (CtxPtr->PrivateCtxPtr->Endian == XIL_VITIS_NET_P4_BIG_ENDIAN)
        {
            Result = XilVitisNetP4CamByteSwap(CtxPtr->PrivateCtxPtr, KeyParamPtr, KeyPtr, VITISNETP4_CAM_SWAP_KEY);
            if (Result == XIL_VITIS_NET_P4_SUCCESS)
            {
                Result = XilVitisNetP4CamByteSwap(CtxPtr->PrivateCtxPtr, MaskParamPtr, MaskPtr, VITISNETP4_CAM_SWAP_KEY_MASK);
            }
        }
    }

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4TcamGetByKey(XilVitisNetP4TcamCtx *CtxPtr, uint8_t *KeyPtr, uint8_t *MaskPtr, uint32_t *PriorityPtr, uint8_t *ResponsePtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    int CamResult = CAM_OK;
    uint8_t *KeyParamPtr = KeyPtr;
    uint8_t *MaskParamPtr = MaskPtr;
    uint8_t *ResponseParamPtr = ResponsePtr;

    if ((CtxPtr == NULL) || (KeyPtr == NULL) || (MaskPtr == NULL) || (PriorityPtr == NULL) || (ResponsePtr == NULL))
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    if (CtxPtr->PrivateCtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_INVALID_CONTEXT;
    }

    if (CtxPtr->PrivateCtxPtr->Endian == XIL_VITIS_NET_P4_BIG_ENDIAN)
    {
        ResponseParamPtr = CtxPtr->PrivateCtxPtr->ResponseBufPtr;

        KeyParamPtr = CtxPtr->PrivateCtxPtr->KeyBufPtr;
        Result = XilVitisNetP4CamByteSwap(CtxPtr->PrivateCtxPtr, KeyPtr, KeyParamPtr, VITISNETP4_CAM_SWAP_KEY);
        if (Result == XIL_VITIS_NET_P4_SUCCESS)
        {
            MaskParamPtr = CtxPtr->PrivateCtxPtr->MaskBufPtr;
            Result = XilVitisNetP4CamByteSwap(CtxPtr->PrivateCtxPtr, MaskPtr, MaskParamPtr, VITISNETP4_CAM_SWAP_KEY_MASK);
        }
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        CamResult = tcam_get_by_key(CtxPtr->PrivateCtxPtr->CamUnion.TcamPtr, KeyParamPtr, MaskParamPtr, PriorityPtr, ResponseParamPtr);
        Result = CamTopPrivCamConvertErrorCode(CamResult);
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        if (CtxPtr->PrivateCtxPtr->Endian == XIL_VITIS_NET_P4_BIG_ENDIAN)
        {
            Result = XilVitisNetP4CamByteSwap(CtxPtr->PrivateCtxPtr, ResponseParamPtr, ResponsePtr, VITISNETP4_CAM_SWAP_RESPONSE);
        }
    }

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4TcamLookup(XilVitisNetP4TcamCtx *CtxPtr, uint8_t *KeyPtr, uint8_t *ResponsePtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    int CamResult = CAM_OK;
    uint8_t *KeyParamPtr = KeyPtr;
    uint8_t *ResponseParamPtr = ResponsePtr;

    if ((CtxPtr == NULL) || (KeyPtr == NULL) || (ResponsePtr == NULL))
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    if (CtxPtr->PrivateCtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_INVALID_CONTEXT;
    }

    if (CtxPtr->PrivateCtxPtr->Endian == XIL_VITIS_NET_P4_BIG_ENDIAN)
    {
        ResponseParamPtr = CtxPtr->PrivateCtxPtr->ResponseBufPtr;

        KeyParamPtr = CtxPtr->PrivateCtxPtr->KeyBufPtr;
        Result = XilVitisNetP4CamByteSwap(CtxPtr->PrivateCtxPtr, KeyPtr, KeyParamPtr, VITISNETP4_CAM_SWAP_KEY);
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        CamResult = tcam_lookup(CtxPtr->PrivateCtxPtr->CamUnion.TcamPtr, KeyParamPtr, ResponseParamPtr);
        Result = CamTopPrivCamConvertErrorCode(CamResult);
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        if (CtxPtr->PrivateCtxPtr->Endian == XIL_VITIS_NET_P4_BIG_ENDIAN)
        {
            Result = XilVitisNetP4CamByteSwap(CtxPtr->PrivateCtxPtr, ResponseParamPtr, ResponsePtr, VITISNETP4_CAM_SWAP_RESPONSE);
        }
    }

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4TcamDelete(XilVitisNetP4TcamCtx *CtxPtr, uint8_t *KeyPtr, uint8_t *MaskPtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    int CamResult = CAM_OK;
    uint8_t *KeyParamPtr = KeyPtr;
    uint8_t *MaskParamPtr = MaskPtr;

    if ((CtxPtr == NULL) || (KeyPtr == NULL) || (MaskPtr == NULL))
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    if (CtxPtr->PrivateCtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_INVALID_CONTEXT;
    }

    if (CtxPtr->PrivateCtxPtr->Endian == XIL_VITIS_NET_P4_BIG_ENDIAN)
    {
        KeyParamPtr = CtxPtr->PrivateCtxPtr->KeyBufPtr;
        Result = XilVitisNetP4CamByteSwap(CtxPtr->PrivateCtxPtr, KeyPtr, KeyParamPtr, VITISNETP4_CAM_SWAP_KEY);
        if (Result == XIL_VITIS_NET_P4_SUCCESS)
        {
            MaskParamPtr = CtxPtr->PrivateCtxPtr->MaskBufPtr;
            Result = XilVitisNetP4CamByteSwap(CtxPtr->PrivateCtxPtr, MaskPtr, MaskParamPtr, VITISNETP4_CAM_SWAP_KEY_MASK);
        }
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        CamResult = tcam_delete(CtxPtr->PrivateCtxPtr->CamUnion.TcamPtr, KeyParamPtr, MaskParamPtr);
        Result = CamTopPrivCamConvertErrorCode(CamResult);
    }

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4TcamGetEccCountersClearOnRead(XilVitisNetP4TcamCtx *CtxPtr,
                                                         uint32_t *CorrectedSingleBitErrorsPtr,
                                                         uint32_t *DetectedDoubleBitErrorsPtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    int CamResult = CAM_OK;

    if ((CtxPtr == NULL) || (CorrectedSingleBitErrorsPtr == NULL) || (DetectedDoubleBitErrorsPtr == NULL))
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    if (CtxPtr->PrivateCtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_INVALID_CONTEXT;
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        CamResult
            = tcam_read_and_clear_ecc_counters(CtxPtr->PrivateCtxPtr->CamUnion.TcamPtr, CorrectedSingleBitErrorsPtr, DetectedDoubleBitErrorsPtr);
        Result = CamTopPrivCamConvertErrorCode(CamResult);
    }

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4TcamGetEccAddressesClearOnRead(XilVitisNetP4TcamCtx *CtxPtr,
                                                          uint32_t *FailingAddressSingleBitErrorPtr,
                                                          uint32_t *FailingAddressDoubleBitErrorPtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    int CamResult = CAM_OK;

    if ((CtxPtr == NULL) || (FailingAddressSingleBitErrorPtr == NULL) || (FailingAddressDoubleBitErrorPtr == NULL))
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    if (CtxPtr->PrivateCtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_INVALID_CONTEXT;
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        CamResult = tcam_read_and_clear_ecc_addresses(CtxPtr->PrivateCtxPtr->CamUnion.TcamPtr,
                                                      FailingAddressSingleBitErrorPtr,
                                                      FailingAddressDoubleBitErrorPtr);
        Result = CamTopPrivCamConvertErrorCode(CamResult);
    }

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4TcamExit(XilVitisNetP4TcamCtx *CtxPtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;

    if (CtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    if (CtxPtr->PrivateCtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_INVALID_CONTEXT;
    }

    tcam_destroy(CtxPtr->PrivateCtxPtr->CamUnion.TcamPtr);
    CtxPtr->PrivateCtxPtr->CamUnion.TcamPtr = NULL;

    CamTopPrivCamCtxDestroyCamArg(CtxPtr->PrivateCtxPtr);
    Result = XilVitisNetP4CamCtxDestroy(CtxPtr->PrivateCtxPtr);

    return Result;
}

/****************************************************************************************************************************************************/
/* SECTION: Global function definitions - STCAM API */
/****************************************************************************************************************************************************/

XilVitisNetP4ReturnType XilVitisNetP4StcamInit(XilVitisNetP4StcamCtx *CtxPtr, const XilVitisNetP4EnvIf *EnvIfPtr, XilVitisNetP4CamConfig *ConfigPtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;

    if ((CtxPtr == NULL) || (EnvIfPtr == NULL) || (ConfigPtr == NULL))
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    Result = CamTopPrivCamInit(&(CtxPtr->PrivateCtxPtr), EnvIfPtr, ConfigPtr, XIL_VITIS_NET_P4_CAM_STCAM, false);

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4StcamReset(XilVitisNetP4StcamCtx *CtxPtr)
{
    int CamResult = CAM_OK;
    uint32_t Position = 0;
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    uint8_t *KeyBufPtr;
    uint8_t *MaskBufPtr;
    uint8_t *ResponsePtr;
    uint8_t *ResponseMaskPtr;

    if (CtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    if (CtxPtr->PrivateCtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_INVALID_CONTEXT;
    }

    KeyBufPtr = calloc(1, CtxPtr->PrivateCtxPtr->KeyBufSizeBytes);
    if (KeyBufPtr == NULL)
    {
        Result = XIL_VITIS_NET_P4_CAM_ERR_MALLOC_FAILED;
        goto exit_func;
    }

    MaskBufPtr = calloc(1, CtxPtr->PrivateCtxPtr->KeyBufSizeBytes);
    if (MaskBufPtr == NULL)
    {
        Result = XIL_VITIS_NET_P4_CAM_ERR_MALLOC_FAILED;
        goto exit_free_key;
    }

    ResponsePtr = calloc(1, CtxPtr->PrivateCtxPtr->ResponseBufSizeBytes);
    if (ResponsePtr == NULL)
    {
        Result = XIL_VITIS_NET_P4_CAM_ERR_MALLOC_FAILED;
        goto exit_free_mask;
    }

    ResponseMaskPtr = calloc(1, CtxPtr->PrivateCtxPtr->ResponseBufSizeBytes);
    if (ResponseMaskPtr == NULL)
    {
        Result = XIL_VITIS_NET_P4_CAM_ERR_MALLOC_FAILED;
        goto exit_free_response;
    }

    /*
     * Using a ResponseMask of all zeros means get_by_response will find all
     *  keys present
     */
    while (1)
    {
        CamResult = stcam_get_by_response(CtxPtr->PrivateCtxPtr->CamUnion.StcamPtr, ResponsePtr, ResponseMaskPtr, &Position, KeyBufPtr, MaskBufPtr);

        if (CamResult == CAM_ERROR_KEY_NOT_FOUND)
        {
            /* Table has been successfully emptied */
            CamResult = CAM_OK;
            break;
        }
        if (CamResult != 0)
        {
            break;
        }

        CamResult = stcam_delete(CtxPtr->PrivateCtxPtr->CamUnion.StcamPtr, KeyBufPtr, MaskBufPtr);
        if (CamResult != 0)
        {
            break;
        }
    }
    Result = CamTopPrivCamConvertErrorCode(CamResult);

    free(ResponseMaskPtr);
exit_free_response:
    free(ResponsePtr);
exit_free_mask:
    free(MaskBufPtr);
exit_free_key:
    free(KeyBufPtr);
exit_func:
    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4StcamInsert(XilVitisNetP4StcamCtx *CtxPtr, uint8_t *KeyPtr, uint8_t *MaskPtr, uint32_t Priority, uint8_t *ResponsePtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    int CamResult = CAM_OK;
    uint8_t *KeyParamPtr = KeyPtr;
    uint8_t *MaskParamPtr = MaskPtr;
    uint8_t *ResponseParamPtr = ResponsePtr;

    if ((CtxPtr == NULL) || (KeyPtr == NULL) || (MaskPtr == NULL) || (ResponsePtr == NULL))
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    if (CtxPtr->PrivateCtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_INVALID_CONTEXT;
    }

    if (CtxPtr->PrivateCtxPtr->Endian == XIL_VITIS_NET_P4_BIG_ENDIAN)
    {
        KeyParamPtr = CtxPtr->PrivateCtxPtr->KeyBufPtr;
        Result = XilVitisNetP4CamByteSwap(CtxPtr->PrivateCtxPtr, KeyPtr, KeyParamPtr, VITISNETP4_CAM_SWAP_KEY);
        if (Result == XIL_VITIS_NET_P4_SUCCESS)
        {
            MaskParamPtr = CtxPtr->PrivateCtxPtr->MaskBufPtr;
            Result = XilVitisNetP4CamByteSwap(CtxPtr->PrivateCtxPtr, MaskPtr, MaskParamPtr, VITISNETP4_CAM_SWAP_KEY_MASK);
            if (Result == XIL_VITIS_NET_P4_SUCCESS)
            {
                ResponseParamPtr = CtxPtr->PrivateCtxPtr->ResponseBufPtr;
                Result = XilVitisNetP4CamByteSwap(CtxPtr->PrivateCtxPtr, ResponsePtr, ResponseParamPtr, VITISNETP4_CAM_SWAP_RESPONSE);
            }
        }
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        CamResult = stcam_insert(CtxPtr->PrivateCtxPtr->CamUnion.StcamPtr, KeyParamPtr, MaskParamPtr, Priority, ResponseParamPtr);
        Result = CamTopPrivCamConvertErrorCode(CamResult);
    }

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4StcamUpdate(XilVitisNetP4StcamCtx *CtxPtr, uint8_t *KeyPtr, uint8_t *MaskPtr, uint8_t *ResponsePtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    int CamResult = CAM_OK;
    uint8_t *KeyParamPtr = KeyPtr;
    uint8_t *MaskParamPtr = MaskPtr;
    uint8_t *ResponseParamPtr = ResponsePtr;

    if ((CtxPtr == NULL) || (KeyPtr == NULL) || (MaskPtr == NULL) || (ResponsePtr == NULL))
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    if (CtxPtr->PrivateCtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_INVALID_CONTEXT;
    }

    if (CtxPtr->PrivateCtxPtr->Endian == XIL_VITIS_NET_P4_BIG_ENDIAN)
    {
        KeyParamPtr = CtxPtr->PrivateCtxPtr->KeyBufPtr;
        Result = XilVitisNetP4CamByteSwap(CtxPtr->PrivateCtxPtr, KeyPtr, KeyParamPtr, VITISNETP4_CAM_SWAP_KEY);
        if (Result == XIL_VITIS_NET_P4_SUCCESS)
        {
            MaskParamPtr = CtxPtr->PrivateCtxPtr->MaskBufPtr;
            Result = XilVitisNetP4CamByteSwap(CtxPtr->PrivateCtxPtr, MaskPtr, MaskParamPtr, VITISNETP4_CAM_SWAP_KEY_MASK);
            if (Result == XIL_VITIS_NET_P4_SUCCESS)
            {
                ResponseParamPtr = CtxPtr->PrivateCtxPtr->ResponseBufPtr;
                Result = XilVitisNetP4CamByteSwap(CtxPtr->PrivateCtxPtr, ResponsePtr, ResponseParamPtr, VITISNETP4_CAM_SWAP_RESPONSE);
            }
        }
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        CamResult = stcam_update(CtxPtr->PrivateCtxPtr->CamUnion.StcamPtr, KeyParamPtr, MaskParamPtr, ResponseParamPtr);
        Result = CamTopPrivCamConvertErrorCode(CamResult);
    }

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4StcamGetByResponse(XilVitisNetP4StcamCtx *CtxPtr,
                                              uint8_t *ResponsePtr,
                                              uint8_t *ResponseMaskPtr,
                                              uint32_t *PositionPtr,
                                              uint8_t *KeyPtr,
                                              uint8_t *MaskPtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    int CamResult = CAM_OK;
    uint8_t *ResponseParamPtr = ResponsePtr;
    uint8_t *ResponseMaskParamPtr = ResponseMaskPtr;
    uint8_t *KeyParamPtr = KeyPtr;
    uint8_t *MaskParamPtr = MaskPtr;

    if ((CtxPtr == NULL) || (ResponsePtr == NULL) || (ResponseMaskPtr == NULL) || (PositionPtr == NULL) || (KeyPtr == NULL) || (MaskPtr == NULL))
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    if (CtxPtr->PrivateCtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_INVALID_CONTEXT;
    }

    if (CtxPtr->PrivateCtxPtr->Endian == XIL_VITIS_NET_P4_BIG_ENDIAN)
    {
        KeyParamPtr = CtxPtr->PrivateCtxPtr->KeyBufPtr;
        MaskParamPtr = CtxPtr->PrivateCtxPtr->MaskBufPtr;

        ResponseParamPtr = CtxPtr->PrivateCtxPtr->ResponseBufPtr;
        Result = XilVitisNetP4CamByteSwap(CtxPtr->PrivateCtxPtr, ResponsePtr, ResponseParamPtr, VITISNETP4_CAM_SWAP_RESPONSE);
        if (Result == XIL_VITIS_NET_P4_SUCCESS)
        {
            ResponseMaskParamPtr = CtxPtr->PrivateCtxPtr->ResponseMaskBufPtr;
            Result = XilVitisNetP4CamByteSwap(CtxPtr->PrivateCtxPtr, ResponseMaskPtr, ResponseMaskParamPtr, VITISNETP4_CAM_SWAP_RESPONSE_MASK);
        }
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        CamResult = stcam_get_by_response(CtxPtr->PrivateCtxPtr->CamUnion.StcamPtr,
                                          ResponseParamPtr,
                                          ResponseMaskParamPtr,
                                          PositionPtr,
                                          KeyParamPtr,
                                          MaskParamPtr);
        Result = CamTopPrivCamConvertErrorCode(CamResult);
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        if (CtxPtr->PrivateCtxPtr->Endian == XIL_VITIS_NET_P4_BIG_ENDIAN)
        {
            Result = XilVitisNetP4CamByteSwap(CtxPtr->PrivateCtxPtr, KeyParamPtr, KeyPtr, VITISNETP4_CAM_SWAP_KEY);
            if (Result == XIL_VITIS_NET_P4_SUCCESS)
            {
                Result = XilVitisNetP4CamByteSwap(CtxPtr->PrivateCtxPtr, MaskParamPtr, MaskPtr, VITISNETP4_CAM_SWAP_KEY_MASK);
            }
        }
    }

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4StcamGetByKey(XilVitisNetP4StcamCtx *CtxPtr, uint8_t *KeyPtr, uint8_t *MaskPtr, uint32_t *PriorityPtr, uint8_t *ResponsePtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    int CamResult = CAM_OK;
    uint8_t *KeyParamPtr = KeyPtr;
    uint8_t *MaskParamPtr = MaskPtr;
    uint8_t *ResponseParamPtr = ResponsePtr;

    if ((CtxPtr == NULL) || (KeyPtr == NULL) || (MaskPtr == NULL) || (PriorityPtr == NULL) || (ResponsePtr == NULL))
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    if (CtxPtr->PrivateCtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_INVALID_CONTEXT;
    }

    if (CtxPtr->PrivateCtxPtr->Endian == XIL_VITIS_NET_P4_BIG_ENDIAN)
    {
        ResponseParamPtr = CtxPtr->PrivateCtxPtr->ResponseBufPtr;

        KeyParamPtr = CtxPtr->PrivateCtxPtr->KeyBufPtr;
        Result = XilVitisNetP4CamByteSwap(CtxPtr->PrivateCtxPtr, KeyPtr, KeyParamPtr, VITISNETP4_CAM_SWAP_KEY);
        if (Result == XIL_VITIS_NET_P4_SUCCESS)
        {
            MaskParamPtr = CtxPtr->PrivateCtxPtr->MaskBufPtr;
            Result = XilVitisNetP4CamByteSwap(CtxPtr->PrivateCtxPtr, MaskPtr, MaskParamPtr, VITISNETP4_CAM_SWAP_KEY_MASK);
        }
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        CamResult = stcam_get_by_key(CtxPtr->PrivateCtxPtr->CamUnion.StcamPtr, KeyParamPtr, MaskParamPtr, PriorityPtr, ResponseParamPtr);
        Result = CamTopPrivCamConvertErrorCode(CamResult);
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        if (CtxPtr->PrivateCtxPtr->Endian == XIL_VITIS_NET_P4_BIG_ENDIAN)
        {
            Result = XilVitisNetP4CamByteSwap(CtxPtr->PrivateCtxPtr, ResponseParamPtr, ResponsePtr, VITISNETP4_CAM_SWAP_RESPONSE);
        }
    }

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4StcamLookup(XilVitisNetP4StcamCtx *CtxPtr, uint8_t *KeyPtr, uint8_t *ResponsePtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    int CamResult = CAM_OK;
    uint8_t *KeyParamPtr = KeyPtr;
    uint8_t *ResponseParamPtr = ResponsePtr;

    if ((CtxPtr == NULL) || (KeyPtr == NULL) || (ResponsePtr == NULL))
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    if (CtxPtr->PrivateCtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_INVALID_CONTEXT;
    }

    if (CtxPtr->PrivateCtxPtr->Endian == XIL_VITIS_NET_P4_BIG_ENDIAN)
    {
        ResponseParamPtr = CtxPtr->PrivateCtxPtr->ResponseBufPtr;

        KeyParamPtr = CtxPtr->PrivateCtxPtr->KeyBufPtr;
        Result = XilVitisNetP4CamByteSwap(CtxPtr->PrivateCtxPtr, KeyPtr, KeyParamPtr, VITISNETP4_CAM_SWAP_KEY);
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        CamResult = stcam_lookup(CtxPtr->PrivateCtxPtr->CamUnion.StcamPtr, KeyParamPtr, ResponseParamPtr);
        Result = CamTopPrivCamConvertErrorCode(CamResult);
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        if (CtxPtr->PrivateCtxPtr->Endian == XIL_VITIS_NET_P4_BIG_ENDIAN)
        {
            Result = XilVitisNetP4CamByteSwap(CtxPtr->PrivateCtxPtr, ResponseParamPtr, ResponsePtr, VITISNETP4_CAM_SWAP_RESPONSE);
        }
    }

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4StcamDelete(XilVitisNetP4StcamCtx *CtxPtr, uint8_t *KeyPtr, uint8_t *MaskPtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    int CamResult = CAM_OK;
    uint8_t *KeyParamPtr = KeyPtr;
    uint8_t *MaskParamPtr = MaskPtr;

    if ((CtxPtr == NULL) || (KeyPtr == NULL) || (MaskPtr == NULL))
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    if (CtxPtr->PrivateCtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_INVALID_CONTEXT;
    }

    if (CtxPtr->PrivateCtxPtr->Endian == XIL_VITIS_NET_P4_BIG_ENDIAN)
    {
        KeyParamPtr = CtxPtr->PrivateCtxPtr->KeyBufPtr;
        Result = XilVitisNetP4CamByteSwap(CtxPtr->PrivateCtxPtr, KeyPtr, KeyParamPtr, VITISNETP4_CAM_SWAP_KEY);
        if (Result == XIL_VITIS_NET_P4_SUCCESS)
        {
            MaskParamPtr = CtxPtr->PrivateCtxPtr->MaskBufPtr;
            Result = XilVitisNetP4CamByteSwap(CtxPtr->PrivateCtxPtr, MaskPtr, MaskParamPtr, VITISNETP4_CAM_SWAP_KEY_MASK);
        }
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        CamResult = stcam_delete(CtxPtr->PrivateCtxPtr->CamUnion.StcamPtr, KeyParamPtr, MaskParamPtr);
        Result = CamTopPrivCamConvertErrorCode(CamResult);
    }

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4StcamGetEccCountersClearOnRead(XilVitisNetP4StcamCtx *CtxPtr,
                                                          uint32_t *CorrectedSingleBitErrorsPtr,
                                                          uint32_t *DetectedDoubleBitErrorsPtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    int CamResult = CAM_OK;

    if ((CtxPtr == NULL) || (CorrectedSingleBitErrorsPtr == NULL) || (DetectedDoubleBitErrorsPtr == NULL))
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    if (CtxPtr->PrivateCtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_INVALID_CONTEXT;
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        CamResult
            = stcam_read_and_clear_ecc_counters(CtxPtr->PrivateCtxPtr->CamUnion.StcamPtr, CorrectedSingleBitErrorsPtr, DetectedDoubleBitErrorsPtr);
        Result = CamTopPrivCamConvertErrorCode(CamResult);
    }

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4StcamGetEccAddressesClearOnRead(XilVitisNetP4StcamCtx *CtxPtr,
                                                           uint32_t *FailingAddressSingleBitErrorPtr,
                                                           uint32_t *FailingAddressDoubleBitErrorPtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    int CamResult = CAM_OK;

    if ((CtxPtr == NULL) || (FailingAddressSingleBitErrorPtr == NULL) || (FailingAddressDoubleBitErrorPtr == NULL))
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    if (CtxPtr->PrivateCtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_INVALID_CONTEXT;
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        CamResult = stcam_read_and_clear_ecc_addresses(CtxPtr->PrivateCtxPtr->CamUnion.StcamPtr,
                                                       FailingAddressSingleBitErrorPtr,
                                                       FailingAddressDoubleBitErrorPtr);
        Result = CamTopPrivCamConvertErrorCode(CamResult);
    }

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4StcamExit(XilVitisNetP4StcamCtx *CtxPtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;

    if (CtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    if (CtxPtr->PrivateCtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_INVALID_CONTEXT;
    }

    stcam_destroy(CtxPtr->PrivateCtxPtr->CamUnion.StcamPtr);
    CtxPtr->PrivateCtxPtr->CamUnion.StcamPtr = NULL;

    CamTopPrivCamCtxDestroyCamArg(CtxPtr->PrivateCtxPtr);
    Result = XilVitisNetP4CamCtxDestroy(CtxPtr->PrivateCtxPtr);

    return Result;
}

/****************************************************************************************************************************************************/
/* SECTION: Global function definitions - Utilities */
/****************************************************************************************************************************************************/
XilVitisNetP4ReturnType XilVitisNetP4CamGetMaxHeapSize(void* CtxPtr, XilVitisNetP4EnvIf* EnvIfPtr, XilVitisNetP4CamConfig* ConfigPtr, XilVitisNetP4CamType CamType, uint64_t* HeapSizePtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;

    if ((CtxPtr == NULL) || (EnvIfPtr == NULL) || (ConfigPtr == NULL))
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    switch (CamType)
    {
        case XIL_VITIS_NET_P4_CAM_BCAM:
            Result = CamTopPrivCamInit(&(((XilVitisNetP4BcamCtx*)CtxPtr)->PrivateCtxPtr), EnvIfPtr, ConfigPtr, CamType, true, HeapSizePtr);
            break;

        case XIL_VITIS_NET_P4_CAM_STCAM:
            Result = CamTopPrivCamInit(&(((XilVitisNetP4StcamCtx*)CtxPtr)->PrivateCtxPtr), EnvIfPtr, ConfigPtr, CamType, true, HeapSizePtr);
            break;

        case XIL_VITIS_NET_P4_CAM_TCAM:
            Result = CamTopPrivCamInit(&(((XilVitisNetP4TcamCtx*)CtxPtr)->PrivateCtxPtr), EnvIfPtr, ConfigPtr, CamType, true, HeapSizePtr);
            break;

        case XIL_VITIS_NET_P4_CAM_DCAM:
            Result = CamTopPrivDcamInitialise(&(((XilVitisNetP4DcamCtx*)CtxPtr)->PrivateCtxPtr), EnvIfPtr, ConfigPtr, true, HeapSizePtr);
            break;

        case XIL_VITIS_NET_P4_CAM_TINY_BCAM:
            Result = GetHeapSizeTinyCam(&(((XilVitisNetP4TinyBcamCtx*)CtxPtr)->PrivateCtxPtr), EnvIfPtr, ConfigPtr, XIL_VITIS_NET_P4_TINY_CAM_BCAM_EMULATION, HeapSizePtr);
            break;

        case XIL_VITIS_NET_P4_CAM_TINY_TCAM:
            Result = GetHeapSizeTinyCam(&(((XilVitisNetP4TinyTcamCtx*)CtxPtr)->PrivateCtxPtr), EnvIfPtr, ConfigPtr, XIL_VITIS_NET_P4_TINY_CAM_TCAM_EMULATION, HeapSizePtr);
            break;

        default:
            Result = XIL_VITIS_NET_P4_TABLE_ERR_INVALID_TABLE_MODE;
            break;
    }

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4CamGetKeyLengthInBits(char *FormatStringPtr, uint32_t *KeyLengthPtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    XilVitisNetP4ReturnType ExtractResult = XIL_VITIS_NET_P4_SUCCESS;
    uint32_t KeyLength = 0;
    uint32_t FieldWidth = 0;

    if ((FormatStringPtr == NULL) || (KeyLengthPtr == NULL))
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    /* Iterate over format string and extract the size of each field */
    while (ExtractResult == XIL_VITIS_NET_P4_SUCCESS)
    {
        ExtractResult = CamTopPrivCamExtractNextFieldWidth(&FormatStringPtr, &FieldWidth);
        if (ExtractResult == XIL_VITIS_NET_P4_SUCCESS)
        {
            KeyLength += FieldWidth;
        }
    }

    if (KeyLength != 0)
    {
        *KeyLengthPtr = KeyLength;
    }
    else
    {
        Result = XIL_VITIS_NET_P4_CAM_ERR_FORMAT_SYNTAX;
    }

    return Result;
}

/****************************************************************************************************************************************************/
/* SECTION: Debug controls */
/****************************************************************************************************************************************************/

static uint32_t XilVitisNetP4CamDebugFlags = CAM_DEBUG_NONE;

void XilVitisNetP4CamSetDebugFlags(uint32_t flags)
{
    XilVitisNetP4CamDebugFlags = flags;
}

uint32_t XilVitisNetP4CamGetDebugFlags(void)
{
    return XilVitisNetP4CamDebugFlags;
}


/****************************************************************************************************************************************************/
/* SECTION: Common Cam functions definitions*/
/****************************************************************************************************************************************************/
XilVitisNetP4ReturnType XilVitisNetP4CamCtxCreate(XilVitisNetP4CamCtx** CtxPtr, const XilVitisNetP4EnvIf* EnvIfPtr, XilVitisNetP4CamConfig* ConfigPtr, bool CalcHeapSize, ...)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    XilVitisNetP4CamCtx *AllocPtr = NULL;
    bool DoSwap = false;

    if ((CtxPtr == NULL) || (EnvIfPtr == NULL) || (ConfigPtr == NULL))
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_INTERNAL_ASSERTION;
    }

    /* Provided pointer should be NULL - if not, we're looking at invalid memory somehow */
    if (*CtxPtr != NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_INTERNAL_ASSERTION;
    }

    /* If endian swap is needed, more memory must be allocated */
    if (ConfigPtr->Endian == XIL_VITIS_NET_P4_LITTLE_ENDIAN)
    {
        DoSwap = false;
    }
    else if (ConfigPtr->Endian == XIL_VITIS_NET_P4_BIG_ENDIAN)
    {
        DoSwap = true;
    }
    else
    {
        return XIL_VITIS_NET_P4_CAM_ERR_INVALID_ENDIAN;
    }

    AllocPtr = calloc(1, sizeof(XilVitisNetP4CamCtx));
    if (AllocPtr == NULL)
    {
        return XIL_VITIS_NET_P4_CAM_ERR_MALLOC_FAILED;
    }

    /* Compute and store key width - needed for BCAM and STCAM and also for endian swap buffer sizing */
    Result = XilVitisNetP4CamGetKeyLengthInBits(ConfigPtr->FormatStringPtr, &(AllocPtr->KeySizeBits));
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        goto fail_key_len;
    }

    /* Compute storage needs used for little endian configuration and reset functions */
    AllocPtr->KeyPaddingSizeBits = AllocPtr->KeySizeBits % XIL_VITIS_NET_P4_BITS_PER_BYTE;
    AllocPtr->KeyBufSizeBytes = AllocPtr->KeySizeBits / XIL_VITIS_NET_P4_BITS_PER_BYTE;
    if (AllocPtr->KeyPaddingSizeBits != 0)
    {
        AllocPtr->KeyBufSizeBytes++;
        AllocPtr->KeyPaddingSizeBits = XIL_VITIS_NET_P4_BITS_PER_BYTE - AllocPtr->KeyPaddingSizeBits;
        AllocPtr->KeyPaddingMask = (uint8_t)(XIL_VITIS_NET_P4_BYTE_MASK >> AllocPtr->KeyPaddingSizeBits);
    }

    AllocPtr->ResponseSizeBits = ConfigPtr->ResponseSizeBits;
    AllocPtr->ResponsePaddingSizeBits = ConfigPtr->ResponseSizeBits % XIL_VITIS_NET_P4_BITS_PER_BYTE;
    AllocPtr->ResponseBufSizeBytes = ConfigPtr->ResponseSizeBits / XIL_VITIS_NET_P4_BITS_PER_BYTE;
    if (AllocPtr->ResponsePaddingSizeBits != 0)
    {
        AllocPtr->ResponseBufSizeBytes++;
        AllocPtr->ResponsePaddingSizeBits = XIL_VITIS_NET_P4_BITS_PER_BYTE - AllocPtr->ResponsePaddingSizeBits;
        AllocPtr->ResponsePaddingMask = (uint8_t)(XIL_VITIS_NET_P4_BYTE_MASK >> AllocPtr->ResponsePaddingSizeBits);
    }

    /* These steps are not needed when running in little-endian configuration */
    if (DoSwap == true)
    {
        if (CalcHeapSize == false)
        {
            /* Allocate storage */
            AllocPtr->KeyBufPtr = calloc(AllocPtr->KeyBufSizeBytes, sizeof(uint8_t));
            AllocPtr->MaskBufPtr = calloc(AllocPtr->KeyBufSizeBytes, sizeof(uint8_t));
            AllocPtr->ResponseBufPtr = calloc(AllocPtr->ResponseBufSizeBytes, sizeof(uint8_t));
            AllocPtr->ResponseMaskBufPtr = calloc(AllocPtr->ResponseBufSizeBytes, sizeof(uint8_t));

            /* If any of the above should fail, clean up any and all which may have succeeded */
            if ((AllocPtr->KeyBufPtr == NULL) || (AllocPtr->MaskBufPtr == NULL) || (AllocPtr->ResponseBufPtr == NULL)
                || (AllocPtr->ResponseMaskBufPtr == NULL))
            {
                Result = XIL_VITIS_NET_P4_CAM_ERR_MALLOC_FAILED;
                goto fail_endian_swap_malloc;
            }
        }
    }

    /* Complete the population of the context and return it to the caller */
    memcpy(&(AllocPtr->EnvIf), EnvIfPtr, sizeof(AllocPtr->EnvIf));
    AllocPtr->BaseAddr = ConfigPtr->BaseAddr;
    AllocPtr->NumEntries = ConfigPtr->NumEntries;
    AllocPtr->Endian = ConfigPtr->Endian;
    *CtxPtr = AllocPtr;

    if (CalcHeapSize == true)
    {
        /* Calculate the amount of memory in bytes that has been allocated on the heap. The memory must still be allocated */
        /* so that the a cam instance can be populated within the cam library                                              */
        uint64_t* VitisNetP4CamCtxHeapSizePtr;
        va_list   ArgPtr;
        va_start(ArgPtr, CalcHeapSize);
        VitisNetP4CamCtxHeapSizePtr = va_arg(ArgPtr, uint64_t*);
        va_end(ArgPtr);
        *VitisNetP4CamCtxHeapSizePtr = (uint64_t)sizeof(XilVitisNetP4CamCtx) +
                                  (uint64_t)((AllocPtr->KeyBufSizeBytes) * sizeof(uint8_t)) +
                                  (uint64_t)((AllocPtr->KeyBufSizeBytes) * sizeof(uint8_t)) +
                                  (uint64_t)((AllocPtr->ResponseBufSizeBytes) * sizeof(uint8_t)) +
                                  (uint64_t)((AllocPtr->ResponseBufSizeBytes) * sizeof(uint8_t));
    }

    return Result;

    /* Error handling */
fail_endian_swap_malloc:
    /* Safe even if memory has not been allocated */
    free(AllocPtr->KeyBufPtr);
    free(AllocPtr->MaskBufPtr);
    free(AllocPtr->ResponseBufPtr);
    free(AllocPtr->ResponseMaskBufPtr);

fail_key_len:
    free(AllocPtr);

    return Result;
}

static XilVitisNetP4ReturnType CamTopPrivCamCtxCreateCamArg(XilVitisNetP4CamCtx *CtxPtr, bool CalcHeapSize, ...)
{
    int CamResult = CAM_OK;
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    if (CtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_INTERNAL_ASSERTION;
    }

    /* Allocation to store the obfuscated cam driver configuration */
    CamResult = cam_arg_create(&(CtxPtr->CamArgPtr));

    if (CalcHeapSize == true)
    {
        /* Retrieve the size of heap allocation for the cam arguments*/
        uint64_t* HeapSizePtr;
        va_list   ArgPtr;
        va_start(ArgPtr, CalcHeapSize);

        HeapSizePtr = va_arg(ArgPtr, uint64_t*);
        va_end(ArgPtr);

        *HeapSizePtr = cam_arg_size();
    }

    Result = CamTopPrivCamConvertErrorCode(CamResult);
    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4CamCtxDestroy(XilVitisNetP4CamCtx *CtxPtr)
{
    /* If a NULL is passed, the context has already been cleaned up so return indicating success */
    if (CtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_SUCCESS;
    }

    /* Ensure that the CAM-specific context has been cleaned up first so that there's no dangling pointers - union means one check is enough */
    if (CtxPtr->CamUnion.BcamPtr != NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_INTERNAL_ASSERTION;
    }

    /* Free up all memories that were allocated during creation */
    free(CtxPtr->KeyBufPtr);
    free(CtxPtr->MaskBufPtr);
    free(CtxPtr->ResponseBufPtr);
    free(CtxPtr->ResponseMaskBufPtr);
    free(CtxPtr);

    return XIL_VITIS_NET_P4_SUCCESS;
}

static void CamTopPrivCamCtxDestroyCamArg(XilVitisNetP4CamCtx *CtxPtr)
{
    /* If a NULL is passed, the context has already been cleaned up so return indicating success */
    if (CtxPtr == NULL)
    {
        return ;
    }

    cam_arg_destroy(CtxPtr->CamArgPtr);
    CtxPtr->CamArgPtr = NULL;

    return ;
}

/****************************************************************************************************************************************************/
/* SECTION: Local function definitions */
/****************************************************************************************************************************************************/
static XilVitisNetP4ReturnType CamTopPrivDcamInitialise(XilVitisNetP4CamCtx** CtxPtr, const XilVitisNetP4EnvIf* EnvIfPtr, XilVitisNetP4CamConfig* ConfigPtr, bool CalcHeapSize, ...)
{
    XilVitisNetP4ReturnType Ret = XIL_VITIS_NET_P4_SUCCESS;
    XilVitisNetP4CamCtx* PrivateCtxPtr = NULL;

    if (CalcHeapSize == true)
    {
        /* Calculate the size of heap allocation for the cam arguments*/
        uint64_t* HeapSizePtr;
        va_list   ArgPtr;
        va_start(ArgPtr, CalcHeapSize);

        HeapSizePtr = va_arg(ArgPtr, uint64_t*);
        va_end(ArgPtr);

        *HeapSizePtr = (uint64_t)(sizeof(XilVitisNetP4CamCtx));
    }
    else
    {
        PrivateCtxPtr = calloc(1, sizeof(XilVitisNetP4CamCtx));
        if (PrivateCtxPtr == NULL)
        {
            return XIL_VITIS_NET_P4_CAM_ERR_MALLOC_FAILED;
        }

        /* Store provided configuration */
        memcpy(&(PrivateCtxPtr->EnvIf), EnvIfPtr, sizeof(XilVitisNetP4EnvIf));
        PrivateCtxPtr->BaseAddr = ConfigPtr->BaseAddr;
        PrivateCtxPtr->NumEntries = ConfigPtr->NumEntries;
        PrivateCtxPtr->ResponseSizeBits = ConfigPtr->ResponseSizeBits;
        PrivateCtxPtr->ResponsePaddingSizeBits = (ConfigPtr->ResponseSizeBits % XIL_VITIS_NET_P4_BITS_PER_BYTE);
        PrivateCtxPtr->Endian = ConfigPtr->Endian;

        /* Compute the size of the response in terms of bytes, taking padding into account */
        PrivateCtxPtr->ResponseBufSizeBytes = PrivateCtxPtr->ResponseSizeBits / XIL_VITIS_NET_P4_BITS_PER_BYTE;
        if ((PrivateCtxPtr->ResponseSizeBits % XIL_VITIS_NET_P4_BITS_PER_BYTE) != 0)
        {
            PrivateCtxPtr->ResponseBufSizeBytes++;
        }

        *CtxPtr = PrivateCtxPtr;
    }

    return Ret;
}

static XilVitisNetP4ReturnType CamTopPrivCamInit(XilVitisNetP4CamCtx **CtxPtr, const XilVitisNetP4EnvIf *EnvIfPtr, XilVitisNetP4CamConfig *ConfigPtr, XilVitisNetP4CamType CamType, bool CalcHeapSize, ...)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    XilVitisNetP4CamCtx *AllocPtr = NULL;
    uint64_t* HeapSizePtr = NULL;
    uint64_t VitisNetP4CamCtxHeapSize;
    uint64_t CamArgConfigHeapSize;
    uint64_t CamInstanceHeapSize ;

    if (CalcHeapSize == true)
    {
        va_list   ArgPtr;
        va_start(ArgPtr, CalcHeapSize);
        HeapSizePtr = va_arg(ArgPtr, uint64_t*);
        va_end(ArgPtr);
    }

    if (CalcHeapSize == true)
    {
        Result = XilVitisNetP4CamCtxCreate(&AllocPtr, EnvIfPtr, ConfigPtr, CalcHeapSize, &VitisNetP4CamCtxHeapSize);
    }
    else
    {
        Result = XilVitisNetP4CamCtxCreate(&AllocPtr, EnvIfPtr, ConfigPtr, CalcHeapSize);
    }

    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        goto fail_ctx_create;
    }

    if (CalcHeapSize == true)
    {
        Result = CamTopPrivCamCtxCreateCamArg(AllocPtr, CalcHeapSize, &CamArgConfigHeapSize);
    }
    else
    {
        Result = CamTopPrivCamCtxCreateCamArg(AllocPtr, CalcHeapSize);
    }
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        goto fail_add_cam_arg;
    }

    Result = CamTopPrivCamObfDrvSetConfig(AllocPtr, ConfigPtr, CamType, CalcHeapSize);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        goto fail_obf_drv;
    }

    if (CalcHeapSize == true)
    {
        /* Calculate the heap allocation for the cam. This will require a populated vitisnetp4 cam context which will have been alloced */
        Result = CamTopPrivCamObfDrvCreate(AllocPtr, CamType, CalcHeapSize, &CamInstanceHeapSize);
    }
    else
    {
        Result = CamTopPrivCamObfDrvCreate(AllocPtr, CamType, CalcHeapSize);
    }

    if (CalcHeapSize == true)
    {
        /* Calculate total heap size for cam instance. As parf of this calculcation some memory will have been alloc'd */
        *HeapSizePtr = VitisNetP4CamCtxHeapSize + CamArgConfigHeapSize + CamInstanceHeapSize;
        goto release_heap_mem;
    }

    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        goto fail_obf_drv;
    }

    /* Return allocated context to the caller */
    *CtxPtr = AllocPtr;



    return Result;

release_heap_mem:
fail_obf_drv:/* Error handling path with deliberate fall-through */
    CamTopPrivCamCtxDestroyCamArg(AllocPtr);
fail_add_cam_arg:
    XilVitisNetP4CamCtxDestroy(AllocPtr);
fail_ctx_create:
    return Result;
}


static XilVitisNetP4ReturnType CamTopPrivCamObfDrvSetConfig(XilVitisNetP4CamCtx *CtxPtr, XilVitisNetP4CamConfig *ConfigPtr, XilVitisNetP4CamType CamType, bool CalcHeapSize)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    int CamResult = CAM_OK;
    float LookupFrequencyMhz;
    float RamFrequencyMhz;
    float LookupsMillionsPerSec;
    uint32_t CamOptimizationValue;


    if ((CtxPtr == NULL) || (ConfigPtr == NULL))
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_INTERNAL_ASSERTION;
    }

    /* Force priority width if specified */
    if (ConfigPtr->PrioritySizeBits != XIL_VITIS_NET_P4_CAM_PRIORITY_SIZE_DEFAULT)
    {
        cam_arg_force_prio_width(CtxPtr->CamArgPtr, ConfigPtr->PrioritySizeBits);
    }

    /* Apply selected memory type configuration  */
    switch (ConfigPtr->MemType)
    {
        case XIL_VITIS_NET_P4_CAM_MEM_AUTO:
            break;

        case XIL_VITIS_NET_P4_CAM_MEM_BRAM:
            cam_arg_force_mem_type(CtxPtr->CamArgPtr, false);
            break;

        case XIL_VITIS_NET_P4_CAM_MEM_URAM:
            cam_arg_force_mem_type(CtxPtr->CamArgPtr, true);
            break;

        case XIL_VITIS_NET_P4_CAM_MEM_HBM:
            cam_arg_force_hbm(CtxPtr->CamArgPtr);
            break;

        /* currently this options is only used to internal CAM testing of ASIC ram memory */
        case XIL_VITIS_NET_P4_CAM_MEM_RAM:
            cam_arg_force_asic_ram(CtxPtr->CamArgPtr);
            /* As this is for internal asic testing only the size unit eqauls datawidth used for asic not Kilobytes */
            cam_arg_set_ram_size(CtxPtr->CamArgPtr, ConfigPtr->RamSizeKbytes);
            break;

        default:
            Result = XIL_VITIS_NET_P4_CAM_ERR_INVALID_MEM_TYPE;
            break;
    }

    /* Apply remaining configuration */
    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        cam_arg_calc_heap_size(CtxPtr->CamArgPtr, CalcHeapSize);
        cam_arg_set_inst_ctx(CtxPtr->CamArgPtr, CtxPtr);
        cam_arg_set_hw_write_function(CtxPtr->CamArgPtr, CamTopPrivCamHwWrite);
        cam_arg_set_hw_read_function(CtxPtr->CamArgPtr, CamTopPrivCamHwRead);
        cam_arg_set_error_print_function(CtxPtr->CamArgPtr, CamTopPrivCamLogError);
        cam_arg_set_info_print_function(CtxPtr->CamArgPtr, CamTopPrivCamLogInfo);
        cam_arg_set_debug_flags(CtxPtr->CamArgPtr, XilVitisNetP4CamDebugFlags);
        cam_arg_set_num_entries(CtxPtr->CamArgPtr, ConfigPtr->NumEntries);
        cam_arg_set_response_width(CtxPtr->CamArgPtr, ConfigPtr->ResponseSizeBits);

        LookupsMillionsPerSec = (float)(ConfigPtr->LookupsPerSec/1000000.0);
        cam_arg_set_engine_lookup_rate(CtxPtr->CamArgPtr, LookupsMillionsPerSec);

        /* The following must be skipped when HBM is being used */
        if (ConfigPtr->MemType != XIL_VITIS_NET_P4_CAM_MEM_HBM)
        {
            RamFrequencyMhz = (float)(ConfigPtr->RamFrequencyHz/XIL_VITIS_NET_P4_NUM_HZ_PER_MHZ);
            cam_arg_set_ram_freq(CtxPtr->CamArgPtr, RamFrequencyMhz);

            LookupFrequencyMhz = (float)(ConfigPtr->LookupFrequencyHz/XIL_VITIS_NET_P4_NUM_HZ_PER_MHZ);
            cam_arg_set_lookup_interface_freq(CtxPtr->CamArgPtr, LookupFrequencyMhz);
        }
    }

    switch (CamType)
    {
        case XIL_VITIS_NET_P4_CAM_BCAM:
            cam_arg_set_key_width(CtxPtr->CamArgPtr, CtxPtr->KeySizeBits);
            break;

        case XIL_VITIS_NET_P4_CAM_TCAM:
            CamResult = cam_arg_set_format(CtxPtr->CamArgPtr, ConfigPtr->FormatStringPtr);
            Result = CamTopPrivCamConvertErrorCode(CamResult);
            break;

        case XIL_VITIS_NET_P4_CAM_STCAM:
            cam_arg_set_key_width(CtxPtr->CamArgPtr, CtxPtr->KeySizeBits);
            cam_arg_set_num_masks(CtxPtr->CamArgPtr, ConfigPtr->NumMasks);
            break;

        default:
            Result = XIL_VITIS_NET_P4_GENERAL_ERR_INTERNAL_ASSERTION;
            break;
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        // Set the Optimisation level here
        Result = CamTopPrivCamConvertOptTypeToCamOptValue(ConfigPtr->OptimizationType, &CamOptimizationValue);
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {

        cam_arg_set_optimization_flags(CtxPtr->CamArgPtr, CamOptimizationValue);
    }

    return Result;
}

static XilVitisNetP4ReturnType CamTopPrivCamObfDrvCreate(XilVitisNetP4CamCtx *CtxPtr, XilVitisNetP4CamType CamType, bool CalcHeapSize, ...)
{
    int CamResult = CAM_OK;
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    uint64_t* HeapSizePtr;

    if (CtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_INTERNAL_ASSERTION;
    }

    switch (CamType)
    {
        case XIL_VITIS_NET_P4_CAM_BCAM:
            CamResult = bcam_create(CtxPtr->CamArgPtr, &(CtxPtr->CamUnion.BcamPtr));
            break;

        case XIL_VITIS_NET_P4_CAM_TCAM:
            CamResult = tcam_create(CtxPtr->CamArgPtr, &(CtxPtr->CamUnion.TcamPtr));
            break;

        case XIL_VITIS_NET_P4_CAM_STCAM:
            CamResult = stcam_create(CtxPtr->CamArgPtr, &(CtxPtr->CamUnion.StcamPtr));
            break;

        default:
            Result = XIL_VITIS_NET_P4_GENERAL_ERR_INTERNAL_ASSERTION;
            break;
    }

    /* If one of the cases matched, get its result */
    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        if (CalcHeapSize == true) /* CamResult is either the heap size in bytes or an error */
        {
            if (CamResult > CAM_ERROR_FATAL_BASE_END)
            {
                /* Pass back the heap size */
                va_list   ArgPtr;
                va_start(ArgPtr, CalcHeapSize);
                HeapSizePtr = va_arg(ArgPtr, uint64_t *);
                va_end(ArgPtr);

                *HeapSizePtr = CamResult;
                Result = XIL_VITIS_NET_P4_SUCCESS;
            }
            else
            {
                /* Return an error code */
                Result = CamTopPrivCamConvertErrorCode(CamResult);
            }
        }
        else
        {
            Result = CamTopPrivCamConvertErrorCode(CamResult);
        }
    }

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4CamByteSwap(XilVitisNetP4CamCtx* CtxPtr, uint8_t* SrcDataPtr, uint8_t* DstDataPtr, XilVitisNetP4SwapCmd Cmd)
{
    uint32_t ByteIndex = 0;
    uint32_t BufSizeBytes = 0;

    if ((CtxPtr == NULL) || (SrcDataPtr == NULL) || (DstDataPtr == NULL))
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_INTERNAL_ASSERTION;
    }

    /* Determine the buffer size of the provided data */
    switch (Cmd)
    {
        case VITISNETP4_CAM_SWAP_KEY:
        case VITISNETP4_CAM_SWAP_KEY_MASK:
            BufSizeBytes = CtxPtr->KeyBufSizeBytes;
            break;

        case VITISNETP4_CAM_SWAP_RESPONSE:
        case VITISNETP4_CAM_SWAP_RESPONSE_MASK:
            BufSizeBytes = CtxPtr->ResponseBufSizeBytes;
            break;
    }

    for (ByteIndex = 0; ByteIndex < BufSizeBytes; ByteIndex++)
    {
        DstDataPtr[BufSizeBytes - ByteIndex - 1] = SrcDataPtr[ByteIndex];
    }

    return XIL_VITIS_NET_P4_SUCCESS;
}

static XilVitisNetP4ReturnType CamTopPrivCamExtractNextFieldWidth(char **FormatStringPtr, uint32_t *FieldWidthPtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    uint32_t FieldWidth = 0;
    char *FieldEndPtr = NULL;
    char *CurrCharPtr = NULL;
    uint32_t ScaleFactor = 1;
    int CurrDigit = 0;
    int FieldStringLength = 0;

    if ((FormatStringPtr == NULL) || (FieldWidthPtr == NULL))
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_INTERNAL_ASSERTION;
    }

    if (*FormatStringPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_INTERNAL_ASSERTION;
    }

    FieldEndPtr = strchr(*FormatStringPtr, ':');
    if (FieldEndPtr != NULL)
    {
        /* Delimiter not allowed in first character of the string */
        if (FieldEndPtr == *FormatStringPtr)
        {
            FieldEndPtr = NULL;
            Result = XIL_VITIS_NET_P4_CAM_ERR_FORMAT_SYNTAX;
        }
    }
    else
    {
        FieldStringLength = strlen(*FormatStringPtr);

        /* The string must have at least one character */
        if (FieldStringLength == 0)
        {
            Result = XIL_VITIS_NET_P4_CAM_ERR_FORMAT_SYNTAX;
        }
        else
        {
            FieldEndPtr = *FormatStringPtr + FieldStringLength;
        }
    }

    /* Identify the format of the field */
    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        CurrCharPtr = FieldEndPtr - 1;
        Result = CamTopPrivCamValidateFieldType(*CurrCharPtr);
    }

    /* Extract the width of the field, working from the least to the most significant digit */
    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        while (CurrCharPtr-- != *FormatStringPtr)
        {
            if ((*CurrCharPtr >= '0') && (*CurrCharPtr <= '9'))
            {
                /* Digits are scaled based on their significance (position) in the string - units, tens, hundreds etc. */
                CurrDigit = *CurrCharPtr - '0';
                FieldWidth += ((uint32_t)CurrDigit) * ScaleFactor;
                ScaleFactor *= 10;
            }
            else
            {
                Result = XIL_VITIS_NET_P4_CAM_ERR_FORMAT_SYNTAX;
                break;
            }
        }
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        if (*FieldEndPtr != '\0')
        {
            FieldEndPtr++;
        }

        *FormatStringPtr = FieldEndPtr;
        *FieldWidthPtr = FieldWidth;
    }

    return Result;
}

static XilVitisNetP4ReturnType CamTopPrivCamValidateFieldType(char FieldType)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;

    switch (FieldType)
    {
        /* Deliberate fall-through execution */
        case 'r':
        case 'p':
        case 'b':
        case 't':
        case 'c':
        case 'u':
            break;

        /* Invalid field type */
        default:
            Result = XIL_VITIS_NET_P4_CAM_ERR_FORMAT_SYNTAX;
            break;
    }

    return Result;
}

static XilVitisNetP4ReturnType CamTopPrivCamConvertErrorCode(int ErrorCode)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_CAM_ERR_UNKNOWN;

    switch (ErrorCode)
    {
        case CAM_OK:
            Result = XIL_VITIS_NET_P4_SUCCESS;
            break;

        case CAM_ERROR_FULL:
            Result = XIL_VITIS_NET_P4_CAM_ERR_FULL;
            break;

        case CAM_ERROR_NO_OPEN:
            Result = XIL_VITIS_NET_P4_CAM_ERR_NO_OPEN;
            break;

        case CAM_ERROR_WRONG_KEY_WIDTH:
            Result = XIL_VITIS_NET_P4_CAM_ERR_WRONG_KEY_WIDTH;
            break;

        case CAM_ERROR_TOO_MANY_INSTANCES:
            Result = XIL_VITIS_NET_P4_CAM_ERR_TOO_MANY_INSTANCES;
            break;

        case CAM_ERROR_WRONG_BIT_FIELD_MASK:
            Result = XIL_VITIS_NET_P4_CAM_ERR_WRONG_BIT_FIELD_MASK;
            break;

        case CAM_ERROR_WRONG_CONST_FIELD_MASK:
            Result = XIL_VITIS_NET_P4_CAM_ERR_WRONG_CONST_FIELD_MASK;
            break;

        case CAM_ERROR_WRONG_UNUSED_FIELD_MASK:
            Result = XIL_VITIS_NET_P4_CAM_ERR_WRONG_UNUSED_FIELD_MASK;
            break;

        case CAM_ERROR_INVALID_TERNARY_FIELD_LEN:
            Result = XIL_VITIS_NET_P4_CAM_ERR_INVALID_TERNARY_FIELD_LEN;
            break;

        case CAM_ERROR_WRONG_PRIO_WIDTH:
            Result = XIL_VITIS_NET_P4_CAM_ERR_WRONG_PRIO_WIDTH;
            break;

        case CAM_ERROR_WRONG_MAX:
            Result = XIL_VITIS_NET_P4_CAM_ERR_WRONG_MAX;
            break;

        case CAM_ERROR_DUPLICATE_FOUND:
            Result = XIL_VITIS_NET_P4_CAM_ERR_DUPLICATE_FOUND;
            break;

        case CAM_ERROR_WRONG_PREFIX:
            Result = XIL_VITIS_NET_P4_CAM_ERR_WRONG_PREFIX;
            break;

        case CAM_ERROR_WRONG_PREFIX_MASK:
            Result = XIL_VITIS_NET_P4_CAM_ERR_WRONG_PREFIX_MASK;
            break;

        case CAM_ERROR_WRONG_RANGE:
            Result = XIL_VITIS_NET_P4_CAM_ERR_WRONG_RANGE;
            break;

        case CAM_ERROR_WRONG_RANGE_MASK:
            Result = XIL_VITIS_NET_P4_CAM_ERR_WRONG_RANGE_MASK;
            break;

        case CAM_ERROR_KEY_NOT_FOUND:
            Result = XIL_VITIS_NET_P4_CAM_ERR_KEY_NOT_FOUND;
            break;

        case CAM_ERROR_WRONG_MIN:
            Result = XIL_VITIS_NET_P4_CAM_ERR_WRONG_MIN;
            break;

        case CAM_ERROR_WRONG_PRIO:
            Result = XIL_VITIS_NET_P4_CAM_ERR_WRONG_PRIO;
            break;

        case CAM_ERROR_WRONG_LIST_LENGTH:
            Result = XIL_VITIS_NET_P4_CAM_ERR_WRONG_LIST_LENGTH;
            break;

        case CAM_ERROR_WRONG_NUMBER_OF_SLOTS:
            Result = XIL_VITIS_NET_P4_CAM_ERR_WRONG_NUMBER_OF_SLOTS;
            break;

        case CAM_ERROR_INVALID_MEM_TYPE:
            Result = XIL_VITIS_NET_P4_CAM_ERR_INVALID_MEM_TYPE;
            break;

        case CAM_ERROR_TOO_HIGH_FREQUENCY:
            Result = XIL_VITIS_NET_P4_CAM_ERR_TOO_HIGH_FREQUENCY;
            break;

        case CAM_ERROR_WRONG_TERNARY_MASK:
            Result = XIL_VITIS_NET_P4_CAM_ERR_WRONG_TERNARY_MASK;
            break;

        case CAM_ERROR_MASKED_KEY_BIT_IS_SET:
            Result = XIL_VITIS_NET_P4_CAM_ERR_MASKED_KEY_BIT_IS_SET;
            break;

        case CAM_ERROR_INVALID_MODE:
            Result = XIL_VITIS_NET_P4_CAM_ERR_INVALID_MODE;
            break;

        case CAM_ERROR_WRONG_RESPONSE_WIDTH:
            Result = XIL_VITIS_NET_P4_CAM_ERR_WRONG_RESPONSE_WIDTH;
            break;

        case CAM_ERROR_FORMAT_SYNTAX:
            Result = XIL_VITIS_NET_P4_CAM_ERR_FORMAT_SYNTAX;
            break;

        case CAM_ERROR_TOO_MANY_FIELDS:
            Result = XIL_VITIS_NET_P4_CAM_ERR_TOO_MANY_FIELDS;
            break;

        case CAM_ERROR_TOO_MANY_RANGES:
            Result = XIL_VITIS_NET_P4_CAM_ERR_TOO_MANY_RANGES;
            break;

        case CAM_ERROR_INVALID_RANGE_LEN:
            Result = XIL_VITIS_NET_P4_CAM_ERR_INVALID_RANGE_LEN;
            break;

        case CAM_ERROR_INVALID_RANGE_START:
            Result = XIL_VITIS_NET_P4_CAM_ERR_INVALID_RANGE_START;
            break;

        case CAM_ERROR_INVALID_PREFIX_LEN:
            Result = XIL_VITIS_NET_P4_CAM_ERR_INVALID_PREFIX_LEN;
            break;

        case CAM_ERROR_INVALID_PREFIX_START:
            Result = XIL_VITIS_NET_P4_CAM_ERR_INVALID_PREFIX_START;
            break;

        case CAM_ERROR_INVALID_PREFIX_KEY:
            Result = XIL_VITIS_NET_P4_CAM_ERR_INVALID_PREFIX_KEY;
            break;

        case CAM_ERROR_INVALID_BIT_FIELD_LEN:
            Result = XIL_VITIS_NET_P4_CAM_ERR_INVALID_BIT_FIELD_LEN;
            break;

        case CAM_ERROR_INVALID_BIT_FIELD_START:
            Result = XIL_VITIS_NET_P4_CAM_ERR_INVALID_BIT_FIELD_START;
            break;

        case CAM_ERROR_INVALID_CONST_FIELD_LEN:
            Result = XIL_VITIS_NET_P4_CAM_ERR_INVALID_CONST_FIELD_LEN;
            break;

        case CAM_ERROR_INVALID_CONST_FIELD_START:
            Result = XIL_VITIS_NET_P4_CAM_ERR_INVALID_CONST_FIELD_START;
            break;

        case CAM_ERROR_INVALID_UNUSED_FIELD_LEN:
            Result = XIL_VITIS_NET_P4_CAM_ERR_INVALID_UNUSED_FIELD_LEN;
            break;

        case CAM_ERROR_INVALID_UNUSED_FIELD_START:
            Result = XIL_VITIS_NET_P4_CAM_ERR_INVALID_UNUSED_FIELD_START;
            break;

        case CAM_ERROR_MAX_KEY_LEN_EXCEED:
            Result = XIL_VITIS_NET_P4_CAM_ERR_MAX_KEY_LEN_EXCEED;
            break;

        case CAM_ERROR_INVALID_PRIO_AND_INDEX_WIDTH:
            Result = XIL_VITIS_NET_P4_CAM_ERR_INVALID_PRIO_AND_INDEX_WIDTH;
            break;

        case CAM_ERROR_TOO_MANY_UNITS:
            Result = XIL_VITIS_NET_P4_CAM_ERR_TOO_MANY_UNITS;
            break;

        case CAM_ERROR_NO_MASK:
            Result = XIL_VITIS_NET_P4_CAM_ERR_NO_MASK;
            break;

        case CAM_ERROR_INVALID_MEMORY_WIDTH:
            Result = XIL_VITIS_NET_P4_CAM_ERR_INVALID_MEMORY_WIDTH;
            break;

        case CAM_ERROR_UNSUPPORTED_COMMAND:
            Result = XIL_VITIS_NET_P4_CAM_ERR_UNSUPPORTED_COMMAND;
            break;

        case CAM_ERROR_ENVIRONMENT:
            Result = XIL_VITIS_NET_P4_CAM_ERR_ENVIRONMENT;
            break;

        case CAM_ERROR_UNSUPPORTED_CAM_TYPE:
            Result = XIL_VITIS_NET_P4_CAM_ERR_UNSUPPORTED_CAM_TYPE;
            break;

        case CAM_ERROR_NULL_POINTER:
            Result = XIL_VITIS_NET_P4_CAM_ERR_NULL_POINTER;
            break;

        case CAM_ERROR_TOO_MANY_PCS:
            Result = XIL_VITIS_NET_P4_CAM_ERR_TOO_MANY_PCS;
            break;

        case CAM_ERROR_CONFIGURATION:
            Result = XIL_VITIS_NET_P4_CAM_ERR_CONFIGURATION;
            break;

        case CAM_ERROR_ENVIRONMENT_FSMBUSY:
            Result = XIL_VITIS_NET_P4_CAM_ERR_ENVIRONMENT_FSMBUSY;
            break;

        case CAM_ERROR_ENVIRONMENT_POLLED_OUT:
            Result = XIL_VITIS_NET_P4_CAM_ERR_ENVIRONMENT_POLLED_OUT;
            break;

        case CAM_ERROR_SEGMENT_COUNT_ZERO:
            Result = XIL_VITIS_NET_P4_CAM_ERR_SEGMENT_COUNT_ZERO;
            break;

        case CAM_ERROR_VC_UNSUPPORTED_COMMAND:
            Result = XIL_VITIS_NET_P4_CAM_ERR_VC_UNSUPPORTED_COMMAND;
            break;

        case CAM_ERROR_WRONG_VIRTUAL_CAM_ID:
            Result = XIL_VITIS_NET_P4_CAM_ERR_WRONG_VIRTUAL_CAM_ID;
            break;

        case CAM_ERROR_WRONG_SEGMENT_NUMBER:
            Result = XIL_VITIS_NET_P4_CAM_ERR_WRONG_SEGMENT_NUMBER;
            break;

        case CAM_ERROR_TOO_MANY_RAMS:
            Result = XIL_VITIS_NET_P4_CAM_ERR_TOO_MANY_RAMS;
            break;

        case CAM_ERROR_TOO_SMALL_HEAP:
            Result = XIL_VITIS_NET_P4_CAM_ERR_TOO_SMALL_HEAP;
            break;

        case CAM_ERROR_RAM_TOO_SMALL:
            Result = XIL_VITIS_NET_P4_CAM_ERR_RAM_TOO_SMALL;
            break;

        case CAM_ERROR_NOMEM:
            Result = XIL_VITIS_NET_P4_CAM_ERR_NOMEM;
            break;

        case CAM_ERROR_MALLOC_FAILED:
            Result = XIL_VITIS_NET_P4_CAM_ERR_MALLOC_FAILED;
            break;

        default:
            break;
    }

    return Result;
}


static XilVitisNetP4ReturnType CamTopPrivCamConvertOptTypeToCamOptValue(XilVitisNetP4CamOptimizationType VitisNetP4OptType, uint32_t *CamOptValuePtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;

    switch (VitisNetP4OptType)
    {
        case XIL_VITIS_NET_P4_CAM_OPTIMIZE_NONE:
            *CamOptValuePtr = CAM_OPTIMIZE_NONE;
            break;
        case XIL_VITIS_NET_P4_CAM_OPTIMIZE_RAM:
            *CamOptValuePtr = CAM_OPTIMIZE_RAM;
            break;
        case XIL_VITIS_NET_P4_CAM_OPTIMIZE_LOGIC:
            *CamOptValuePtr = CAM_OPTIMIZE_LOGIC;
            break;
        default:
            Result = XIL_VITIS_NET_P4_CAM_ERR_OPTIMIZATION_TYPE_UNKNOWN;
            break;
    }

    return Result;
}

static void CamTopPrivCamHwWrite(void *CamCtxPtr, uint32_t Offset, uint32_t Data)
{
    XilVitisNetP4CamCtx *CtxPtr = CamCtxPtr;
    XilVitisNetP4EnvIf *EnvIfPtr = NULL;
    XilVitisNetP4AddressType Address = Offset;

    if (CtxPtr != NULL)
    {
        Address += CtxPtr->BaseAddr;
        EnvIfPtr = &(CtxPtr->EnvIf);
    }

    if (EnvIfPtr != NULL)
    {
        EnvIfPtr->WordWrite32(EnvIfPtr, Address, Data);
    }
}

static void CamTopPrivCamHwRead(void *CamCtxPtr, uint32_t Offset, uint32_t *DataPtr)
{
    XilVitisNetP4CamCtx *CtxPtr = CamCtxPtr;
    XilVitisNetP4EnvIf *EnvIfPtr = NULL;
    XilVitisNetP4AddressType Address = Offset;

    if (CtxPtr != NULL)
    {
        Address += CtxPtr->BaseAddr;
        EnvIfPtr = &(CtxPtr->EnvIf);
    }

    if ((EnvIfPtr != NULL) && (DataPtr != NULL))
    {
        EnvIfPtr->WordRead32(EnvIfPtr, Address, DataPtr);
    }
}

static void CamTopPrivCamLogError(void *CamCtxPtr, char *MessagePtr)
{
    XilVitisNetP4CamCtx *CtxPtr = CamCtxPtr;
    XilVitisNetP4EnvIf *EnvIfPtr = NULL;

    if (CtxPtr != NULL)
    {
        EnvIfPtr = &(CtxPtr->EnvIf);
    }

    if ((EnvIfPtr != NULL) && (MessagePtr != NULL))
    {
        EnvIfPtr->LogError(EnvIfPtr, MessagePtr);
    }
}

static void CamTopPrivCamLogInfo(void *CamCtxPtr, char *MessagePtr)
{
    XilVitisNetP4CamCtx *CtxPtr = CamCtxPtr;
    XilVitisNetP4EnvIf *EnvIfPtr = NULL;

    if (CtxPtr != NULL)
    {
        EnvIfPtr = &(CtxPtr->EnvIf);
    }

    if ((EnvIfPtr != NULL) && (MessagePtr != NULL))
    {
        EnvIfPtr->LogInfo(EnvIfPtr, MessagePtr);
    }
}

XilVitisNetP4ReturnType XilVitisNetP4CamValidateCtx(XilVitisNetP4CamCtx *CtxPtr)
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

static XilVitisNetP4ReturnType CamTopPrivDcamValidateConfig(XilVitisNetP4CamConfig *ConfigPtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;

    if (ConfigPtr == NULL)
    {
        Result = XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        if ((ConfigPtr->ResponseSizeBits == 0) || (ConfigPtr->ResponseSizeBits > XIL_VITIS_NET_P4_DCAM_MAX_RESP_SIZE_BITS))
        {
            Result = XIL_VITIS_NET_P4_CAM_ERR_WRONG_RESPONSE_WIDTH;
        }
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        if (ConfigPtr->NumEntries == 0)
        {
            Result = XIL_VITIS_NET_P4_CAM_ERR_INVALID_NUM_ENTRIES;
        }
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        switch (ConfigPtr->Endian)
        {
            /* Deliberate fall-through execution to validate parameter */
            case XIL_VITIS_NET_P4_LITTLE_ENDIAN:
            case XIL_VITIS_NET_P4_BIG_ENDIAN:
                break;

            default:
                Result = XIL_VITIS_NET_P4_CAM_ERR_INVALID_ENDIAN;
                break;
        }
    }

    return Result;
}

static XilVitisNetP4ReturnType CamTopPrivDcamValidateDataBuffer(XilVitisNetP4CamCtx *CtxPtr, uint8_t *DataPtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;

    if (CtxPtr == NULL)
    {
        Result = XIL_VITIS_NET_P4_GENERAL_ERR_INTERNAL_ASSERTION;
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        if (DataPtr == NULL)
        {
            Result = XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
        }
    }

    return Result;
}

static XilVitisNetP4ReturnType CamTopPrivDcamValidateKey(XilVitisNetP4CamCtx *CtxPtr, uint32_t Key)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;

    if (CtxPtr == NULL)
    {
        Result = XIL_VITIS_NET_P4_GENERAL_ERR_INTERNAL_ASSERTION;
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        if (Key >= CtxPtr->NumEntries)
        {
            return XIL_VITIS_NET_P4_CAM_ERR_INVALID_KEY;
        }
    }

    return Result;
}

static XilVitisNetP4ReturnType CamTopPrivDcamGetEntryStatus(XilVitisNetP4CamCtx *CtxPtr, uint32_t Key)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    uint32_t HitFlag = 0;

    /* Checks */
    Result = XilVitisNetP4CamValidateCtx(CtxPtr);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }

    Result = CamTopPrivDcamValidateKey(CtxPtr, Key);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }

    /* Check if hitflag is 1 or 0 */
    Result = CtxPtr->EnvIf.WordWrite32(&(CtxPtr->EnvIf), (CtxPtr->BaseAddr + XIL_VITIS_NET_P4_DCAM_ADDRESS_REG), Key);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }

    Result = CtxPtr->EnvIf.WordWrite32(&(CtxPtr->EnvIf), (CtxPtr->BaseAddr + XIL_VITIS_NET_P4_DCAM_CONTROL_STATUS_REG), XIL_VITIS_NET_P4_DCAM_READ);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }

    Result = CtxPtr->EnvIf.WordRead32(&(CtxPtr->EnvIf), (CtxPtr->BaseAddr + XIL_VITIS_NET_P4_DCAM_CONTROL_STATUS_REG), &HitFlag);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }

    /* If flag is set to 0, entry is not in use */
    Result = XIL_VITIS_NET_P4_CAM_ERR_KEY_NOT_FOUND;

    /* If flag is set to 1, entry is in use */
    if ((HitFlag & XIL_VITIS_NET_P4_DCAM_HIT) != 0)
    {
        Result = XIL_VITIS_NET_P4_CAM_ERR_DUPLICATE_FOUND;
    }

    return Result;
}

static XilVitisNetP4ReturnType CamTopPrivDcamWriteEntry(XilVitisNetP4CamCtx *CtxPtr, uint32_t Key, uint8_t *DataPtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    XilVitisNetP4AddressType Address = 0;
    uint32_t DataIndex = 0;
    uint32_t DataOffset = 0;
    uint32_t ByteOffset = 0;
    uint32_t RegValue = 0;

    /* Check input parameters */
    Result = XilVitisNetP4CamValidateCtx(CtxPtr);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }

    Result = CamTopPrivDcamValidateKey(CtxPtr, Key);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }

    Result = CamTopPrivDcamValidateDataBuffer(CtxPtr, DataPtr);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }

    /* Compute starting address of response data */
    Address = (CtxPtr->BaseAddr + XIL_VITIS_NET_P4_DCAM_DATA_REG_1);

    /* Package byte array into 32-bit chunks for writing to HW memory */
    for (DataIndex = 0; DataIndex < CtxPtr->ResponseBufSizeBytes; DataIndex++)
    {
        ByteOffset = DataIndex % sizeof(uint32_t);

        if (CtxPtr->Endian == XIL_VITIS_NET_P4_BIG_ENDIAN)
        {
            DataOffset = CtxPtr->ResponseBufSizeBytes - DataIndex - 1;
        }
        else
        {
            DataOffset = DataIndex;
        }

        RegValue |= DataPtr[DataOffset] << (ByteOffset * XIL_VITIS_NET_P4_BITS_PER_BYTE);

        if ((ByteOffset == sizeof(uint32_t) - 1) || (DataIndex == CtxPtr->ResponseBufSizeBytes - 1))
        {
            Result = CtxPtr->EnvIf.WordWrite32(&(CtxPtr->EnvIf), Address, RegValue);
            if (Result != XIL_VITIS_NET_P4_SUCCESS)
            {
                return Result;
            }

            RegValue = 0;
            Address += sizeof(uint32_t);
        }
    }

    return Result;
}

static XilVitisNetP4ReturnType CamTopPrivDcamReadEntry(XilVitisNetP4CamCtx *CtxPtr, uint32_t Key, uint8_t *DataPtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    XilVitisNetP4AddressType Address = 0;
    uint32_t DataIndex = 0;
    uint32_t DataOffset = 0;
    uint32_t ByteOffset = 0;
    uint32_t RegValue = 0;

    /* Check input parameters */
    Result = XilVitisNetP4CamValidateCtx(CtxPtr);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }

    Result = CamTopPrivDcamValidateKey(CtxPtr, Key);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }

    Result = CamTopPrivDcamValidateDataBuffer(CtxPtr, DataPtr);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }

    /* Compute starting address of response data */
    Address = (CtxPtr->BaseAddr + XIL_VITIS_NET_P4_DCAM_DATA_REG_1);

    /* Read data in 32-bit chunks and unpack into provided byte array */
    for (DataIndex = 0; DataIndex < CtxPtr->ResponseBufSizeBytes; DataIndex++)
    {
        ByteOffset = DataIndex % sizeof(uint32_t);

        if (CtxPtr->Endian == XIL_VITIS_NET_P4_BIG_ENDIAN)
        {
            DataOffset = CtxPtr->ResponseBufSizeBytes - DataIndex - 1;
        }
        else
        {
            DataOffset = DataIndex;
        }

        if (ByteOffset == 0)
        {
            Result = CtxPtr->EnvIf.WordRead32(&(CtxPtr->EnvIf), Address, &RegValue);
            if (Result != XIL_VITIS_NET_P4_SUCCESS)
            {
                return Result;
            }

            Address += sizeof(uint32_t);
        }

        DataPtr[DataOffset] = (RegValue & ((1 << XIL_VITIS_NET_P4_BITS_PER_BYTE) - 1));
        RegValue >>= XIL_VITIS_NET_P4_BITS_PER_BYTE;
    }

    return Result;
}

static XilVitisNetP4ReturnType CamTopPrivDcamSetEntryStatus(XilVitisNetP4CamCtx *CtxPtr, uint32_t Key, bool Status)
{
    XilVitisNetP4ReturnType Result;

    Result = CtxPtr->EnvIf.WordWrite32(&(CtxPtr->EnvIf), (CtxPtr->BaseAddr + XIL_VITIS_NET_P4_DCAM_ADDRESS_REG), Key);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }

    if (Status == true)
    {
        Result = CtxPtr->EnvIf.WordWrite32(&(CtxPtr->EnvIf), (CtxPtr->BaseAddr + XIL_VITIS_NET_P4_DCAM_CONTROL_STATUS_REG), XIL_VITIS_NET_P4_DCAM_WRITE_AND_HIT);
        if (Result != XIL_VITIS_NET_P4_SUCCESS)
        {
            return Result;
        }
    }
    else
    {
        Result = CtxPtr->EnvIf.WordWrite32(&(CtxPtr->EnvIf), (CtxPtr->BaseAddr + XIL_VITIS_NET_P4_DCAM_CONTROL_STATUS_REG), XIL_VITIS_NET_P4_DCAM_WRITE_AND_CLEAR);
        if (Result != XIL_VITIS_NET_P4_SUCCESS)
        {
            return Result;
        }
    }

    return Result;
}

static XilVitisNetP4ReturnType CamTopPrivDcamCheckMatch(XilVitisNetP4CamCtx *CtxPtr,
                                             uint8_t *ResponsePtr,
                                             uint8_t *ResponseMaskPtr,
                                             uint8_t *EntryPtr,
                                             bool *MatchFoundPtr)
{
    bool MatchFound = true;
    uint32_t ByteIndex = 0;
    uint32_t X = 0;
    uint32_t Y = 0;
    uint32_t M = 0;
    uint32_t BufSizeBytes = CtxPtr->ResponseBufSizeBytes;
    uint32_t PaddingBits = CtxPtr->ResponsePaddingSizeBits;
    uint8_t PaddingMask = 0;
    uint32_t ByteOffset = 0;

    PaddingMask = XIL_VITIS_NET_P4_BYTE_MASK << (XIL_VITIS_NET_P4_BITS_PER_BYTE - PaddingBits);

    /* Iterate over the data and test for equality of all bits based on the asserted bits in the response mask */
    for (ByteIndex = 0; ByteIndex < BufSizeBytes; ByteIndex++)
    {
        if (CtxPtr->Endian == XIL_VITIS_NET_P4_BIG_ENDIAN)
        {
            ByteOffset = BufSizeBytes - ByteIndex - 1;
        }
        else
        {
            ByteOffset = ByteIndex;
        }

        M = ResponseMaskPtr[ByteOffset];
        X = ResponsePtr[ByteOffset];
        Y = EntryPtr[ByteOffset];

        /* Special case on the last byte - ignore any padding bits in the byte array by forcing them to an equal value */
        if ((PaddingBits != 0) && (ByteIndex == (BufSizeBytes - 1)))
        {
            M |= PaddingMask;
            X |= PaddingMask;
            Y |= PaddingMask;
        }

        /* If a mismatch is detected on any byte, stop and discard */
        if ((X & M) != (Y & M))
        {
            MatchFound = false;
            break;
        }
    }

    *MatchFoundPtr = MatchFound;

    return XIL_VITIS_NET_P4_SUCCESS;
}
