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
-- Revision       : $Revision: #2 $
-- Date           : $DateTime: 2021/03/23 10:58:14 $
-- Last Author    : $Author: jaimeh $
--
--------------------------------------------------------------------------------
-- Description : Implementation of top level VitisNetP4 Tiny Cam driver
--
*/
/****************************************************************************************************************************************************/
/* SECTION: Header includes */
/****************************************************************************************************************************************************/
#include "tiny_cam.h"
#include "bcam.h"
#include "cam_priv.h"
#include "cam_top.h"
#include "stcam.h"
#include "tcam.h"
#include <string.h>

/****************************************************************************************************************************************************/
/* SECTION: Constants/macros */
/****************************************************************************************************************************************************/
#define XIL_VITIS_NET_P4_TINYCAM_MAX_RESP_SIZE_BITS (7168)
#define XIL_VITIS_NET_P4_TINYCAM_MAX_KEY_SIZE_BITS  (2048)
#define XIL_VITIS_NET_P4_TINYCAM_TABLE_SIZE_BITS    (0x20000) /* 128K */
#define XIL_VITIS_NET_P4_TCAM_MAX_RESP_SIZE_BYTES   (XIL_VITIS_NET_P4_TINYCAM_MAX_RESP_SIZE_BITS / XIL_VITIS_NET_P4_BITS_PER_BYTE)

/* Register map */
#define XIL_VITIS_NET_P4_TINY_CAM_CONTROL_REG            (0x00)
#define XIL_VITIS_NET_P4_TINY_CAM_ENTRY_ID_REG           (0x04)
#define XIL_VITIS_NET_P4_TINY_CAM_EMULATION_MODE_REG     (0x08)
#define XIL_VITIS_NET_P4_TINY_CAM_LOOKUP_COUNT_REG       (0x0c)
#define XIL_VITIS_NET_P4_TINY_CAM_HIT_COUNT_REG          (0x10)
#define XIL_VITIS_NET_P4_TINY_CAM_MISS_COUNT_REG         (0x14)
#define XIL_VITIS_NET_P4_TINY_CAM_DATA_REG_PART_1_REG    (0x40)
#define XIL_VITIS_NET_P4_TINY_CAM_DATA_REG_PART_2032_REG (0x1FFC)

/* Control/status register bit masks */
#define XIL_VITIS_NET_P4_TINY_CAM_READ_MASK                 (1UL << 0)
#define XIL_VITIS_NET_P4_TINY_CAM_WRITE_MASK                (1UL << 1)
#define XIL_VITIS_NET_P4_TINY_CAM_ENTRY_IN_USE_MASK         (1UL << 31)
#define XIL_VITIS_NET_P4_TINY_CAM_ENTRY_IN_USE_BIT          (31)
#define XIL_VITIS_NET_P4_TINY_CAM_ENTRY_IN_USE_ENTRY_EXISTS (1)

/* Control register command definitions */
#define XIL_VITIS_NET_P4_TINY_CAM_READ                       (XIL_VITIS_NET_P4_TINY_CAM_READ_MASK)
#define XIL_VITIS_NET_P4_TINY_CAM_WRITE                      (XIL_VITIS_NET_P4_TINY_CAM_WRITE_MASK)
#define XIL_VITIS_NET_P4_TINY_CAM_WRITE_AND_ENTRY_IN_USE     (XIL_VITIS_NET_P4_TINY_CAM_WRITE_MASK | XIL_VITIS_NET_P4_TINY_CAM_ENTRY_IN_USE_MASK)
#define XIL_VITIS_NET_P4_TINY_CAM_ENTRY_IN_USE_ADDED_UPDATED (XIL_VITIS_NET_P4_TINY_CAM_ENTRY_IN_USE_MASK)
#define XIL_VITIS_NET_P4_TINY_CAM_DELETE                     (XIL_VITIS_NET_P4_TINY_CAM_WRITE)

/* Emulation Mode Register */
#define XIL_VITIS_NET_P4_TINY_CAM_EMULATION_MODE_MASK     (1UL << 0)
#define XIL_VITIS_NET_P4_TINY_CAM_HW_BCAM_EMULATION_VALUE (0)
#define XIL_VITIS_NET_P4_TINY_CAM_HW_TCAM_EMULATION_VALUE (1)

#define XIL_VITIS_NET_P4_TINY_CAM_ZERO_BYTE_PADDING 0x00


/****************************************************************************************************************************************************/
/* SECTION: Type defintions */
/****************************************************************************************************************************************************/
typedef enum XilVitisNetP4GetTableEntryCmd
{
    XIL_VITIS_NET_P4_TINY_CAM_GET_KEY,
    XIL_VITIS_NET_P4_TINY_CAM_GET_KEY_MASK,
    XIL_VITIS_NET_P4_TINY_CAM_GET_RESPONSE
} XilVitisNetP4GetTableEntryCmd;

typedef enum XilVitisNetP4Operation
{
    XIL_VITIS_NET_P4_TINY_CAM_OP_INSERT,
    XIL_VITIS_NET_P4_TINY_CAM_OP_UPDATE,
    XIL_VITIS_NET_P4_TINY_CAM_OP_DELETE
} XilVitisNetP4Operation;

/****************************************************************************************************************************************************/
/* SECTION: Local function declarations */
/****************************************************************************************************************************************************/

static XilVitisNetP4ReturnType TinyCamBcamDelete(XilVitisNetP4TinyCamPrvCtx *PrivateCtxPtr, uint8_t *KeyPtr, uint8_t *MaskPtr);

static XilVitisNetP4ReturnType TinyTcamInsert(XilVitisNetP4TinyCamPrvCtx *PrivateCtxPtr,
                                         uint8_t *KeyPtr,
                                         uint8_t *MaskPtr,
                                         uint32_t Priority,
                                         uint8_t *ResponsePtr);
static XilVitisNetP4ReturnType TinyCamTcamDelete(XilVitisNetP4TinyCamPrvCtx *PrivateCtxPtr, uint8_t *KeyPtr, uint8_t *MaskPtr);

static XilVitisNetP4ReturnType TinyCamCommonInit(XilVitisNetP4TinyCamPrvCtx **PrivateCtxPtrPtr,
                                            const XilVitisNetP4EnvIf *EnvIfPtr,
                                            XilVitisNetP4CamConfig *ConfigPtr,
                                            XilVitisNetP4TinyCamEmulationMode EmulationMode,
                                            bool CalcHeapSize,
                                            ...);
static XilVitisNetP4ReturnType TinyCamCommonExit(XilVitisNetP4TinyCamPrvCtx *PrivateCtxPtr);
static XilVitisNetP4ReturnType TinyCamCommonUpdate(XilVitisNetP4TinyCamPrvCtx *PrivateCtxPtr, uint8_t *KeyPtr, uint8_t *MaskPtr, uint8_t *ResponsePtr);
static XilVitisNetP4ReturnType TinyCamCommonGetByResponse(XilVitisNetP4TinyCamPrvCtx *PrivateCtxPtr,
                                                     uint8_t *ResponsePtr,
                                                     uint8_t *ResponseMaskPtr,
                                                     uint32_t *PositionPtr,
                                                     XilVitisNetP4TinyCamEntry **FoundEntryPtrPtr);
static XilVitisNetP4ReturnType TinyCamCommonGetByKey(XilVitisNetP4TinyCamPrvCtx *PrivateCtxPtr,
                                                uint8_t *KeyPtr,
                                                uint8_t *MaskPtr,
                                                XilVitisNetP4TinyCamEntry **FoundEntryPtrPtr);
static XilVitisNetP4ReturnType TinyCamCommonReset(XilVitisNetP4TinyCamPrvCtx *PrivateCtxPtr);
static XilVitisNetP4ReturnType TinyCamCommonGetEccCountersClearOnRead(XilVitisNetP4TinyCamPrvCtx *PrivateCtxPtr,
                                                                 uint32_t *CorrectedSingleBitErrorsPtr,
                                                                 uint32_t *DetectedDoubleBitErrorsPtr);
static XilVitisNetP4ReturnType TinyCamCommonGetEccAddressesClearOnRead(XilVitisNetP4TinyCamPrvCtx *PrivateCtxPtr,
                                                                  uint32_t *FailingAddressSingleBitErrorPtr,
                                                                  uint32_t *FailingAddressDoubleBitErrorPtr);

static XilVitisNetP4ReturnType TinyCamCommonRewrite(XilVitisNetP4TinyCamPrvCtx *PrivateCtxPtr);
static XilVitisNetP4ReturnType TinyCamMallocPrivateCtxBuffers(XilVitisNetP4TinyCamPrvCtx* PrivateCtxPtr, bool CalcHeapSize, ...);
static XilVitisNetP4ReturnType TinyCamFreePrivateCtxBuffers(XilVitisNetP4TinyCamPrvCtx *PrivateCtxPtr);

static XilVitisNetP4ReturnType TinyCamHwWriteEntryDataRegs(XilVitisNetP4TinyCamPrvCtx *TinyCamPrvCtxPtr, uint8_t *BufferPtr, uint32_t NumReg32Writes);
static XilVitisNetP4ReturnType XilVitisNetP4TinyCamCalcHeapShadowTable(XilVitisNetP4TinyCamPrvCtx* TinyCamPrvCtxPtr, uint64_t* HeapSizePtr);
static XilVitisNetP4ReturnType TinyCamHwSetControlRegisterEntryInUseAndWrFlag(XilVitisNetP4TinyCamPrvCtx *TinyCamPrvCtxPtr);
static XilVitisNetP4ReturnType TinyCamHwSetControlRegisterEntryInUseDeletedFlag(XilVitisNetP4TinyCamPrvCtx *TinyCamPrvCtxPtr);
static XilVitisNetP4ReturnType TinyCamHwGetControlRegisterEntryInUseBit(XilVitisNetP4TinyCamPrvCtx *TinyCamPrvCtxPtr, uint32_t *EntryInUseBitPtr);
static XilVitisNetP4ReturnType TinyCamHwSetControlRegisterRdFlag(XilVitisNetP4TinyCamPrvCtx *TinyCamPrvCtxPtr);
static XilVitisNetP4ReturnType TinyCamHwSetEntryIdRegister(XilVitisNetP4TinyCamPrvCtx *TinyCamPrvCtxPtr, uint32_t EntryNum);
static XilVitisNetP4ReturnType TinyCamHwGetEmulationMode(XilVitisNetP4TinyCamPrvCtx *TinyCamPrvCtxPtr, XilVitisNetP4TinyCamEmulationMode *EmulationModePtr);
static XilVitisNetP4ReturnType TinyCamHwReadEntryDataRegs(XilVitisNetP4TinyCamPrvCtx *TinyCamPrvCtxPtr, uint8_t *BufferPtr, uint32_t NumReg32Reads);

/****************************************************************************************************************************************************/
/* SECTION:  public functions for TinyCam in BCAM Emulation Mode */
/****************************************************************************************************************************************************/

XilVitisNetP4ReturnType XilVitisNetP4TinyBcamInit(XilVitisNetP4TinyBcamCtx *CtxPtr, const XilVitisNetP4EnvIf *EnvIfPtr, XilVitisNetP4CamConfig *ConfigPtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    XilVitisNetP4TinyCamPrvCtx *PrivateTinyCamCtxPtr = NULL;

    if ((CtxPtr == NULL) || (EnvIfPtr == NULL) || (ConfigPtr == NULL))
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    Result = TinyCamCommonInit(&PrivateTinyCamCtxPtr, EnvIfPtr, ConfigPtr, XIL_VITIS_NET_P4_TINY_CAM_BCAM_EMULATION, false);
    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        CtxPtr->PrivateCtxPtr = PrivateTinyCamCtxPtr;
    }
    else
    {
        CtxPtr->PrivateCtxPtr = NULL;
    }

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4TinyBcamExit(XilVitisNetP4TinyBcamCtx *CtxPtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;

    /* Check input parameters */
    if (CtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_INVALID_CONTEXT;
    }

    Result = TinyCamCommonExit(CtxPtr->PrivateCtxPtr);

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        CtxPtr->PrivateCtxPtr = NULL;
    }

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4TinyBcamInsert(XilVitisNetP4TinyBcamCtx *CtxPtr, uint8_t *KeyPtr, uint8_t *ResponsePtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;

    uint8_t *KeyParamPtr = KeyPtr;
    uint8_t *ResponseParamPtr = ResponsePtr;
    XilVitisNetP4CamCtx *CamCtxPtr = NULL;
    XilVitisNetP4TinyCamPrvCtx *PrivateCtxPtr = NULL;
    XilVitisNetP4TinyCamEntry *EntryPtr = NULL; /* Null to ensure starts at Head */
    bool IsEntryEmpty;
    bool EntryMatches;

    if ((CtxPtr == NULL) || (KeyPtr == NULL) || (ResponsePtr == NULL))
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    if (CtxPtr->PrivateCtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_INVALID_CONTEXT;
    }

    if (CtxPtr->PrivateCtxPtr->PrivateVitisNetP4CamCtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_INVALID_CONTEXT;
    }

    PrivateCtxPtr = CtxPtr->PrivateCtxPtr;
    CamCtxPtr = PrivateCtxPtr->PrivateVitisNetP4CamCtxPtr;

    if (CamCtxPtr->Endian == XIL_VITIS_NET_P4_BIG_ENDIAN)
    {
        KeyParamPtr = CamCtxPtr->KeyBufPtr;
        Result = XilVitisNetP4CamByteSwap(CamCtxPtr, KeyPtr, KeyParamPtr, VITISNETP4_CAM_SWAP_KEY);
        if (Result == XIL_VITIS_NET_P4_SUCCESS)
        {
            ResponseParamPtr = CamCtxPtr->ResponseBufPtr;
            Result = XilVitisNetP4CamByteSwap(CamCtxPtr, ResponsePtr, ResponseParamPtr, VITISNETP4_CAM_SWAP_RESPONSE);
        }
    }

    /* search shadow table */
    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        Result = XilVitisNetP4TinyCamPrvShadowTableFindEntry(PrivateCtxPtr, &(PrivateCtxPtr->ShadowTable), KeyParamPtr, CamCtxPtr->MaskBufPtr, &EntryPtr);
    }

    /* Should not find the entry in the table */
    if ((Result != XIL_VITIS_NET_P4_CAM_ERR_KEY_NOT_FOUND) && (EntryPtr != NULL))
    {
        /* Check Matches with Hardware */
        Result = XilVitisNetP4TinyCamPrvIpTableCheckEntryMatches(PrivateCtxPtr, EntryPtr, &EntryMatches);
        if (Result == XIL_VITIS_NET_P4_SUCCESS)
        {
            if (EntryMatches == false)
            {
                /*this means that the Sahdow Table and Cam IP Table are out of sync */
                Result = XIL_VITIS_NET_P4_GENERAL_ERR_INTERNAL_ASSERTION;
            }
            else
            {
                Result = XIL_VITIS_NET_P4_CAM_ERR_DUPLICATE_FOUND;
            }
        }
    }
    else
    {
        EntryPtr = NULL;
        Result = XilVitisNetP4TinyCamPrvShadowTableNextEmptyEntry(&(PrivateCtxPtr->ShadowTable), &EntryPtr);

        if (Result == XIL_VITIS_NET_P4_SUCCESS)
        {
            Result = XilVitisNetP4TinyCamPrvIpTableCheckEntryEmpty(PrivateCtxPtr, EntryPtr, &IsEntryEmpty);
        }

        if (Result == XIL_VITIS_NET_P4_SUCCESS)
        {
            if (IsEntryEmpty == true)
            {
                Result = XilVitisNetP4TinyCamPrvShadowTableFillEntry(PrivateCtxPtr, EntryPtr, KeyParamPtr, CamCtxPtr->MaskBufPtr, 0, ResponseParamPtr);
            }
            else
            {
                Result = XIL_VITIS_NET_P4_GENERAL_ERR_INTERNAL_ASSERTION;
            }

            if (Result == XIL_VITIS_NET_P4_SUCCESS)
            {
                Result = XilVitisNetP4TinyCamPrvIpTableWriteEntry(PrivateCtxPtr, EntryPtr);
            }

            if (Result != XIL_VITIS_NET_P4_SUCCESS)
            {
                XilVitisNetP4TinyCamPrvShadowTableEmptyEntry(PrivateCtxPtr, EntryPtr);
            }
        }
    }

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4TinyBcamUpdate(XilVitisNetP4TinyBcamCtx *CtxPtr, uint8_t *KeyPtr, uint8_t *ResponsePtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;

    uint8_t *KeyParamPtr = KeyPtr;
    uint8_t *ResponseParamPtr = ResponsePtr;
    XilVitisNetP4CamCtx *CamCtxPtr = NULL;
    XilVitisNetP4TinyCamPrvCtx *PrivateCtxPtr = NULL;

    if ((CtxPtr == NULL) || (KeyPtr == NULL) || (ResponsePtr == NULL))
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    if (CtxPtr->PrivateCtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_INVALID_CONTEXT;
    }

    if (CtxPtr->PrivateCtxPtr->PrivateVitisNetP4CamCtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_INVALID_CONTEXT;
    }

    PrivateCtxPtr = CtxPtr->PrivateCtxPtr;
    CamCtxPtr = PrivateCtxPtr->PrivateVitisNetP4CamCtxPtr;

    if (CamCtxPtr->Endian == XIL_VITIS_NET_P4_BIG_ENDIAN)
    {
        KeyParamPtr = CamCtxPtr->KeyBufPtr;
        Result = XilVitisNetP4CamByteSwap(CamCtxPtr, KeyPtr, KeyParamPtr, VITISNETP4_CAM_SWAP_KEY);
        if (Result == XIL_VITIS_NET_P4_SUCCESS)
        {
            ResponseParamPtr = CamCtxPtr->ResponseBufPtr;
            Result = XilVitisNetP4CamByteSwap(CamCtxPtr, ResponsePtr, ResponseParamPtr, VITISNETP4_CAM_SWAP_RESPONSE);
        }
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        Result = TinyCamCommonUpdate(PrivateCtxPtr, KeyParamPtr, CamCtxPtr->MaskBufPtr, ResponseParamPtr);
    }

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4TinyBcamGetByResponse(XilVitisNetP4TinyBcamCtx *CtxPtr,
                                                 uint8_t *ResponsePtr,
                                                 uint8_t *ResponseMaskPtr,
                                                 uint32_t *PositionPtr,
                                                 uint8_t *KeyPtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    uint8_t *ResponseParamPtr = ResponsePtr;
    uint8_t *ResponseMaskParamPtr = ResponseMaskPtr;
    XilVitisNetP4CamCtx *CamCtxPtr = NULL;
    XilVitisNetP4TinyCamPrvCtx *PrivateCtxPtr = NULL;
    XilVitisNetP4TinyCamEntry *FoundEntryPtr;

    if ((CtxPtr == NULL) || (ResponsePtr == NULL) || (ResponseMaskPtr == NULL) || (PositionPtr == NULL) || (KeyPtr == NULL))
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    if (CtxPtr->PrivateCtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_INVALID_CONTEXT;
    }

    if (CtxPtr->PrivateCtxPtr->PrivateVitisNetP4CamCtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_INVALID_CONTEXT;
    }

    PrivateCtxPtr = CtxPtr->PrivateCtxPtr;
    CamCtxPtr = PrivateCtxPtr->PrivateVitisNetP4CamCtxPtr;

    /* At the end will not find any more keys */
    if (*PositionPtr >= CamCtxPtr->NumEntries)
    {
        return XIL_VITIS_NET_P4_CAM_ERR_KEY_NOT_FOUND;
    }

    if (CamCtxPtr->Endian == XIL_VITIS_NET_P4_BIG_ENDIAN)
    {
        ResponseParamPtr = CamCtxPtr->ResponseBufPtr;
        Result = XilVitisNetP4CamByteSwap(CamCtxPtr, ResponsePtr, ResponseParamPtr, VITISNETP4_CAM_SWAP_RESPONSE);
        if (Result == XIL_VITIS_NET_P4_SUCCESS)
        {
            ResponseMaskParamPtr = CamCtxPtr->ResponseMaskBufPtr;
            Result = XilVitisNetP4CamByteSwap(CamCtxPtr, ResponseMaskPtr, ResponseMaskParamPtr, VITISNETP4_CAM_SWAP_RESPONSE_MASK);
        }
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        Result = TinyCamCommonGetByResponse(PrivateCtxPtr, ResponseParamPtr, ResponseMaskParamPtr, PositionPtr, &FoundEntryPtr);
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        if (CamCtxPtr->Endian == XIL_VITIS_NET_P4_BIG_ENDIAN)
        {
            Result = XilVitisNetP4CamByteSwap(CamCtxPtr, FoundEntryPtr->KeyPtr, KeyPtr, VITISNETP4_CAM_SWAP_KEY);
        }
        else
        {
            memcpy(KeyPtr, FoundEntryPtr->KeyPtr, CamCtxPtr->KeyBufSizeBytes);
        }

        *PositionPtr = FoundEntryPtr->EntryId + 1;
    }

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4TinyBcamGetByKey(XilVitisNetP4TinyBcamCtx *CtxPtr, uint8_t *KeyPtr, uint8_t *ResponsePtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    XilVitisNetP4CamCtx *CamCtxPtr = NULL;
    XilVitisNetP4TinyCamPrvCtx *PrivateCtxPtr = NULL;
    XilVitisNetP4TinyCamEntry *FoundEntryPtr = NULL; /* Ensure search starts at head */
    uint8_t *KeyParamPtr = KeyPtr;

    if ((CtxPtr == NULL) || (KeyPtr == NULL) || (ResponsePtr == NULL))
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    if (CtxPtr->PrivateCtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_INVALID_CONTEXT;
    }

    if (CtxPtr->PrivateCtxPtr->PrivateVitisNetP4CamCtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_INVALID_CONTEXT;
    }

    PrivateCtxPtr = CtxPtr->PrivateCtxPtr;
    CamCtxPtr = PrivateCtxPtr->PrivateVitisNetP4CamCtxPtr;

    if (CamCtxPtr->Endian == XIL_VITIS_NET_P4_BIG_ENDIAN)
    {
        KeyParamPtr = CamCtxPtr->KeyBufPtr;
        Result = XilVitisNetP4CamByteSwap(CamCtxPtr, KeyPtr, KeyParamPtr, VITISNETP4_CAM_SWAP_KEY);
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        Result = TinyCamCommonGetByKey(PrivateCtxPtr, KeyParamPtr, CamCtxPtr->MaskBufPtr, &FoundEntryPtr);
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        if (CamCtxPtr->Endian == XIL_VITIS_NET_P4_BIG_ENDIAN)
        {
            Result = XilVitisNetP4CamByteSwap(CamCtxPtr, FoundEntryPtr->ResponsePtr, ResponsePtr, VITISNETP4_CAM_SWAP_RESPONSE);
        }
        else
        {
            memcpy(ResponsePtr, FoundEntryPtr->ResponsePtr, CamCtxPtr->ResponseBufSizeBytes);
        }
    }

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4TinyBcamDelete(XilVitisNetP4TinyBcamCtx *CtxPtr, uint8_t *KeyPtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    uint8_t *KeyParamPtr = KeyPtr;
    XilVitisNetP4CamCtx *CamCtxPtr = NULL;
    XilVitisNetP4TinyCamPrvCtx *PrivateCtxPtr = NULL;

    if ((CtxPtr == NULL) || (KeyPtr == NULL))
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    if (CtxPtr->PrivateCtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_INVALID_CONTEXT;
    }

    if (CtxPtr->PrivateCtxPtr->PrivateVitisNetP4CamCtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_INVALID_CONTEXT;
    }

    PrivateCtxPtr = CtxPtr->PrivateCtxPtr;
    CamCtxPtr = PrivateCtxPtr->PrivateVitisNetP4CamCtxPtr;

    if (CamCtxPtr->Endian == XIL_VITIS_NET_P4_BIG_ENDIAN)
    {
        KeyParamPtr = CamCtxPtr->KeyBufPtr;
        Result = XilVitisNetP4CamByteSwap(CamCtxPtr, KeyPtr, KeyParamPtr, VITISNETP4_CAM_SWAP_KEY);
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        Result = TinyCamBcamDelete(PrivateCtxPtr, KeyParamPtr, CamCtxPtr->MaskBufPtr);
    }

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4TinyBcamReset(XilVitisNetP4TinyBcamCtx *CtxPtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    XilVitisNetP4TinyCamPrvCtx *PrivateCtxPtr;

    if (CtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    if (CtxPtr->PrivateCtxPtr->PrivateVitisNetP4CamCtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_INVALID_CONTEXT;
    }

    PrivateCtxPtr = CtxPtr->PrivateCtxPtr;

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        Result = TinyCamCommonReset(PrivateCtxPtr);
    }

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4TinyBcamGetEccCountersClearOnRead(XilVitisNetP4TinyBcamCtx *CtxPtr,
                                                             uint32_t *CorrectedSingleBitErrorsPtr,
                                                             uint32_t *DetectedDoubleBitErrorsPtr)
{
    XilVitisNetP4TinyCamPrvCtx *PrivateCtxPtr;

    if ((CtxPtr == NULL) || (CorrectedSingleBitErrorsPtr == NULL) || (DetectedDoubleBitErrorsPtr == NULL))
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    if (CtxPtr->PrivateCtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_INVALID_CONTEXT;
    }

    PrivateCtxPtr = CtxPtr->PrivateCtxPtr;

    return TinyCamCommonGetEccCountersClearOnRead(PrivateCtxPtr, CorrectedSingleBitErrorsPtr, DetectedDoubleBitErrorsPtr);
}

XilVitisNetP4ReturnType XilVitisNetP4TinyBcamGetEccAddressesClearOnRead(XilVitisNetP4TinyBcamCtx *CtxPtr,
                                                              uint32_t *FailingAddressSingleBitErrorPtr,
                                                              uint32_t *FailingAddressDoubleBitErrorPtr)
{
    XilVitisNetP4TinyCamPrvCtx *PrivateCtxPtr;

    if ((CtxPtr == NULL) || (FailingAddressSingleBitErrorPtr == NULL) || (FailingAddressDoubleBitErrorPtr == NULL))
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    if (CtxPtr->PrivateCtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_INVALID_CONTEXT;
    }

    PrivateCtxPtr = CtxPtr->PrivateCtxPtr;

    return TinyCamCommonGetEccAddressesClearOnRead(PrivateCtxPtr, FailingAddressSingleBitErrorPtr, FailingAddressDoubleBitErrorPtr);
}

XilVitisNetP4ReturnType XilVitisNetP4TinyBcamRewrite(XilVitisNetP4TinyBcamCtx *CtxPtr)
{
    XilVitisNetP4TinyCamPrvCtx *PrivateCtxPtr;

    if (CtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    if (CtxPtr->PrivateCtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_INVALID_CONTEXT;
    }

    PrivateCtxPtr = CtxPtr->PrivateCtxPtr;

    return TinyCamCommonRewrite(PrivateCtxPtr);
}

/****************************************************************************************************************************************************/
/* SECTION:  Tiny Cam for TCAM Emulation mode*/
/****************************************************************************************************************************************************/

XilVitisNetP4ReturnType XilVitisNetP4TinyTcamInit(XilVitisNetP4TinyTcamCtx *CtxPtr, const XilVitisNetP4EnvIf *EnvIfPtr, XilVitisNetP4CamConfig *ConfigPtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    XilVitisNetP4TinyCamPrvCtx *PrivateTinyCamCtxPtr = NULL;

    if ((CtxPtr == NULL) || (EnvIfPtr == NULL) || (ConfigPtr == NULL))
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    Result = TinyCamCommonInit(&PrivateTinyCamCtxPtr, EnvIfPtr, ConfigPtr, XIL_VITIS_NET_P4_TINY_CAM_TCAM_EMULATION, false);
    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        CtxPtr->PrivateCtxPtr = PrivateTinyCamCtxPtr;
    }
    else
    {
        CtxPtr->PrivateCtxPtr = NULL;
    }

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4TinyTcamExit(XilVitisNetP4TinyTcamCtx *CtxPtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;

    /* Check input parameters */
    if (CtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_INVALID_CONTEXT;
    }

    Result = TinyCamCommonExit(CtxPtr->PrivateCtxPtr);

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        CtxPtr->PrivateCtxPtr = NULL;
    }

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4TinyTcamInsert(XilVitisNetP4TinyTcamCtx *CtxPtr, uint8_t *KeyPtr, uint8_t *MaskPtr, uint32_t Priority, uint8_t *ResponsePtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    XilVitisNetP4CamCtx *CamCtxPtr = NULL;
    XilVitisNetP4TinyCamPrvCtx *PrivateCtxPtr = NULL;
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

    if (CtxPtr->PrivateCtxPtr->PrivateVitisNetP4CamCtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_INVALID_CONTEXT;
    }

    PrivateCtxPtr = CtxPtr->PrivateCtxPtr;
    CamCtxPtr = PrivateCtxPtr->PrivateVitisNetP4CamCtxPtr;

    /* The maximum priority equals the number of entries */
    if (Priority >= CamCtxPtr->NumEntries)
    {
        return XIL_VITIS_NET_P4_CAM_ERR_WRONG_PRIO;
    }

    if (CamCtxPtr->Endian == XIL_VITIS_NET_P4_BIG_ENDIAN)
    {
        KeyParamPtr = CamCtxPtr->KeyBufPtr;
        Result = XilVitisNetP4CamByteSwap(CamCtxPtr, KeyPtr, KeyParamPtr, VITISNETP4_CAM_SWAP_KEY);
        if (Result == XIL_VITIS_NET_P4_SUCCESS)
        {
            MaskParamPtr = CamCtxPtr->MaskBufPtr;
            Result = XilVitisNetP4CamByteSwap(CamCtxPtr, MaskPtr, MaskParamPtr, VITISNETP4_CAM_SWAP_KEY_MASK);
            if (Result == XIL_VITIS_NET_P4_SUCCESS)
            {
                ResponseParamPtr = CamCtxPtr->ResponseBufPtr;
                Result = XilVitisNetP4CamByteSwap(CamCtxPtr, ResponsePtr, ResponseParamPtr, VITISNETP4_CAM_SWAP_RESPONSE);
            }
        }
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        Result = TinyTcamInsert(PrivateCtxPtr, KeyParamPtr, MaskParamPtr, Priority, ResponseParamPtr);
    }
    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4TinyTcamUpdate(XilVitisNetP4TinyTcamCtx *CtxPtr, uint8_t *KeyPtr, uint8_t *MaskPtr, uint8_t *ResponsePtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    XilVitisNetP4CamCtx *CamCtxPtr = NULL;
    XilVitisNetP4TinyCamPrvCtx *PrivateCtxPtr = NULL;
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

    if (CtxPtr->PrivateCtxPtr->PrivateVitisNetP4CamCtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_INVALID_CONTEXT;
    }

    PrivateCtxPtr = CtxPtr->PrivateCtxPtr;
    CamCtxPtr = PrivateCtxPtr->PrivateVitisNetP4CamCtxPtr;

    if (CamCtxPtr->Endian == XIL_VITIS_NET_P4_BIG_ENDIAN)
    {
        KeyParamPtr = CamCtxPtr->KeyBufPtr;
        Result = XilVitisNetP4CamByteSwap(CamCtxPtr, KeyPtr, KeyParamPtr, VITISNETP4_CAM_SWAP_KEY);
        if (Result == XIL_VITIS_NET_P4_SUCCESS)
        {
            MaskParamPtr = CamCtxPtr->MaskBufPtr;
            Result = XilVitisNetP4CamByteSwap(CamCtxPtr, MaskPtr, MaskParamPtr, VITISNETP4_CAM_SWAP_KEY_MASK);
            if (Result == XIL_VITIS_NET_P4_SUCCESS)
            {
                ResponseParamPtr = CamCtxPtr->ResponseBufPtr;
                Result = XilVitisNetP4CamByteSwap(CamCtxPtr, ResponsePtr, ResponseParamPtr, VITISNETP4_CAM_SWAP_RESPONSE);
            }
        }
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        Result = TinyCamCommonUpdate(PrivateCtxPtr, KeyParamPtr, MaskParamPtr, ResponseParamPtr);
    }

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4TinyTcamGetByResponse(XilVitisNetP4TinyTcamCtx *CtxPtr,
                                                 uint8_t *ResponsePtr,
                                                 uint8_t *ResponseMaskPtr,
                                                 uint32_t *PositionPtr,
                                                 uint8_t *KeyPtr,
                                                 uint8_t *MaskPtr)
{

    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    XilVitisNetP4CamCtx *CamCtxPtr = NULL;
    XilVitisNetP4TinyCamPrvCtx *PrivateCtxPtr = NULL;
    XilVitisNetP4TinyCamEntry *FoundEntryPtr;
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

    if (CtxPtr->PrivateCtxPtr->PrivateVitisNetP4CamCtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_INVALID_CONTEXT;
    }

    PrivateCtxPtr = CtxPtr->PrivateCtxPtr;
    CamCtxPtr = PrivateCtxPtr->PrivateVitisNetP4CamCtxPtr;

    /* At the end will not find any more keys */
    if (*PositionPtr >= CamCtxPtr->NumEntries)
    {
        return XIL_VITIS_NET_P4_CAM_ERR_KEY_NOT_FOUND;
    }

    if (CamCtxPtr->Endian == XIL_VITIS_NET_P4_BIG_ENDIAN)
    {
        ResponseParamPtr = CamCtxPtr->ResponseBufPtr;
        Result = XilVitisNetP4CamByteSwap(CamCtxPtr, ResponsePtr, ResponseParamPtr, VITISNETP4_CAM_SWAP_RESPONSE);
        if (Result == XIL_VITIS_NET_P4_SUCCESS)
        {
            ResponseMaskParamPtr = CamCtxPtr->ResponseMaskBufPtr;
            Result = XilVitisNetP4CamByteSwap(CamCtxPtr, ResponseMaskPtr, ResponseMaskParamPtr, VITISNETP4_CAM_SWAP_RESPONSE_MASK);
        }
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        Result = TinyCamCommonGetByResponse(PrivateCtxPtr, ResponseParamPtr, ResponseMaskParamPtr, PositionPtr, &FoundEntryPtr);
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        if (CamCtxPtr->Endian == XIL_VITIS_NET_P4_BIG_ENDIAN)
        {
            Result = XilVitisNetP4CamByteSwap(CamCtxPtr, FoundEntryPtr->KeyPtr, KeyPtr, VITISNETP4_CAM_SWAP_KEY);
            if (Result == XIL_VITIS_NET_P4_SUCCESS)
            {
                Result = XilVitisNetP4CamByteSwap(CamCtxPtr, FoundEntryPtr->MaskPtr, MaskPtr, VITISNETP4_CAM_SWAP_KEY_MASK);
            }
        }
        else
        {
            memcpy(KeyPtr, FoundEntryPtr->KeyPtr, CamCtxPtr->KeyBufSizeBytes);
            memcpy(MaskPtr, FoundEntryPtr->MaskPtr, CamCtxPtr->KeyBufSizeBytes);
        }

        *PositionPtr = FoundEntryPtr->EntryId + 1;
    }

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4TinyTcamGetByKey(XilVitisNetP4TinyTcamCtx *CtxPtr,
                                            uint8_t *KeyPtr,
                                            uint8_t *MaskPtr,
                                            uint32_t *PriorityPtr,
                                            uint8_t *ResponsePtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    XilVitisNetP4CamCtx *CamCtxPtr = NULL;
    XilVitisNetP4TinyCamPrvCtx *PrivateCtxPtr = NULL;
    XilVitisNetP4TinyCamEntry *FoundEntryPtr = NULL; /* Ensure search starts at head */
    uint8_t *KeyParamPtr = KeyPtr;
    uint8_t *MaskParamPtr = MaskPtr;

    if ((CtxPtr == NULL) || (KeyPtr == NULL) || (MaskPtr == NULL) || (PriorityPtr == NULL) || (ResponsePtr == NULL))
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    if (CtxPtr->PrivateCtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_INVALID_CONTEXT;
    }

    if (CtxPtr->PrivateCtxPtr->PrivateVitisNetP4CamCtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_INVALID_CONTEXT;
    }

    PrivateCtxPtr = CtxPtr->PrivateCtxPtr;
    CamCtxPtr = PrivateCtxPtr->PrivateVitisNetP4CamCtxPtr;

    if (CamCtxPtr->Endian == XIL_VITIS_NET_P4_BIG_ENDIAN)
    {
        KeyParamPtr = CamCtxPtr->KeyBufPtr;
        Result = XilVitisNetP4CamByteSwap(CamCtxPtr, KeyPtr, KeyParamPtr, VITISNETP4_CAM_SWAP_KEY);
        if (Result == XIL_VITIS_NET_P4_SUCCESS)
        {
            MaskParamPtr = CamCtxPtr->MaskBufPtr;
            Result = XilVitisNetP4CamByteSwap(CamCtxPtr, MaskPtr, MaskParamPtr, VITISNETP4_CAM_SWAP_KEY_MASK);
        }
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        Result = TinyCamCommonGetByKey(PrivateCtxPtr, KeyParamPtr, MaskParamPtr, &FoundEntryPtr);
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        if (CamCtxPtr->Endian == XIL_VITIS_NET_P4_BIG_ENDIAN)
        {
            Result = XilVitisNetP4CamByteSwap(CamCtxPtr, FoundEntryPtr->ResponsePtr, ResponsePtr, VITISNETP4_CAM_SWAP_RESPONSE);
        }
        else
        {
            memcpy(ResponsePtr, FoundEntryPtr->ResponsePtr, CamCtxPtr->ResponseBufSizeBytes);
        }

        *PriorityPtr = FoundEntryPtr->Priority;
    }

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4TinyTcamDelete(XilVitisNetP4TinyTcamCtx *CtxPtr, uint8_t *KeyPtr, uint8_t *MaskPtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    uint8_t *KeyParamPtr = KeyPtr;
    uint8_t *MaskParamPtr = MaskPtr;
    XilVitisNetP4CamCtx *CamCtxPtr = NULL;
    XilVitisNetP4TinyCamPrvCtx *PrivateCtxPtr = NULL;

    if ((CtxPtr == NULL) || (KeyPtr == NULL) || (MaskPtr == NULL))
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    if (CtxPtr->PrivateCtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_INVALID_CONTEXT;
    }

    if (CtxPtr->PrivateCtxPtr->PrivateVitisNetP4CamCtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_INVALID_CONTEXT;
    }

    PrivateCtxPtr = CtxPtr->PrivateCtxPtr;
    CamCtxPtr = PrivateCtxPtr->PrivateVitisNetP4CamCtxPtr;

    if (CamCtxPtr->Endian == XIL_VITIS_NET_P4_BIG_ENDIAN)
    {
        KeyParamPtr = CamCtxPtr->KeyBufPtr;
        Result = XilVitisNetP4CamByteSwap(CamCtxPtr, KeyPtr, KeyParamPtr, VITISNETP4_CAM_SWAP_KEY);
        if (Result == XIL_VITIS_NET_P4_SUCCESS)
        {
            MaskParamPtr = CamCtxPtr->MaskBufPtr;
            Result = XilVitisNetP4CamByteSwap(CamCtxPtr, MaskPtr, MaskParamPtr, VITISNETP4_CAM_SWAP_KEY_MASK);
        }
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        Result = TinyCamTcamDelete(PrivateCtxPtr, KeyParamPtr, MaskParamPtr);
    }

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4TinyTcamReset(XilVitisNetP4TinyTcamCtx *CtxPtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    XilVitisNetP4TinyCamPrvCtx *PrivateCtxPtr;

    if (CtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    if (CtxPtr->PrivateCtxPtr->PrivateVitisNetP4CamCtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_INVALID_CONTEXT;
    }

    PrivateCtxPtr = CtxPtr->PrivateCtxPtr;

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        Result = TinyCamCommonReset(PrivateCtxPtr);
    }

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4TinyTcamGetEccCountersClearOnRead(XilVitisNetP4TinyTcamCtx *CtxPtr,
                                                             uint32_t *CorrectedSingleBitErrorsPtr,
                                                             uint32_t *DetectedDoubleBitErrorsPtr)
{
    XilVitisNetP4TinyCamPrvCtx *PrivateCtxPtr;

    if ((CtxPtr == NULL) || (CorrectedSingleBitErrorsPtr == NULL) || (DetectedDoubleBitErrorsPtr == NULL))
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    if (CtxPtr->PrivateCtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_INVALID_CONTEXT;
    }

    PrivateCtxPtr = CtxPtr->PrivateCtxPtr;

    return TinyCamCommonGetEccCountersClearOnRead(PrivateCtxPtr, CorrectedSingleBitErrorsPtr, DetectedDoubleBitErrorsPtr);
}

XilVitisNetP4ReturnType XilVitisNetP4TinyTcamGetEccAddressesClearOnRead(XilVitisNetP4TinyTcamCtx *CtxPtr,
                                                              uint32_t *FailingAddressSingleBitErrorPtr,
                                                              uint32_t *FailingAddressDoubleBitErrorPtr)
{
    XilVitisNetP4TinyCamPrvCtx *PrivateCtxPtr;

    if ((CtxPtr == NULL) || (FailingAddressSingleBitErrorPtr == NULL) || (FailingAddressDoubleBitErrorPtr == NULL))
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    if (CtxPtr->PrivateCtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_INVALID_CONTEXT;
    }

    PrivateCtxPtr = CtxPtr->PrivateCtxPtr;

    return TinyCamCommonGetEccAddressesClearOnRead(PrivateCtxPtr, FailingAddressSingleBitErrorPtr, FailingAddressDoubleBitErrorPtr);
}

XilVitisNetP4ReturnType XilVitisNetP4TinyTcamRewrite(XilVitisNetP4TinyTcamCtx *CtxPtr)
{
    XilVitisNetP4TinyCamPrvCtx *PrivateCtxPtr;

    if (CtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    if (CtxPtr->PrivateCtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_INVALID_CONTEXT;
    }

    PrivateCtxPtr = CtxPtr->PrivateCtxPtr;

    return TinyCamCommonRewrite(PrivateCtxPtr);
}

/****************************************************************************************************************************************************/
/* SECTION:  Local function definitions for BCAM Emulation*/
/****************************************************************************************************************************************************/
static XilVitisNetP4ReturnType TinyCamBcamDelete(XilVitisNetP4TinyCamPrvCtx *PrivateCtxPtr, uint8_t *KeyPtr, uint8_t *MaskPtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    XilVitisNetP4TinyCamEntry *FoundEntryPtr = NULL; /* Ensure search starts at head */
    bool EntryMatches;

    /* search shadow table */
    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        Result = XilVitisNetP4TinyCamPrvShadowTableFindEntry(PrivateCtxPtr, &(PrivateCtxPtr->ShadowTable), KeyPtr, MaskPtr, &FoundEntryPtr);
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        Result = XilVitisNetP4TinyCamPrvIpTableCheckEntryMatches(PrivateCtxPtr, FoundEntryPtr, &EntryMatches);
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        if (EntryMatches == true)
        {
            Result = XilVitisNetP4TinyCamPrvIpTableDeleteEntry(PrivateCtxPtr, FoundEntryPtr);

            if (Result == XIL_VITIS_NET_P4_SUCCESS)
            {
                Result = XilVitisNetP4TinyCamPrvShadowTableEmptyEntry(PrivateCtxPtr, FoundEntryPtr);
            }
        }
        else
        {
            /* Shadow Table and Cam IP Table are out of sync should not happen */
            Result = XIL_VITIS_NET_P4_GENERAL_ERR_INTERNAL_ASSERTION;
        }
    }

    return Result;
}

/****************************************************************************************************************************************************/
/* SECTION:  Local function definitions for TCAM Emulation*/
/****************************************************************************************************************************************************/
static XilVitisNetP4ReturnType TinyTcamInsert(XilVitisNetP4TinyCamPrvCtx *PrivateCtxPtr,
                                         uint8_t *KeyPtr,
                                         uint8_t *MaskPtr,
                                         uint32_t Priority,
                                         uint8_t *ResponsePtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    bool EntryMatches;
    XilVitisNetP4TinyCamEntry *FoundEntryPtr = NULL;
    XilVitisNetP4TinyCamEntry *InsertEntryPtr = NULL;
    XilVitisNetP4TinyCamEntry *PivotEntryPtr = NULL;
    XilVitisNetP4TinyCamEntry *CurrentEntryPtr = NULL;
    XilVitisNetP4TinyCamShadowTable *ShadowTablePtr = &(PrivateCtxPtr->ShadowTable);

    /* search shadow table */
    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        Result = XilVitisNetP4TinyCamPrvShadowTableFindEntry(PrivateCtxPtr, ShadowTablePtr, KeyPtr, MaskPtr, &FoundEntryPtr);
    }

    /* Should not find the entry in the table but if do check CAM IP Table that it is present*/
    if (Result != XIL_VITIS_NET_P4_CAM_ERR_KEY_NOT_FOUND)
    {
        Result = XilVitisNetP4TinyCamPrvIpTableCheckEntryMatches(PrivateCtxPtr, FoundEntryPtr, &EntryMatches);
        if (Result == XIL_VITIS_NET_P4_SUCCESS)
        {
            if (EntryMatches == false)
            {
                /*this means that the Shadow Table and CAM IP Table are out of sync */
                Result = XIL_VITIS_NET_P4_GENERAL_ERR_INTERNAL_ASSERTION;
            }
            else
            {
                Result = XIL_VITIS_NET_P4_CAM_ERR_DUPLICATE_FOUND;
            }
        }
    }
    else
    {
        /* Find if there is an Entry in the Shadow table that can be used to insert new entry*/
        Result = XilVitisNetP4TinyCamPrvShadowTableNextEmptyEntry(ShadowTablePtr, &InsertEntryPtr);
        if (Result == XIL_VITIS_NET_P4_SUCCESS)
        {
            /* Find the position that wait to reuse */
            Result = XilVitisNetP4TinyCamPrvShadowTableFindPivotEntryByPriority(ShadowTablePtr, Priority, &PivotEntryPtr);
        }

        if (Result == XIL_VITIS_NET_P4_SUCCESS)
        {
            Result = XilVitisNetP4TinyCamPrvShadowTableFillEntry(PrivateCtxPtr, InsertEntryPtr, KeyPtr, MaskPtr, Priority, ResponsePtr);
        }

        if (Result == XIL_VITIS_NET_P4_SUCCESS)
        {
            Result = XilVitisNetP4TinyCamPrvShadowTableInsertEntryAfter(ShadowTablePtr, PivotEntryPtr, InsertEntryPtr);
        }

        /* Find last used */
        if (Result == XIL_VITIS_NET_P4_SUCCESS)
        {

            if (PivotEntryPtr == NULL)
            {
                CurrentEntryPtr = ShadowTablePtr->EntryListHeadPtr;
            }
            else
            {
                CurrentEntryPtr = PivotEntryPtr;
            }

            while ((CurrentEntryPtr->NextEntryPtr != NULL) && (CurrentEntryPtr->NextEntryPtr->InUse == true))
            {
                CurrentEntryPtr = CurrentEntryPtr->NextEntryPtr;
            }
        }

        /* Update the CAM IP Table  */
        while ((CurrentEntryPtr != PivotEntryPtr) && (Result == XIL_VITIS_NET_P4_SUCCESS))
        {   
            if (Result == XIL_VITIS_NET_P4_SUCCESS)
            {
                Result = XilVitisNetP4TinyCamPrvIpTableRewriteEntry(PrivateCtxPtr, CurrentEntryPtr);    
                CurrentEntryPtr = CurrentEntryPtr->PreviousEntryPtr;   
            }
        }
    }

    return Result;
}

static XilVitisNetP4ReturnType TinyCamTcamDelete(XilVitisNetP4TinyCamPrvCtx *PrivateCtxPtr, uint8_t *KeyPtr, uint8_t *MaskPtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    XilVitisNetP4TinyCamEntry *FoundEntryPtr = NULL;   /* Ensure search starts at head */
    XilVitisNetP4TinyCamEntry *CurrentEntryPtr = NULL; /* Ensure search starts at head */
    bool EntryMatches;

    /* search shadow table */
    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        Result = XilVitisNetP4TinyCamPrvShadowTableFindEntry(PrivateCtxPtr, &(PrivateCtxPtr->ShadowTable), KeyPtr, MaskPtr, &FoundEntryPtr);
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        Result = XilVitisNetP4TinyCamPrvIpTableCheckEntryMatches(PrivateCtxPtr, FoundEntryPtr, &EntryMatches);
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        if (EntryMatches == true)
        {
            /* Delete the found entry for each entry after decrement the entry position in the Cam IP Table */
            CurrentEntryPtr = FoundEntryPtr;
            do
            {
                if (Result == XIL_VITIS_NET_P4_SUCCESS)
                {
                    Result = XilVitisNetP4TinyCamPrvIpTableDeleteEntry(PrivateCtxPtr, CurrentEntryPtr);
                }

                if (Result == XIL_VITIS_NET_P4_SUCCESS)
                {
                    /* Current is  the tail then we do not need to write the next (there is no next) */
                    if (CurrentEntryPtr->NextEntryPtr == NULL)
                    {
                        break;
                    }

                    /* If the next is not empty, then move  next entry to current position (adjust EntryId) in the CAM IP */
                    if (CurrentEntryPtr->NextEntryPtr->InUse == true)
                    {
                        CurrentEntryPtr->NextEntryPtr->EntryId--;
                        Result = XilVitisNetP4TinyCamPrvIpTableRewriteEntry(PrivateCtxPtr, CurrentEntryPtr->NextEntryPtr);
                        CurrentEntryPtr->NextEntryPtr->EntryId++;
                    }
                }

                CurrentEntryPtr = CurrentEntryPtr->NextEntryPtr;

            } while ((CurrentEntryPtr->InUse == true) && (Result == XIL_VITIS_NET_P4_SUCCESS));

            /* Remove from Shadow Table, and ensure aligned with CAM IP */
            if (Result == XIL_VITIS_NET_P4_SUCCESS)
            {
                Result = XilVitisNetP4TinyCamPrvShadowTableMoveEmptyEntryToTail(&(PrivateCtxPtr->ShadowTable), FoundEntryPtr);
            }
        }
        else
        {
            /* Shadow Table and Cam IP Table are out of sync should not happen */
            Result = XIL_VITIS_NET_P4_GENERAL_ERR_INTERNAL_ASSERTION;
        }
    }

    return Result;
}

/****************************************************************************************************************************************************/
/* SECTION: local functions definitions common to BCAM and TCAM Emulation */
/****************************************************************************************************************************************************/

static XilVitisNetP4ReturnType TinyCamCommonInit(XilVitisNetP4TinyCamPrvCtx **PrivateCtxPtrPtr,
                                            const XilVitisNetP4EnvIf *EnvIfPtr,
                                            XilVitisNetP4CamConfig *ConfigPtr,
                                            XilVitisNetP4TinyCamEmulationMode EmulationMode,
                                            bool CalcHeapSize,
                                            ...)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    XilVitisNetP4CamCtx *PrivateCamCtxPtr = NULL;
    uint32_t KeySizeBits;
    uint32_t ResponseSizeBits;
    uint32_t ResponsePaddedBytes;
    uint64_t* HeapSizePtr = NULL;
    uint64_t TinyCamCtxHeapSize = 0;
    uint64_t VitisNetP4CamCtxHeapSize = 0;
    uint64_t TinyCamMaskHeapSize = 0;
    uint64_t TinyCamPrivateCtxBuffersHeapSize = 0;
    uint64_t TinyCamShadowHeapSize = 0;

    Result = XilVitisNetP4ValidateEnvIf(EnvIfPtr);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }

    if ((ConfigPtr->Endian != XIL_VITIS_NET_P4_LITTLE_ENDIAN) && (ConfigPtr->Endian != XIL_VITIS_NET_P4_BIG_ENDIAN))
    {
        Result = XIL_VITIS_NET_P4_CAM_ERR_INVALID_ENDIAN;
        return Result;
    }

    Result = XilVitisNetP4CamGetKeyLengthInBits(ConfigPtr->FormatStringPtr, &KeySizeBits);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }

    ResponseSizeBits = ConfigPtr->ResponseSizeBits;

    if ((KeySizeBits == 0) || (KeySizeBits > XIL_VITIS_NET_P4_TINYCAM_MAX_KEY_SIZE_BITS))
    {
        Result = XIL_VITIS_NET_P4_CAM_ERR_WRONG_KEY_WIDTH;
        return Result;
    }

    if ((ResponseSizeBits == 0) || (ResponseSizeBits > XIL_VITIS_NET_P4_TINYCAM_MAX_RESP_SIZE_BITS))
    {
        Result = XIL_VITIS_NET_P4_CAM_ERR_WRONG_RESPONSE_WIDTH;
        return Result;
    }

    *PrivateCtxPtrPtr = calloc(1, sizeof(XilVitisNetP4TinyCamPrvCtx));
    if (*PrivateCtxPtrPtr == NULL)
    {
        return XIL_VITIS_NET_P4_CAM_ERR_MALLOC_FAILED;
    }
    else
    {
        if (CalcHeapSize == true)
        {
            TinyCamCtxHeapSize = sizeof(XilVitisNetP4TinyCamPrvCtx);
        }
    }

    if (CalcHeapSize == true)
    {
        Result = XilVitisNetP4CamCtxCreate(&PrivateCamCtxPtr, EnvIfPtr, ConfigPtr, true, &VitisNetP4CamCtxHeapSize);
    }
    else
    {
        Result = XilVitisNetP4CamCtxCreate(&PrivateCamCtxPtr, EnvIfPtr, ConfigPtr, false);
    }

    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        free(*PrivateCtxPtrPtr);
        return Result;
    }

    /* Allocate Dummy Mask Ptr, is always needed not just for endian swap */
    if (PrivateCamCtxPtr->Endian == XIL_VITIS_NET_P4_LITTLE_ENDIAN)
    {
        PrivateCamCtxPtr->MaskBufPtr = calloc(PrivateCamCtxPtr->KeyBufSizeBytes, sizeof(uint8_t));
        if (PrivateCamCtxPtr->MaskBufPtr == NULL)
        {
            goto fail_mask_buf_alloc;
        }
        else
        {
            if (CalcHeapSize == true)
            {
                TinyCamMaskHeapSize = (uint64_t)(PrivateCamCtxPtr->KeyBufSizeBytes * sizeof(uint8_t));
            }
        }
    }

    /* Used to reduce the calcualtions on table operations */
    (*PrivateCtxPtrPtr)->TableInfo.EntrySizeBytes = (2 * PrivateCamCtxPtr->KeyBufSizeBytes) + PrivateCamCtxPtr->ResponseBufSizeBytes;
    (*PrivateCtxPtrPtr)->TableInfo.EntrySizeWord32 = (*PrivateCtxPtrPtr)->TableInfo.EntrySizeBytes / XIL_VITIS_NET_P4_BYTES_PER_REG;
    (*PrivateCtxPtrPtr)->TableInfo.NumPaddingBytes = 0;
    if (((*PrivateCtxPtrPtr)->TableInfo.EntrySizeBytes % XIL_VITIS_NET_P4_BYTES_PER_REG) != 0)
    {
        (*PrivateCtxPtrPtr)->TableInfo.EntrySizeWord32++;
        (*PrivateCtxPtrPtr)->TableInfo.NumPaddingBytes
            = XIL_VITIS_NET_P4_BYTES_PER_REG - ((*PrivateCtxPtrPtr)->TableInfo.EntrySizeBytes % XIL_VITIS_NET_P4_BYTES_PER_REG);
    }

    (*PrivateCtxPtrPtr)->TableInfo.TotalSizeBits
        = (ConfigPtr->NumEntries) * ((*PrivateCtxPtrPtr)->TableInfo.EntrySizeWord32) * (XIL_VITIS_NET_P4_BITS_PER_REG);

    if ((*PrivateCtxPtrPtr)->TableInfo.TotalSizeBits > XIL_VITIS_NET_P4_TINYCAM_TABLE_SIZE_BITS)
    {
        Result = XIL_VITIS_NET_P4_CAM_ERR_INVALID_NUM_ENTRIES;
        goto fail_table_size_check;
    }

    (*PrivateCtxPtrPtr)->TableInfo.NumKeyRegisters = PrivateCamCtxPtr->KeyBufSizeBytes / XIL_VITIS_NET_P4_BYTES_PER_REG;
    if (PrivateCamCtxPtr->KeyBufSizeBytes % XIL_VITIS_NET_P4_BYTES_PER_REG)
    {
        (*PrivateCtxPtrPtr)->TableInfo.NumKeyRegisters++;
    }

    /* The Start Byte in a register may not align on a 32bit boundary*/
    ResponsePaddedBytes = XIL_VITIS_NET_P4_BYTES_PER_REG - ((PrivateCamCtxPtr->KeyBufSizeBytes * 2) % XIL_VITIS_NET_P4_BYTES_PER_REG);
    (*PrivateCtxPtrPtr)->TableInfo.NumResponseRegisters = (PrivateCamCtxPtr->ResponseBufSizeBytes + ResponsePaddedBytes) / XIL_VITIS_NET_P4_BYTES_PER_REG;
    if ((PrivateCamCtxPtr->ResponseBufSizeBytes + ResponsePaddedBytes) % XIL_VITIS_NET_P4_BYTES_PER_REG)
    {
        (*PrivateCtxPtrPtr)->TableInfo.NumResponseRegisters++;
    }

    if (CalcHeapSize == true)
    {
        Result = TinyCamMallocPrivateCtxBuffers(*PrivateCtxPtrPtr, true, &TinyCamPrivateCtxBuffersHeapSize);
    }
    else
    {
        Result = TinyCamMallocPrivateCtxBuffers(*PrivateCtxPtrPtr, false);
    }

    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        goto fail_alloc_priv_buffs;
    }

    (*PrivateCtxPtrPtr)->PrivateVitisNetP4CamCtxPtr = PrivateCamCtxPtr;

    if (CalcHeapSize != true)
    {
        Result = TinyCamHwGetEmulationMode(*PrivateCtxPtrPtr, &((*PrivateCtxPtrPtr)->EmulationMode));
        if (Result != XIL_VITIS_NET_P4_SUCCESS)
        {
            goto fail_get_emulation_mode;
        }

        if ((*PrivateCtxPtrPtr)->EmulationMode != EmulationMode)
        {
            Result = XIL_VITIS_NET_P4_TINY_CAM_ERR_INVALID_MODE;
            goto fail_check_emulation_mode;
        }
    }

    if (CalcHeapSize == true)
    {
        XilVitisNetP4TinyCamCalcHeapShadowTable(*PrivateCtxPtrPtr, &TinyCamShadowHeapSize);
    }
    else
    {
        Result = XilVitisNetP4TinyCamPrvShadowTableCreate(*PrivateCtxPtrPtr, &((*PrivateCtxPtrPtr)->ShadowTable));
        if (Result != XIL_VITIS_NET_P4_SUCCESS)
        {
            goto fail_create_shadow_table;
        }
    }

    if (CalcHeapSize == true)
    {
        /* Retrieve the size of heap allocation for the cam arguments*/
        va_list   ArgPtr;
        
        va_start(ArgPtr, CalcHeapSize);
        HeapSizePtr = va_arg(ArgPtr, uint64_t*);
        va_end(ArgPtr);
        
        if (HeapSizePtr == NULL)
        {
            Result = XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
        }
        else
        {
            *HeapSizePtr = TinyCamCtxHeapSize + VitisNetP4CamCtxHeapSize + TinyCamMaskHeapSize + TinyCamPrivateCtxBuffersHeapSize + TinyCamShadowHeapSize;
        }

        goto release_all_memory;
    }


    return Result;

release_all_memory:
fail_create_shadow_table:
fail_check_emulation_mode:
fail_get_emulation_mode:
    TinyCamFreePrivateCtxBuffers(*PrivateCtxPtrPtr);

fail_mask_buf_alloc:
fail_table_size_check:
fail_alloc_priv_buffs:
    XilVitisNetP4CamCtxDestroy(PrivateCamCtxPtr);
    free(*PrivateCtxPtrPtr);

    return Result;
}

static XilVitisNetP4ReturnType TinyCamCommonExit(XilVitisNetP4TinyCamPrvCtx *PrivateCtxPtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;

    Result = XilVitisNetP4CamValidateCtx(PrivateCtxPtr->PrivateVitisNetP4CamCtxPtr);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }

    Result = XilVitisNetP4TinyCamPrvShadowTableDestroy(&(PrivateCtxPtr->ShadowTable));
    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        Result = TinyCamFreePrivateCtxBuffers(PrivateCtxPtr);
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        Result = XilVitisNetP4CamCtxDestroy(PrivateCtxPtr->PrivateVitisNetP4CamCtxPtr);
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        PrivateCtxPtr->PrivateVitisNetP4CamCtxPtr = NULL;
        free(PrivateCtxPtr);
    }

    return Result;
}

static XilVitisNetP4ReturnType TinyCamCommonUpdate(XilVitisNetP4TinyCamPrvCtx *PrivateCtxPtr, uint8_t *KeyPtr, uint8_t *MaskPtr, uint8_t *ResponsePtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    XilVitisNetP4TinyCamEntry *EntryPtr = NULL; /* To ensure search starts from head */
    bool EntryMatches;

    /* search shadow table */
    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        Result = XilVitisNetP4TinyCamPrvShadowTableFindEntry(PrivateCtxPtr, &(PrivateCtxPtr->ShadowTable), KeyPtr, MaskPtr, &EntryPtr);
    }

    /* Should not find the entry in the table */
    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        Result = XilVitisNetP4TinyCamPrvIpTableCheckEntryMatches(PrivateCtxPtr, EntryPtr, &EntryMatches);
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        if (EntryMatches == true)
        {
            Result = XilVitisNetP4TinyCamPrvShadowTableUpdateEntry(PrivateCtxPtr, EntryPtr, ResponsePtr);
        }
        else
        {
            Result = XIL_VITIS_NET_P4_GENERAL_ERR_INTERNAL_ASSERTION;
        }

        if (Result == XIL_VITIS_NET_P4_SUCCESS)
        {
            Result = XilVitisNetP4TinyCamPrvIpTableRewriteEntry(PrivateCtxPtr, EntryPtr);
        }
    }

    return Result;
}

static XilVitisNetP4ReturnType TinyCamCommonGetByResponse(XilVitisNetP4TinyCamPrvCtx *PrivateCtxPtr,
                                                     uint8_t *ResponsePtr,
                                                     uint8_t *ResponseMaskPtr,
                                                     uint32_t *PositionPtr,
                                                     XilVitisNetP4TinyCamEntry **FoundEntryPtrPtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    bool EntryMatches;
    uint32_t Position = *PositionPtr;

    if (Position == 0)
    {
        *FoundEntryPtrPtr = NULL; /* Ensure search starts at head */
    }
    else
    {
        Result = XilVitisNetP4TinyCamPrvShadowTableGetEntryById(&(PrivateCtxPtr->ShadowTable), *PositionPtr, FoundEntryPtrPtr);
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        Result = XilVitisNetP4TinyCamPrvIpShadowTableFindEntryByResponse(PrivateCtxPtr,
                                                                    &(PrivateCtxPtr->ShadowTable),
                                                                    ResponsePtr,
                                                                    ResponseMaskPtr,
                                                                    FoundEntryPtrPtr);
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        Result = XilVitisNetP4TinyCamPrvIpTableCheckEntryMatches(PrivateCtxPtr, *FoundEntryPtrPtr, &EntryMatches);
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        if (EntryMatches == false)
        {
            Result = XIL_VITIS_NET_P4_GENERAL_ERR_INTERNAL_ASSERTION;
        }
    }

    return Result;
}

static XilVitisNetP4ReturnType TinyCamCommonGetByKey(XilVitisNetP4TinyCamPrvCtx *PrivateCtxPtr,
                                                uint8_t *KeyPtr,
                                                uint8_t *MaskPtr,
                                                XilVitisNetP4TinyCamEntry **FoundEntryPtrPtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    bool EntryMatches;

    /* search shadow table */
    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        Result = XilVitisNetP4TinyCamPrvShadowTableFindEntry(PrivateCtxPtr, &(PrivateCtxPtr->ShadowTable), KeyPtr, MaskPtr, FoundEntryPtrPtr);
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        Result = XilVitisNetP4TinyCamPrvIpTableCheckEntryMatches(PrivateCtxPtr, *FoundEntryPtrPtr, &EntryMatches);
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        if (EntryMatches == false)
        {
            Result = XIL_VITIS_NET_P4_GENERAL_ERR_INTERNAL_ASSERTION;
        }
    }

    return Result;
}

static XilVitisNetP4ReturnType TinyCamCommonReset(XilVitisNetP4TinyCamPrvCtx *PrivateCtxPtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    uint32_t EntryId;
    XilVitisNetP4TinyCamEntry *EntryPtr;

    /* Loop through all tables and set them equal to zero */
    for (EntryId = 0; EntryId < PrivateCtxPtr->ShadowTable.NumEntriesMax; EntryId++)
    {

        if (Result == XIL_VITIS_NET_P4_SUCCESS)
        {
            Result = XilVitisNetP4TinyCamPrvShadowTableGetEntryById(&(PrivateCtxPtr->ShadowTable), EntryId, &EntryPtr);
        }

        if (Result == XIL_VITIS_NET_P4_SUCCESS)
        {
            Result = XilVitisNetP4TinyCamPrvShadowTableEmptyEntry(PrivateCtxPtr, EntryPtr);
        }

        if (Result == XIL_VITIS_NET_P4_SUCCESS)
        {
            Result = XilVitisNetP4TinyCamPrvIpTableDeleteEntry(PrivateCtxPtr, EntryPtr);
        }

        if (Result != XIL_VITIS_NET_P4_SUCCESS)
        {
            break;
        }
    }

    return Result;
}

static XilVitisNetP4ReturnType TinyCamCommonGetEccCountersClearOnRead(XilVitisNetP4TinyCamPrvCtx *PrivateCtxPtr,
                                                                 uint32_t *CorrectedSingleBitErrorsPtr,
                                                                 uint32_t *DetectedDoubleBitErrorsPtr)
{
    XIL_VITIS_NET_P4_UNUSED_ARG(PrivateCtxPtr);
    XIL_VITIS_NET_P4_UNUSED_ARG(CorrectedSingleBitErrorsPtr);
    XIL_VITIS_NET_P4_UNUSED_ARG(DetectedDoubleBitErrorsPtr);
    return XIL_VITIS_NET_P4_TABLE_ERR_FUNCTION_NOT_SUPPORTED;
}

static XilVitisNetP4ReturnType TinyCamCommonGetEccAddressesClearOnRead(XilVitisNetP4TinyCamPrvCtx *PrivateCtxPtr,
                                                                  uint32_t *FailingAddressSingleBitErrorPtr,
                                                                  uint32_t *FailingAddressDoubleBitErrorPtr)
{
    XIL_VITIS_NET_P4_UNUSED_ARG(PrivateCtxPtr);
    XIL_VITIS_NET_P4_UNUSED_ARG(FailingAddressSingleBitErrorPtr);
    XIL_VITIS_NET_P4_UNUSED_ARG(FailingAddressDoubleBitErrorPtr);
    return XIL_VITIS_NET_P4_TABLE_ERR_FUNCTION_NOT_SUPPORTED;
}

static XilVitisNetP4ReturnType TinyCamCommonRewrite(XilVitisNetP4TinyCamPrvCtx *PrivateCtxPtr)
{
    XIL_VITIS_NET_P4_UNUSED_ARG(PrivateCtxPtr);
    return XIL_VITIS_NET_P4_TABLE_ERR_FUNCTION_NOT_SUPPORTED;
}

static XilVitisNetP4ReturnType TinyCamMallocPrivateCtxBuffers(XilVitisNetP4TinyCamPrvCtx *PrivateCtxPtr, bool CalcHeapSize, ...)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    ;

    /* calloc the true size of a table entry this will include the zero padding to ensure 32 bit boundary*/
    PrivateCtxPtr->ReadBufTableEntryPtr = calloc(PrivateCtxPtr->TableInfo.EntrySizeWord32, sizeof(uint32_t));
    PrivateCtxPtr->WriteBufTableEntryPtr = calloc(PrivateCtxPtr->TableInfo.EntrySizeWord32, sizeof(uint32_t));

    if ((PrivateCtxPtr->ReadBufTableEntryPtr == NULL) || (PrivateCtxPtr->WriteBufTableEntryPtr == NULL))
    {
        goto fail_calloc;
    }
    else
    {
        if (CalcHeapSize == true)
        {
            /* Retrieve the size of heap allocation for the cam arguments*/
            va_list   ArgPtr;
            va_start(ArgPtr, CalcHeapSize);
            uint64_t* HeapSizePtr;

            HeapSizePtr = va_arg(ArgPtr, uint64_t*);
            va_end(ArgPtr);

            if (HeapSizePtr == NULL)
            {
                Result = XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
            }
            else
            {
                *HeapSizePtr = 2 * (uint64_t) (PrivateCtxPtr->TableInfo.EntrySizeWord32) * sizeof(uint32_t);
            }
        }
    }

    return Result;

fail_calloc:
    Result = XIL_VITIS_NET_P4_CAM_ERR_MALLOC_FAILED;
    return Result;
}

static XilVitisNetP4ReturnType TinyCamFreePrivateCtxBuffers(XilVitisNetP4TinyCamPrvCtx *PrivateCtxPtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;

    free(PrivateCtxPtr->ReadBufTableEntryPtr);
    PrivateCtxPtr->ReadBufTableEntryPtr = NULL;

    free(PrivateCtxPtr->WriteBufTableEntryPtr);
    PrivateCtxPtr->WriteBufTableEntryPtr = NULL;

    return Result;
}

/****************************************************************************************************************************************************/
/* SECTION: Function definitions for Shadow Table*/
/****************************************************************************************************************************************************/

XilVitisNetP4ReturnType XilVitisNetP4TinyCamPrvShadowTableCreate(XilVitisNetP4TinyCamPrvCtx *TinyCamPrvCtxPtr, XilVitisNetP4TinyCamShadowTable *ShadowTablePtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;

    uint32_t EntryIndex = 0;
    uint32_t PaddedEntrySizeBytes;
    uint32_t ResponseSizeBytes = TinyCamPrvCtxPtr->PrivateVitisNetP4CamCtxPtr->ResponseBufSizeBytes;
    uint32_t KeySizeBytes = TinyCamPrvCtxPtr->PrivateVitisNetP4CamCtxPtr->KeyBufSizeBytes;
    uint32_t NumEntries = TinyCamPrvCtxPtr->PrivateVitisNetP4CamCtxPtr->NumEntries;
    XilVitisNetP4TinyCamEntry *EntryListHeadPtr;
    uint8_t *EntryArrayMemPtr;

    PaddedEntrySizeBytes = ((2 * KeySizeBytes) + ResponseSizeBytes) * 4;
    PaddedEntrySizeBytes += PaddedEntrySizeBytes % XIL_VITIS_NET_P4_BYTES_PER_REG;

    EntryListHeadPtr = calloc(NumEntries, sizeof(XilVitisNetP4TinyCamEntry));
    if (EntryListHeadPtr == NULL)
    {
        Result = XIL_VITIS_NET_P4_CAM_ERR_MALLOC_FAILED;
        goto list_calloc_failed;
    }

    EntryArrayMemPtr = calloc(NumEntries, PaddedEntrySizeBytes);
    if (EntryArrayMemPtr == NULL)
    {
        Result = XIL_VITIS_NET_P4_CAM_ERR_MALLOC_FAILED;
        goto array_calloc_failed;
    }

    /* Link the list and set initialise entries to be empty, but set the EntryId */
    for (EntryIndex = 0; EntryIndex < NumEntries; EntryIndex++)
    {
        /* For the HEAD */
        if (EntryIndex == 0)
        {
            EntryListHeadPtr[EntryIndex].PreviousEntryPtr = NULL;
            EntryListHeadPtr[EntryIndex].NextEntryPtr = &(EntryListHeadPtr[EntryIndex + 1]);
        }
        else if (EntryIndex == (NumEntries - 1)) /* the list Tail */
        {
            EntryListHeadPtr[EntryIndex].PreviousEntryPtr = &(EntryListHeadPtr[EntryIndex - 1]);
            EntryListHeadPtr[EntryIndex].NextEntryPtr = NULL;
        }
        else /* entries that are linked */
        {
            EntryListHeadPtr[EntryIndex].PreviousEntryPtr = &(EntryListHeadPtr[EntryIndex - 1]);
            EntryListHeadPtr[EntryIndex].NextEntryPtr = &(EntryListHeadPtr[EntryIndex + 1]);
        }

        EntryListHeadPtr[EntryIndex].EntryId = EntryIndex;
        EntryListHeadPtr[EntryIndex].KeyPtr = EntryArrayMemPtr + (EntryIndex * PaddedEntrySizeBytes);
        EntryListHeadPtr[EntryIndex].MaskPtr = EntryListHeadPtr[EntryIndex].KeyPtr + KeySizeBytes;
        EntryListHeadPtr[EntryIndex].ResponsePtr = EntryListHeadPtr[EntryIndex].MaskPtr + KeySizeBytes;
    }

    /* Initialize the ShadowTablePtr values */
    ShadowTablePtr->EntryListHeadPtr = EntryListHeadPtr;
    ShadowTablePtr->EntryListMemPtr = (uint8_t *)EntryListHeadPtr;
    ShadowTablePtr->EntryArrayMemPtr = EntryArrayMemPtr;
    ShadowTablePtr->NumEntriesInUse = 0;
    ShadowTablePtr->NumEntriesMax = NumEntries;

    return Result;

array_calloc_failed:
    free(EntryListHeadPtr);
list_calloc_failed:
    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4TinyCamPrvShadowTableDestroy(XilVitisNetP4TinyCamShadowTable *ShadowTablePtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;

    ShadowTablePtr->NumEntriesMax = 0;
    ShadowTablePtr->NumEntriesInUse = 0;
    free(ShadowTablePtr->EntryArrayMemPtr);
    free(ShadowTablePtr->EntryListMemPtr);

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4TinyCamPrvShadowTableNextEmptyEntry(XilVitisNetP4TinyCamShadowTable *ShadowTablePtr, XilVitisNetP4TinyCamEntry **EmptyEntryPtrPtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    XilVitisNetP4TinyCamEntry *CurrentEntryPtr = ShadowTablePtr->EntryListHeadPtr;
    *EmptyEntryPtrPtr = NULL; /* This is the not found result */

    /* Search until found or end of list */
    do
    {
        if (CurrentEntryPtr->InUse == false)
        {
            *EmptyEntryPtrPtr = CurrentEntryPtr;
        }

        CurrentEntryPtr = CurrentEntryPtr->NextEntryPtr;

    } while ((*EmptyEntryPtrPtr == NULL) && (CurrentEntryPtr != NULL));

    /* Reached the end and all used */
    if (*EmptyEntryPtrPtr == NULL)
    {
        Result = XIL_VITIS_NET_P4_CAM_ERR_FULL;
    }

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4TinyCamPrvShadowTableGetEntryById(XilVitisNetP4TinyCamShadowTable *ShadowTablePtr,
                                                             uint32_t EntryId,
                                                             XilVitisNetP4TinyCamEntry **EmptyEntryPtrPtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    XilVitisNetP4TinyCamEntry *CurrentEntryPtr = ShadowTablePtr->EntryListHeadPtr;
    *EmptyEntryPtrPtr = NULL; /* This is the not found result */

    /* should not happen as should be check at user interface */
    if (EntryId > ShadowTablePtr->NumEntriesMax)
    {
        Result = XIL_VITIS_NET_P4_GENERAL_ERR_INTERNAL_ASSERTION;
        return Result;
    }
    /* Search until found or end of list */
    do
    {
        if (CurrentEntryPtr->EntryId == EntryId)
        {
            *EmptyEntryPtrPtr = CurrentEntryPtr;
        }

        CurrentEntryPtr = CurrentEntryPtr->NextEntryPtr;

    } while ((*EmptyEntryPtrPtr == NULL) && (CurrentEntryPtr != NULL));

    /* Reached the end and all used */
    if (CurrentEntryPtr == NULL)
    {
        Result = XIL_VITIS_NET_P4_GENERAL_ERR_INTERNAL_ASSERTION;
    }

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4TinyCamPrvShadowTableInsertEntryAfter(XilVitisNetP4TinyCamShadowTable *ShadowTablePtr,
                                                                 XilVitisNetP4TinyCamEntry *CurrentEntryPtr,
                                                                 XilVitisNetP4TinyCamEntry *InsertEntryPtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    XilVitisNetP4TinyCamEntry *EntryListHeadPtr = ShadowTablePtr->EntryListHeadPtr;
    XilVitisNetP4TinyCamEntry *EntryIdReassignPtr;
    uint32_t EntryIndex;

    XilVitisNetP4TinyCamEntry *PreviousEntryPtr = InsertEntryPtr->PreviousEntryPtr;
    XilVitisNetP4TinyCamEntry *NextEntryPtr = InsertEntryPtr->NextEntryPtr;  

    /* Remove Insert Entry from List */
    if (PreviousEntryPtr != NULL) /* Update if not the HEAD */
    {
        PreviousEntryPtr->NextEntryPtr = NextEntryPtr;

        /* Do not update the Next if inserting the TAIL */
        if (NextEntryPtr != NULL)
        {
            NextEntryPtr->PreviousEntryPtr = PreviousEntryPtr;
        }
    }


    /* Inserting at the Head */
    if (CurrentEntryPtr == NULL)
    {

        if (InsertEntryPtr != EntryListHeadPtr)
        {
            InsertEntryPtr->PreviousEntryPtr = NULL;
            InsertEntryPtr->NextEntryPtr = EntryListHeadPtr;
            EntryListHeadPtr->PreviousEntryPtr = InsertEntryPtr;
            /* Update the Head */
            ShadowTablePtr->EntryListHeadPtr = InsertEntryPtr;
            EntryListHeadPtr = InsertEntryPtr;
        }
    }
    else
    {
        NextEntryPtr = CurrentEntryPtr->NextEntryPtr;
        
        /* Not at the tail */
        if (NextEntryPtr != NULL)
        {
            NextEntryPtr->PreviousEntryPtr = InsertEntryPtr;
        }
        CurrentEntryPtr->NextEntryPtr = InsertEntryPtr;


        /* Place the Insert into the List */
        InsertEntryPtr->NextEntryPtr = NextEntryPtr;
        InsertEntryPtr->PreviousEntryPtr = CurrentEntryPtr;        
    }

    /* Reassign EntryId to aid keeping Shadow Table in sync with the CAM IP Table */
    EntryIdReassignPtr = EntryListHeadPtr;
    EntryIndex = 0;
    do
    {
        EntryIdReassignPtr->EntryId = EntryIndex;
        EntryIdReassignPtr = EntryIdReassignPtr->NextEntryPtr;
        EntryIndex++;

    } while (EntryIdReassignPtr != NULL);

    ShadowTablePtr->NumEntriesInUse++;

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4TinyCamPrvShadowTableMoveEmptyEntryToTail(XilVitisNetP4TinyCamShadowTable *ShadowTablePtr, XilVitisNetP4TinyCamEntry *EntryPtr)
{
    XilVitisNetP4TinyCamEntry *PreviousEntryPtr;
    XilVitisNetP4TinyCamEntry *NextEntryPtr;
    XilVitisNetP4TinyCamEntry *CurrentEntryPtr;
    bool ReachedTail;

    if (ShadowTablePtr->NumEntriesInUse == 0)
    {
        /* Nothing to Delete */
        return XIL_VITIS_NET_P4_GENERAL_ERR_INTERNAL_ASSERTION;
    }

    /* disconnect the current and link previous and next */
    PreviousEntryPtr = EntryPtr->PreviousEntryPtr;
    NextEntryPtr = EntryPtr->NextEntryPtr;

    if (PreviousEntryPtr == NULL) /* EntryPtr is  the HEAD */
    {
        NextEntryPtr->PreviousEntryPtr = NULL;
        ShadowTablePtr->EntryListHeadPtr = NextEntryPtr;
    }
    else if (NextEntryPtr == NULL) /* EntryPtr is the TAIL */
    {
        /* Delete the Entry no need to move */
        EntryPtr->InUse = false;
        EntryPtr->Priority = 0;
        ShadowTablePtr->NumEntriesInUse--;
        return XIL_VITIS_NET_P4_SUCCESS;
    }
    else
    {
        PreviousEntryPtr->NextEntryPtr = NextEntryPtr;
        NextEntryPtr->PreviousEntryPtr = PreviousEntryPtr;
    }

    /* Starting at the Next Entry, decrement the EntryId ( would have moved one postion in CAM IP Table) */
    CurrentEntryPtr = NextEntryPtr;
    ReachedTail = false;
    do
    {
        CurrentEntryPtr->EntryId--; /* Shifting positiion in linked list */
        if (CurrentEntryPtr->NextEntryPtr != NULL)
        {
            CurrentEntryPtr = CurrentEntryPtr->NextEntryPtr;
        }
        else
        {
            ReachedTail = true;
        }

    } while (ReachedTail == false);

    /* check that have found the tail, Tail EntryId should equal (NumEntriesMax.- 1) decrementd This ensure keep aligned */
    if (CurrentEntryPtr->EntryId != (ShadowTablePtr->NumEntriesMax - 2))
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_INTERNAL_ASSERTION;
    }

    EntryPtr->NextEntryPtr = NULL;
    EntryPtr->PreviousEntryPtr = NULL;
    /* will be the tail entry */
    EntryPtr->EntryId = ShadowTablePtr->NumEntriesMax - 1;

    /* Delete the Entry */
    EntryPtr->InUse = false;
    EntryPtr->Priority = 0;
    ShadowTablePtr->NumEntriesInUse--;

    CurrentEntryPtr->NextEntryPtr = EntryPtr;
    EntryPtr->PreviousEntryPtr = CurrentEntryPtr;

    return XIL_VITIS_NET_P4_SUCCESS;
}

XilVitisNetP4ReturnType XilVitisNetP4TinyCamPrvShadowTableFindEntry(XilVitisNetP4TinyCamPrvCtx *TinyCamPrvCtxPtr,
                                                          XilVitisNetP4TinyCamShadowTable *ShadowTablePtr,
                                                          uint8_t *KeyPtr,
                                                          uint8_t *MaskPtr,
                                                          XilVitisNetP4TinyCamEntry **FoundEntryPtrPtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_CAM_ERR_KEY_NOT_FOUND;
    XilVitisNetP4TinyCamEntry *CurrentEntryPtr;
    uint32_t KeySizeBytes = TinyCamPrvCtxPtr->PrivateVitisNetP4CamCtxPtr->KeyBufSizeBytes;
    int Match;

    /* start search */
    if (*FoundEntryPtrPtr == NULL)
    {
        CurrentEntryPtr = ShadowTablePtr->EntryListHeadPtr;
    }
    else
    {
        CurrentEntryPtr = *FoundEntryPtrPtr;
    }

    /* Search until found or end of list */
    do
    {
        if (CurrentEntryPtr->InUse == true)
        {
            Match = memcmp(KeyPtr, CurrentEntryPtr->KeyPtr, KeySizeBytes);
            if (Match == 0)
            {
                Match = memcmp(MaskPtr, CurrentEntryPtr->MaskPtr, KeySizeBytes);
            }

            if (Match == 0)
            {
                *FoundEntryPtrPtr = CurrentEntryPtr;
                Result = XIL_VITIS_NET_P4_SUCCESS;
            }
        }

        CurrentEntryPtr = CurrentEntryPtr->NextEntryPtr;

    } while ((*FoundEntryPtrPtr == NULL) && (CurrentEntryPtr != NULL));

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4TinyCamPrvIpShadowTableFindEntryByResponse(XilVitisNetP4TinyCamPrvCtx *TinyCamPrvCtxPtr,
                                                                      XilVitisNetP4TinyCamShadowTable *ShadowTablePtr,
                                                                      uint8_t *ResponsePtr,
                                                                      uint8_t *MaskPtr,
                                                                      XilVitisNetP4TinyCamEntry **FoundEntryPtrPtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_CAM_ERR_KEY_NOT_FOUND;
    XilVitisNetP4TinyCamEntry *CurrentEntryPtr;
    uint32_t ResponseSizeBytes = TinyCamPrvCtxPtr->PrivateVitisNetP4CamCtxPtr->ResponseBufSizeBytes;
    uint8_t ResponsePaddingMask = TinyCamPrvCtxPtr->PrivateVitisNetP4CamCtxPtr->ResponsePaddingMask;
    uint32_t BuffCount;
    bool Match = true;
    uint8_t UserResponseAndMaskValue;
    uint8_t EntryResponseAndMaskValue;

    /* start search from*/
    if (*FoundEntryPtrPtr == NULL)
    {
        CurrentEntryPtr = ShadowTablePtr->EntryListHeadPtr;
    }
    else
    {
        CurrentEntryPtr = *FoundEntryPtrPtr;
        *FoundEntryPtrPtr = NULL;
    }

    /* Search until found or end of list */
    do
    {
        if (CurrentEntryPtr->InUse == true)
        {
            Match = true;
            for (BuffCount = 0; BuffCount < ResponseSizeBytes; BuffCount++)
            {
                /* for the last byte apply the padding mask */
                if (BuffCount == (ResponseSizeBytes - 1))
                {
                    EntryResponseAndMaskValue = CurrentEntryPtr->ResponsePtr[BuffCount] & ResponsePaddingMask;
                    EntryResponseAndMaskValue = EntryResponseAndMaskValue & MaskPtr[BuffCount];

                    UserResponseAndMaskValue = ResponsePtr[BuffCount] & ResponsePaddingMask;
                    UserResponseAndMaskValue = UserResponseAndMaskValue & MaskPtr[BuffCount];
                }
                else
                {
                    EntryResponseAndMaskValue = CurrentEntryPtr->ResponsePtr[BuffCount] & MaskPtr[BuffCount];
                    UserResponseAndMaskValue = ResponsePtr[BuffCount] & MaskPtr[BuffCount];
                }

                if (UserResponseAndMaskValue != EntryResponseAndMaskValue)
                {
                    Match = false;
                    break;
                }
            }

            if (Match == true)
            {
                *FoundEntryPtrPtr = CurrentEntryPtr;
                Result = XIL_VITIS_NET_P4_SUCCESS;
            }
        }

        CurrentEntryPtr = CurrentEntryPtr->NextEntryPtr;

    } while ((*FoundEntryPtrPtr == NULL) && (CurrentEntryPtr != NULL));

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4TinyCamPrvShadowTableFindPivotEntryByPriority(XilVitisNetP4TinyCamShadowTable *ShadowTablePtr,
                                                                         uint32_t Priority,
                                                                         XilVitisNetP4TinyCamEntry **FoundEntryPtrPtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_GENERAL_ERR_INTERNAL_ASSERTION;
    XilVitisNetP4TinyCamEntry *CurrentEntryPtr = ShadowTablePtr->EntryListHeadPtr;

    while (CurrentEntryPtr != NULL)
    {
        if (CurrentEntryPtr->InUse == true)
        {
            if (CurrentEntryPtr->Priority > Priority)
            {
                *FoundEntryPtrPtr = CurrentEntryPtr->PreviousEntryPtr;
                Result = XIL_VITIS_NET_P4_SUCCESS;
                break;
            }
        }
        else
        {
            *FoundEntryPtrPtr = CurrentEntryPtr->PreviousEntryPtr;
            Result = XIL_VITIS_NET_P4_SUCCESS;
            break;
        }

        CurrentEntryPtr = CurrentEntryPtr->NextEntryPtr;
    }

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4TinyCamPrvShadowTableFillEntry(XilVitisNetP4TinyCamPrvCtx *TinyCamPrvCtxPtr,
                                                          XilVitisNetP4TinyCamEntry *EntryPtr,
                                                          uint8_t *KeyPtr,
                                                          uint8_t *MaskPtr,
                                                          uint32_t Priority,
                                                          uint8_t *ResponsePtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    uint32_t KeySizeBytes = TinyCamPrvCtxPtr->PrivateVitisNetP4CamCtxPtr->KeyBufSizeBytes;
    uint32_t ResponseSizeBytes = TinyCamPrvCtxPtr->PrivateVitisNetP4CamCtxPtr->ResponseBufSizeBytes;

    if (EntryPtr->InUse == true)
    {
        /* Should not occur should be checked before filling */
        Result = XIL_VITIS_NET_P4_GENERAL_ERR_INTERNAL_ASSERTION;
    }
    else
    {
        EntryPtr->Priority = Priority;
        memcpy(EntryPtr->KeyPtr, KeyPtr, KeySizeBytes);
        /* BCAM excepting the MAskPtr to be Null */
        if (MaskPtr != NULL)
        {
            memcpy(EntryPtr->MaskPtr, MaskPtr, KeySizeBytes);
        }
        else
        {
            memset(EntryPtr->MaskPtr, 0, KeySizeBytes);
        }
        memcpy(EntryPtr->ResponsePtr, ResponsePtr, ResponseSizeBytes);
        EntryPtr->InUse = true;
    }

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4TinyCamPrvShadowTableUpdateEntry(XilVitisNetP4TinyCamPrvCtx *TinyCamPrvCtxPtr,
                                                            XilVitisNetP4TinyCamEntry *EntryPtr,
                                                            uint8_t *ResponsePtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    uint32_t ResponseSizeBytes = TinyCamPrvCtxPtr->PrivateVitisNetP4CamCtxPtr->ResponseBufSizeBytes;

    if (EntryPtr->InUse == false)
    {
        /* Should not occur should be checked before updating */
        Result = XIL_VITIS_NET_P4_GENERAL_ERR_INTERNAL_ASSERTION;
    }
    else
    {
        memcpy(EntryPtr->ResponsePtr, ResponsePtr, ResponseSizeBytes);
    }

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4TinyCamPrvShadowTableEmptyEntry(XilVitisNetP4TinyCamPrvCtx *TinyCamPrvCtxPtr, XilVitisNetP4TinyCamEntry *EntryPtr)
{
    XIL_VITIS_NET_P4_UNUSED_ARG(TinyCamPrvCtxPtr);
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;

    EntryPtr->InUse = false;

    return Result;
}

XilVitisNetP4ReturnType GetHeapSizeTinyCam(XilVitisNetP4TinyCamPrvCtx** PrivateCtxPtrPtr,
                                      XilVitisNetP4EnvIf* EnvIfPtr,
                                      XilVitisNetP4CamConfig* ConfigPtr,
                                      XilVitisNetP4TinyCamEmulationMode EmulationMode,
                                      uint64_t* HeapSizePtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    
    Result = TinyCamCommonInit(PrivateCtxPtrPtr, EnvIfPtr, ConfigPtr, EmulationMode, true, HeapSizePtr);

    return Result;
}

/****************************************************************************************************************************************************/
/* SECTION: TinyCam used to Entries to the CAM IP Table*/
/****************************************************************************************************************************************************/

XilVitisNetP4ReturnType XilVitisNetP4TinyCamPrvIpTableDeleteEntry(XilVitisNetP4TinyCamPrvCtx *TinyCamPrvCtxPtr, XilVitisNetP4TinyCamEntry *EntryPtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;

    uint32_t EntryId = EntryPtr->EntryId;
    uint32_t EntryInUse;

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        Result = TinyCamHwSetEntryIdRegister(TinyCamPrvCtxPtr, EntryId);
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        Result = TinyCamHwSetControlRegisterRdFlag(TinyCamPrvCtxPtr);
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        Result = TinyCamHwGetControlRegisterEntryInUseBit(TinyCamPrvCtxPtr, &EntryInUse);
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        if (EntryInUse == 1)
        {
            Result = TinyCamHwSetControlRegisterEntryInUseDeletedFlag(TinyCamPrvCtxPtr);
        }
        else
        {
            /* This should not happen and could be an indication that the Shadow Table is out of sync with the CAM IP Table */
            Result = XIL_VITIS_NET_P4_GENERAL_ERR_INTERNAL_ASSERTION;
        }
    }

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4TinyCamPrvIpTableWriteEntry(XilVitisNetP4TinyCamPrvCtx *TinyCamPrvCtxPtr, XilVitisNetP4TinyCamEntry *EntryPtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    uint8_t *CamIpTableEntryBufferPtr;
    uint32_t EntryInUse;
    uint32_t NumReg32Writes;

    /*
     * Check to see if the table entry is been used, if yes then this is an internal error that
     * indicates that the ShadowTable and the Cam IP Table are out of sync
     *
     */
    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        Result = TinyCamHwSetEntryIdRegister(TinyCamPrvCtxPtr, EntryPtr->EntryId);
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        Result = TinyCamHwSetControlRegisterRdFlag(TinyCamPrvCtxPtr);
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        Result = TinyCamHwGetControlRegisterEntryInUseBit(TinyCamPrvCtxPtr, &EntryInUse);
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        if (EntryInUse == 1)
        {
            return XIL_VITIS_NET_P4_GENERAL_ERR_INTERNAL_ASSERTION;
        }
    }

    /* Setup the Buffer to write to CAM IP Table */
    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        CamIpTableEntryBufferPtr = EntryPtr->KeyPtr;
        NumReg32Writes = TinyCamPrvCtxPtr->TableInfo.EntrySizeWord32;
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        Result = TinyCamHwWriteEntryDataRegs(TinyCamPrvCtxPtr, CamIpTableEntryBufferPtr, NumReg32Writes);
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        Result = TinyCamHwSetControlRegisterEntryInUseAndWrFlag(TinyCamPrvCtxPtr);
    }

    return Result;
}


XilVitisNetP4ReturnType XilVitisNetP4TinyCamPrvIpTableRewriteEntry(XilVitisNetP4TinyCamPrvCtx *TinyCamPrvCtxPtr, XilVitisNetP4TinyCamEntry *EntryPtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    uint8_t *CamIpTableEntryBufferPtr;
    uint32_t NumReg32Writes;

    /*
     * Check to see if the table entry is been used, if yes then this is an internal error that
     * indicates that the ShadowTable and the Cam IP Table are out of sync
     *
     */
    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        Result = TinyCamHwSetEntryIdRegister(TinyCamPrvCtxPtr, EntryPtr->EntryId);
    }

    /* Setup the Buffer to write to CAM IP Table */
    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        CamIpTableEntryBufferPtr = EntryPtr->KeyPtr;
        NumReg32Writes = TinyCamPrvCtxPtr->TableInfo.EntrySizeWord32;
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        Result = TinyCamHwWriteEntryDataRegs(TinyCamPrvCtxPtr, CamIpTableEntryBufferPtr, NumReg32Writes);
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        Result = TinyCamHwSetControlRegisterEntryInUseAndWrFlag(TinyCamPrvCtxPtr);
    }

    return Result;
}


XilVitisNetP4ReturnType XilVitisNetP4TinyCamPrvIpTableReadEntry(XilVitisNetP4TinyCamPrvCtx *TinyCamPrvCtxPtr, XilVitisNetP4TinyCamEntry *EntryPtr)
{

    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;

    uint8_t *CamIpTableEntryBufferPtr;
    uint32_t EntryInUse;
    uint32_t NumReg32Writes;

    /*
     * Check to see if the table entry is been used, if yes then this is an internal error that
     * indicates that the ShadowTable and the Cam IP Table are out of sync
     *
     */
    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        Result = TinyCamHwSetEntryIdRegister(TinyCamPrvCtxPtr, EntryPtr->EntryId);
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        Result = TinyCamHwSetControlRegisterRdFlag(TinyCamPrvCtxPtr);
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        Result = TinyCamHwGetControlRegisterEntryInUseBit(TinyCamPrvCtxPtr, &EntryInUse);
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        /* If not in use an indication that Shadow IP table is out of sync */
        if (EntryInUse == 0)
        {                
            return XIL_VITIS_NET_P4_GENERAL_ERR_INTERNAL_ASSERTION;
        }
    }

    /* Setup the Buffer to write to CAM IP Table */
    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        CamIpTableEntryBufferPtr = EntryPtr->KeyPtr;
        NumReg32Writes = TinyCamPrvCtxPtr->TableInfo.EntrySizeWord32;
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        Result = TinyCamHwReadEntryDataRegs(TinyCamPrvCtxPtr, CamIpTableEntryBufferPtr, NumReg32Writes);
    }



    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4TinyCamPrvIpTableCheckEntryMatches(XilVitisNetP4TinyCamPrvCtx *TinyCamPrvCtxPtr,
                                                              XilVitisNetP4TinyCamEntry *EntryPtr,
                                                              bool *EntryMatchesPtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;

    uint8_t *CamIpTableEntryBufferPtr;
    uint32_t EntryInUse;
    uint32_t KeySizeBytes = TinyCamPrvCtxPtr->PrivateVitisNetP4CamCtxPtr->KeyBufSizeBytes;
    uint32_t NumReg32Writes;
    int MemCmpResult;
    bool EntryMatches = true;

    /*
     * Check to see if the table entry is been used, if yes then this is an internal error that
     * indicates that the ShadowTable and the Cam IP Table are out of sync
     */
    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        Result = TinyCamHwSetEntryIdRegister(TinyCamPrvCtxPtr, EntryPtr->EntryId);
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        Result = TinyCamHwSetControlRegisterRdFlag(TinyCamPrvCtxPtr);
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        Result = TinyCamHwGetControlRegisterEntryInUseBit(TinyCamPrvCtxPtr, &EntryInUse);
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        /* If not in use an indication that Shadow IP table is out of sync */
        if (EntryInUse == 0)
        {
            return XIL_VITIS_NET_P4_GENERAL_ERR_INTERNAL_ASSERTION;
        }
    }

    CamIpTableEntryBufferPtr = TinyCamPrvCtxPtr->ReadBufTableEntryPtr;

    /* Setup the Buffer to write to CAM IP Table */
    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        NumReg32Writes = TinyCamPrvCtxPtr->TableInfo.EntrySizeWord32;
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        Result = TinyCamHwReadEntryDataRegs(TinyCamPrvCtxPtr, CamIpTableEntryBufferPtr, NumReg32Writes);
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        if (TinyCamPrvCtxPtr->EmulationMode == XIL_VITIS_NET_P4_TINY_CAM_BCAM_EMULATION)
        {
            MemCmpResult = memcmp(EntryPtr->KeyPtr, CamIpTableEntryBufferPtr, KeySizeBytes);
            if (MemCmpResult != 0)
            {
                EntryMatches = false;
            }
        }
        else if (TinyCamPrvCtxPtr->EmulationMode == XIL_VITIS_NET_P4_TINY_CAM_TCAM_EMULATION)
        {

            MemCmpResult = memcmp(EntryPtr->KeyPtr, CamIpTableEntryBufferPtr, KeySizeBytes);
            if (MemCmpResult != 0)
            {
                EntryMatches = false;
            }

            MemCmpResult = memcmp(EntryPtr->MaskPtr, (CamIpTableEntryBufferPtr + KeySizeBytes), KeySizeBytes);
            if (MemCmpResult != 0)
            {
                EntryMatches = false;
            }
        }
    }

    *EntryMatchesPtr = EntryMatches;

        return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4TinyCamPrvIpTableCheckEntryEmpty(XilVitisNetP4TinyCamPrvCtx *TinyCamPrvCtxPtr,
                                                            XilVitisNetP4TinyCamEntry *EntryPtr,
                                                            bool *EntryEmptyPtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    uint32_t EntryInUse;

    /*
     * Check to see if the table entry is been used, if yes then this is an internal error that
     * indicates that the ShadowTable and the Cam IP Table are out of sync
     *
     */
    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        Result = TinyCamHwSetEntryIdRegister(TinyCamPrvCtxPtr, EntryPtr->EntryId);
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        Result = TinyCamHwSetControlRegisterRdFlag(TinyCamPrvCtxPtr);
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        Result = TinyCamHwGetControlRegisterEntryInUseBit(TinyCamPrvCtxPtr, &EntryInUse);
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        /* If not in use an indication that Shadow IP table is out of sync */
        if (EntryInUse == 0)
        {
            *EntryEmptyPtr = true;
        }
        else
        {
            *EntryEmptyPtr = false;
        }
    }

    return Result;
}

/****************************************************************************************************************************************************/
/* SECTION: Local function definitions - Register Access */
/****************************************************************************************************************************************************/

static XilVitisNetP4ReturnType TinyCamHwSetControlRegisterEntryInUseAndWrFlag(XilVitisNetP4TinyCamPrvCtx *TinyCamPrvCtxPtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    XilVitisNetP4AddressType Address;
    uint32_t RegValue;
    XilVitisNetP4CamCtx *PrivateCtxPtr = TinyCamPrvCtxPtr->PrivateVitisNetP4CamCtxPtr;

    Address = (PrivateCtxPtr->BaseAddr + XIL_VITIS_NET_P4_TINY_CAM_CONTROL_REG);
    RegValue = (uint32_t)(XIL_VITIS_NET_P4_TINY_CAM_WRITE_AND_ENTRY_IN_USE);
    Result = PrivateCtxPtr->EnvIf.WordWrite32(&(PrivateCtxPtr->EnvIf), Address, RegValue);

    return Result;
}

static XilVitisNetP4ReturnType TinyCamHwSetControlRegisterEntryInUseDeletedFlag(XilVitisNetP4TinyCamPrvCtx *TinyCamPrvCtxPtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    XilVitisNetP4AddressType Address;
    uint32_t RegValue;
    XilVitisNetP4CamCtx *PrivateCtxPtr = TinyCamPrvCtxPtr->PrivateVitisNetP4CamCtxPtr;

    Address = (PrivateCtxPtr->BaseAddr + XIL_VITIS_NET_P4_TINY_CAM_CONTROL_REG);
    RegValue = (uint32_t)(XIL_VITIS_NET_P4_TINY_CAM_DELETE);
    Result = PrivateCtxPtr->EnvIf.WordWrite32(&(PrivateCtxPtr->EnvIf), Address, RegValue);

    return Result;
}

static XilVitisNetP4ReturnType TinyCamHwGetControlRegisterEntryInUseBit(XilVitisNetP4TinyCamPrvCtx *TinyCamPrvCtxPtr, uint32_t *EntryInUseBitPtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    XilVitisNetP4AddressType Address;
    uint32_t RegValue;
    XilVitisNetP4CamCtx *PrivateCtxPtr = TinyCamPrvCtxPtr->PrivateVitisNetP4CamCtxPtr;

    Address = (PrivateCtxPtr->BaseAddr + XIL_VITIS_NET_P4_TINY_CAM_CONTROL_REG);
    Result = PrivateCtxPtr->EnvIf.WordRead32(&(PrivateCtxPtr->EnvIf), Address, &RegValue);

    *EntryInUseBitPtr = (RegValue & XIL_VITIS_NET_P4_TINY_CAM_ENTRY_IN_USE_MASK) >> XIL_VITIS_NET_P4_TINY_CAM_ENTRY_IN_USE_BIT;

    return Result;
}

static XilVitisNetP4ReturnType TinyCamHwSetControlRegisterRdFlag(XilVitisNetP4TinyCamPrvCtx *TinyCamPrvCtxPtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    XilVitisNetP4AddressType Address;
    uint32_t RegValue;
    XilVitisNetP4CamCtx *PrivateCtxPtr = TinyCamPrvCtxPtr->PrivateVitisNetP4CamCtxPtr;

    Address = (PrivateCtxPtr->BaseAddr + XIL_VITIS_NET_P4_TINY_CAM_CONTROL_REG);
    RegValue = (uint32_t)(XIL_VITIS_NET_P4_TINY_CAM_READ);
    Result = PrivateCtxPtr->EnvIf.WordWrite32(&(PrivateCtxPtr->EnvIf), Address, RegValue);

    return Result;
}

static XilVitisNetP4ReturnType TinyCamHwSetEntryIdRegister(XilVitisNetP4TinyCamPrvCtx *TinyCamPrvCtxPtr, uint32_t EntryNum)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    XilVitisNetP4AddressType Address;
    XilVitisNetP4CamCtx *PrivateCtxPtr = TinyCamPrvCtxPtr->PrivateVitisNetP4CamCtxPtr;

    Address = (PrivateCtxPtr->BaseAddr + XIL_VITIS_NET_P4_TINY_CAM_ENTRY_ID_REG);
    Result = PrivateCtxPtr->EnvIf.WordWrite32(&(PrivateCtxPtr->EnvIf), Address, EntryNum);

    return Result;
}

static XilVitisNetP4ReturnType TinyCamHwGetEmulationMode(XilVitisNetP4TinyCamPrvCtx *TinyCamPrvCtxPtr, XilVitisNetP4TinyCamEmulationMode *EmulationModePtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    XilVitisNetP4AddressType Address;
    uint32_t RegValue;
    XilVitisNetP4TinyCamEmulationMode EmulationMode = XIL_VITIS_NET_P4_TINY_CAM_NUM_EMULATION_MODES;
    XilVitisNetP4CamCtx *PrivateCtxPtr = TinyCamPrvCtxPtr->PrivateVitisNetP4CamCtxPtr;

    Address = (PrivateCtxPtr->BaseAddr + XIL_VITIS_NET_P4_TINY_CAM_EMULATION_MODE_REG);
    Result = PrivateCtxPtr->EnvIf.WordRead32(&(PrivateCtxPtr->EnvIf), Address, &RegValue);

    switch (RegValue)
    {
        case XIL_VITIS_NET_P4_TINY_CAM_HW_BCAM_EMULATION_VALUE:
            EmulationMode = XIL_VITIS_NET_P4_TINY_CAM_BCAM_EMULATION;
            break;
        case XIL_VITIS_NET_P4_TINY_CAM_HW_TCAM_EMULATION_VALUE:
            EmulationMode = XIL_VITIS_NET_P4_TINY_CAM_TCAM_EMULATION;
            break;
        default:
            Result = XIL_VITIS_NET_P4_GENERAL_ERR_INTERNAL_ASSERTION;
            break;
    }

    *EmulationModePtr = EmulationMode;

    return Result;
}

static XilVitisNetP4ReturnType XilVitisNetP4TinyCamCalcHeapShadowTable(XilVitisNetP4TinyCamPrvCtx* TinyCamPrvCtxPtr, uint64_t* HeapSizePtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    uint32_t PaddedEntrySizeBytes;
    uint32_t ResponseSizeBytes = TinyCamPrvCtxPtr->PrivateVitisNetP4CamCtxPtr->ResponseBufSizeBytes;
    uint32_t KeySizeBytes = TinyCamPrvCtxPtr->PrivateVitisNetP4CamCtxPtr->KeyBufSizeBytes;
    uint32_t NumEntries = TinyCamPrvCtxPtr->PrivateVitisNetP4CamCtxPtr->NumEntries;
    uint64_t HeapSizeEntryListHead, HeapSizeEntryArrayMem;

    PaddedEntrySizeBytes = ((2 * KeySizeBytes) + ResponseSizeBytes) * 4;
    PaddedEntrySizeBytes += PaddedEntrySizeBytes % XIL_VITIS_NET_P4_BYTES_PER_REG;
    HeapSizeEntryListHead = (uint64_t)(NumEntries) * sizeof(XilVitisNetP4TinyCamEntry);
    HeapSizeEntryArrayMem = (uint64_t)(NumEntries) * PaddedEntrySizeBytes;
    *HeapSizePtr = HeapSizeEntryListHead + HeapSizeEntryArrayMem;

    return Result;
}

static XilVitisNetP4ReturnType TinyCamHwWriteEntryDataRegs(XilVitisNetP4TinyCamPrvCtx *TinyCamPrvCtxPtr, uint8_t *BufferPtr, uint32_t NumReg32Writes)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    XilVitisNetP4CamCtx *PrivateCamCtxPtr;
    XilVitisNetP4AddressType Address;
    uint32_t RegWriteValue;
    uint32_t Count;
    uint32_t Offset;
    uint32_t ReponseCountStart;

    PrivateCamCtxPtr = TinyCamPrvCtxPtr->PrivateVitisNetP4CamCtxPtr;
    /*
     * Only write a full table entry when the tiny cam emulates a TCAM and the Key Size is less than Register Size.
     * Reduce the IO Transactions as much as possible
     */
    if ((TinyCamPrvCtxPtr->EmulationMode == XIL_VITIS_NET_P4_TINY_CAM_TCAM_EMULATION) || (PrivateCamCtxPtr->KeyBufSizeBytes < XIL_VITIS_NET_P4_BYTES_PER_REG))
    {
        for (Count = 0; Count < NumReg32Writes; Count++)
        {
            Offset = Count * XIL_VITIS_NET_P4_BYTES_PER_REG;
            Address = PrivateCamCtxPtr->BaseAddr + XIL_VITIS_NET_P4_TINY_CAM_DATA_REG_PART_1_REG + Offset;
            RegWriteValue = *((uint32_t *)(BufferPtr + Offset));
            Result = PrivateCamCtxPtr->EnvIf.WordWrite32(&(PrivateCamCtxPtr->EnvIf), Address, RegWriteValue);
        }
    }
    else
    {
        /* Write the Key only */
        for (Count = 0; Count < TinyCamPrvCtxPtr->TableInfo.NumKeyRegisters; Count++)
        {
            Offset = Count * XIL_VITIS_NET_P4_BYTES_PER_REG;
            Address = PrivateCamCtxPtr->BaseAddr + XIL_VITIS_NET_P4_TINY_CAM_DATA_REG_PART_1_REG + Offset;
            RegWriteValue = *((uint32_t *)(BufferPtr + Offset));
            Result = PrivateCamCtxPtr->EnvIf.WordWrite32(&(PrivateCamCtxPtr->EnvIf), Address, RegWriteValue);
        }

        ReponseCountStart = (2 * PrivateCamCtxPtr->KeyBufSizeBytes) / XIL_VITIS_NET_P4_BYTES_PER_REG;

        /* Write the Response only */
        for (Count = ReponseCountStart; Count < (ReponseCountStart + TinyCamPrvCtxPtr->TableInfo.NumResponseRegisters); Count++)
        {
            Offset = Count * XIL_VITIS_NET_P4_BYTES_PER_REG;
            Address = PrivateCamCtxPtr->BaseAddr + XIL_VITIS_NET_P4_TINY_CAM_DATA_REG_PART_1_REG + Offset;
            RegWriteValue = *((uint32_t *)(BufferPtr + Offset));
            Result = PrivateCamCtxPtr->EnvIf.WordWrite32(&(PrivateCamCtxPtr->EnvIf), Address, RegWriteValue);
        }
    }

    return Result;
}

/* Copy a table entry from the data registers to the context.*/
static XilVitisNetP4ReturnType TinyCamHwReadEntryDataRegs(XilVitisNetP4TinyCamPrvCtx *TinyCamPrvCtxPtr, uint8_t *BufferPtr, uint32_t NumReg32Reads)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    XilVitisNetP4CamCtx *PrivateCamCtxPtr;
    XilVitisNetP4AddressType Address;
    uint32_t RegValue;
    uint32_t Count;
    uint32_t Offset;
    uint32_t ReponseCountStart;

    PrivateCamCtxPtr = TinyCamPrvCtxPtr->PrivateVitisNetP4CamCtxPtr;

    if ((TinyCamPrvCtxPtr->EmulationMode == XIL_VITIS_NET_P4_TINY_CAM_TCAM_EMULATION) || (PrivateCamCtxPtr->KeyBufSizeBytes < XIL_VITIS_NET_P4_BYTES_PER_REG))
    {
        for (Count = 0; Count < NumReg32Reads; Count++)
        {
            Offset = Count * XIL_VITIS_NET_P4_BYTES_PER_REG;
            Address = TinyCamPrvCtxPtr->PrivateVitisNetP4CamCtxPtr->BaseAddr + XIL_VITIS_NET_P4_TINY_CAM_DATA_REG_PART_1_REG + Offset;
            Result = TinyCamPrvCtxPtr->PrivateVitisNetP4CamCtxPtr->EnvIf.WordRead32(&(TinyCamPrvCtxPtr->PrivateVitisNetP4CamCtxPtr->EnvIf), Address, &RegValue);
            *((uint32_t *)(BufferPtr + Offset)) = RegValue;
        }
    }
    else
    {
        /* Read the Key only */
        for (Count = 0; Count < TinyCamPrvCtxPtr->TableInfo.NumKeyRegisters; Count++)
        {
            Offset = Count * XIL_VITIS_NET_P4_BYTES_PER_REG;
            Address = TinyCamPrvCtxPtr->PrivateVitisNetP4CamCtxPtr->BaseAddr + XIL_VITIS_NET_P4_TINY_CAM_DATA_REG_PART_1_REG + Offset;
            Result = TinyCamPrvCtxPtr->PrivateVitisNetP4CamCtxPtr->EnvIf.WordRead32(&(TinyCamPrvCtxPtr->PrivateVitisNetP4CamCtxPtr->EnvIf), Address, &RegValue);
            *((uint32_t *)(BufferPtr + Offset)) = RegValue;
        }

        ReponseCountStart = (2 * PrivateCamCtxPtr->KeyBufSizeBytes) / XIL_VITIS_NET_P4_BYTES_PER_REG;

        /* Write the Response only */
        for (Count = ReponseCountStart; Count < (ReponseCountStart + TinyCamPrvCtxPtr->TableInfo.NumResponseRegisters); Count++)
        {
            Offset = Count * XIL_VITIS_NET_P4_BYTES_PER_REG;
            Address = TinyCamPrvCtxPtr->PrivateVitisNetP4CamCtxPtr->BaseAddr + XIL_VITIS_NET_P4_TINY_CAM_DATA_REG_PART_1_REG + Offset;
            Result = TinyCamPrvCtxPtr->PrivateVitisNetP4CamCtxPtr->EnvIf.WordRead32(&(TinyCamPrvCtxPtr->PrivateVitisNetP4CamCtxPtr->EnvIf), Address, &RegValue);
            *((uint32_t *)(BufferPtr + Offset)) = RegValue;
        }
    }

    return Result;
}
