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
-- Description : private header file to store cam top common declarations
--
--------------------------------------------------------------------------------
*/

#ifndef CAM_PRIV_H
#define CAM_PRIV_H
/****************************************************************************************************************************************************/
/* SECTION: Header includes */
/****************************************************************************************************************************************************/
#include "cam.h"
#include "cam_top.h"
#include "vitisnetp4_common.h"
#include "tiny_cam.h"

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************************************************************************************/
/* SECTION: Constants */
/****************************************************************************************************************************************************/

/****************************************************************************************************************************************************/
/* SECTION: Type definitions */
/****************************************************************************************************************************************************/
typedef enum XilVitisNetP4SwapCmd
{
    VITISNETP4_CAM_SWAP_KEY,
    VITISNETP4_CAM_SWAP_KEY_MASK,
    VITISNETP4_CAM_SWAP_RESPONSE,
    VITISNETP4_CAM_SWAP_RESPONSE_MASK
} XilVitisNetP4SwapCmd;

typedef union XilVitisNetP4CamUnion
{
    struct bcam *BcamPtr;
    struct tcam *TcamPtr;
    struct stcam *StcamPtr;
} XilVitisNetP4CamUnion;

struct XilVitisNetP4CamCtx
{
    XilVitisNetP4EnvIf EnvIf;
    XilVitisNetP4AddressType BaseAddr;
    XilVitisNetP4Endian Endian;
    uint32_t NumEntries; /* Number of entries that are in the CAM */

    uint32_t ResponseSizeBits;
    uint32_t ResponsePaddingSizeBits; /* number of extra bits beyond 8 bit boundary in response*/
    uint32_t ResponseBufSizeBytes;
    uint8_t ResponsePaddingMask; /* Mask for the last response byte. Bits that are allowed are 'hi' */

    uint32_t KeySizeBits;
    uint32_t KeyPaddingSizeBits; /* number of extra bits beyond 8 bit boundary in response*/
    uint32_t KeyBufSizeBytes;
    uint8_t KeyPaddingMask; /* Mask for the last key byte. Bits that are allowed are 'hi' */

    uint8_t *KeyBufPtr;
    uint8_t *MaskBufPtr;
    uint8_t *ResponseBufPtr;
    uint8_t *ResponseMaskBufPtr;

    XilVitisNetP4CamUnion CamUnion;
    cam_arg_t *CamArgPtr;
};

typedef struct XilVitisNetP4TinyCamTableStats
{
    uint32_t TotalSizeBits;   /* Total table size in bits. This includes all padding i.e.*/
    uint32_t EntrySizeBytes;  /* Size (in bytes) of a single table entry with key-mask-response data. This does not include extra padding bytes for
                                 32-bit alignment */
    uint32_t EntrySizeWord32; /* Size (in 32-bit words) of a single table entry. This will include padding so that each table entry ends on a 32-bit
                                 boundary */
    uint32_t NumPaddingBytes; /* Number of padding bytes added to the end of the buffer to ensure that the table entry is boundary aligned to a 32-bit register */
    /* For Bcam Mode emulations to ensure used to reduce number register writes to a minimum */
    uint32_t NumKeyRegisters;
    uint32_t NumResponseRegisters;
} XilSdndetTinyCamTableStats;

/* Manages single entry in Entry Array*/
typedef struct XilVitisNetP4TinyCamEntry
{
    bool InUse;
    uint32_t Priority;
    uint8_t EntryId;
    uint8_t *KeyPtr;
    uint8_t *MaskPtr;
    uint8_t *ResponsePtr;
    struct XilVitisNetP4TinyCamEntry *NextEntryPtr;
    struct XilVitisNetP4TinyCamEntry *PreviousEntryPtr;
} XilVitisNetP4TinyCamEntry;

typedef struct XilVitisNetP4TinyCamShadowTable
{
    uint32_t NumEntriesMax;
    uint32_t NumEntriesInUse;
    XilVitisNetP4TinyCamEntry *EntryListHeadPtr; /* Track as the head will move when re-ordering the list */
    uint8_t *EntryListMemPtr;
    uint8_t *EntryArrayMemPtr;
} XilVitisNetP4TinyCamShadowTable;

struct XilVitisNetP4TinyCamPrvCtx
{
    XilVitisNetP4CamCtx *PrivateVitisNetP4CamCtxPtr; /* Internal Tiny Cam context data used by driver */
    XilSdndetTinyCamTableStats TableInfo;  /* Description of the table entry */
    XilVitisNetP4TinyCamShadowTable ShadowTable;

    XilVitisNetP4TinyCamEmulationMode EmulationMode;

    uint8_t *WriteBufTableEntryPtr;
    uint8_t *ReadBufTableEntryPtr;
};

/****************************************************************************************************************************************************/
/* SECTION: CAM common function prototypes */
/****************************************************************************************************************************************************/

XilVitisNetP4ReturnType XilVitisNetP4CamByteSwap(XilVitisNetP4CamCtx *CtxPtr, uint8_t *SrcDataPtr, uint8_t *DstDataPtr, XilVitisNetP4SwapCmd Cmd);

XilVitisNetP4ReturnType XilVitisNetP4CamValidateCtx(XilVitisNetP4CamCtx *CtxPtr);

XilVitisNetP4ReturnType XilVitisNetP4CamCtxCreate(XilVitisNetP4CamCtx **CtxPtr, const XilVitisNetP4EnvIf *EnvIfPtr, XilVitisNetP4CamConfig *ConfigPtr, bool CalcHeapSize, ...);

XilVitisNetP4ReturnType XilVitisNetP4CamCtxDestroy(XilVitisNetP4CamCtx *CtxPtr);

/****************************************************************************************************************************************************/
/* SECTION: Internal TinyCam Function prototypes to facilate grey box testing */
/****************************************************************************************************************************************************/
XilVitisNetP4ReturnType XilVitisNetP4TinyCamPrvShadowTableCreate(XilVitisNetP4TinyCamPrvCtx *TinyCamPrvCtxPtr, XilVitisNetP4TinyCamShadowTable *ShadowTablePtr);

XilVitisNetP4ReturnType XilVitisNetP4TinyCamPrvShadowTableDestroy(XilVitisNetP4TinyCamShadowTable *ShadowTablePtr);

XilVitisNetP4ReturnType XilVitisNetP4TinyCamPrvShadowTableNextEmptyEntry(XilVitisNetP4TinyCamShadowTable *ShadowTablePtr, XilVitisNetP4TinyCamEntry **EmptyEntryPtrPtr);

XilVitisNetP4ReturnType XilVitisNetP4TinyCamPrvShadowTableGetEntryById(XilVitisNetP4TinyCamShadowTable *ShadowTablePtr,
                                                             uint32_t EntryId,
                                                             XilVitisNetP4TinyCamEntry **EmptyEntryPtrPtr);

/* Null for Current indicates at the HEAD, will also remove the Insert Entry from the list as the list will not add new entries only rearranged
 * existing entries */
XilVitisNetP4ReturnType XilVitisNetP4TinyCamPrvShadowTableInsertEntryAfter(XilVitisNetP4TinyCamShadowTable *ShadowTablePtr,
                                                                 XilVitisNetP4TinyCamEntry *CurrentEntryPtr,
                                                                 XilVitisNetP4TinyCamEntry *InsertEntryPtr);

XilVitisNetP4ReturnType XilVitisNetP4TinyCamPrvShadowTableMoveEmptyEntryToTail(XilVitisNetP4TinyCamShadowTable *ShadowTablePtr,
                                                                      XilVitisNetP4TinyCamEntry *CurrentEntryPtr);

XilVitisNetP4ReturnType XilVitisNetP4TinyCamPrvShadowTableFindEntry(XilVitisNetP4TinyCamPrvCtx *TinyCamPrvCtxPtr,
                                                          XilVitisNetP4TinyCamShadowTable *ShadowTablePtr,
                                                          uint8_t *KeyPtr,
                                                          uint8_t *MaskPtr,
                                                          XilVitisNetP4TinyCamEntry **FoundEntryPtrPtr);

XilVitisNetP4ReturnType XilVitisNetP4TinyCamPrvIpShadowTableFindEntryByResponse(XilVitisNetP4TinyCamPrvCtx *TinyCamPrvCtxPtr,
                                                                      XilVitisNetP4TinyCamShadowTable *ShadowTablePtr,
                                                                      uint8_t *ResponsePtr,
                                                                      uint8_t *MaskPtr,
                                                                      XilVitisNetP4TinyCamEntry **FoundEntryPtrPtr);

/* Finds the entry in shadow table where all entires after the found entry will have a lower priority. For case
 * where no lower priority entries exist the Found entry will be assigned NULL, i.e. need to insert higher priority
 * at the head of the shadow tail
 */
XilVitisNetP4ReturnType XilVitisNetP4TinyCamPrvShadowTableFindPivotEntryByPriority(XilVitisNetP4TinyCamShadowTable *ShadowTablePtr,
                                                                          uint32_t Priority,
                                                                          XilVitisNetP4TinyCamEntry **FoundEntryPtrPtr);

/* Should not attempt to fill an entry that is InUse, will return an error of XIL_VITIS_NET_P4_GENERAL_ERR_INTERNAL_ASSERTION,
 * would be an internal error from a user point of view
 */
XilVitisNetP4ReturnType XilVitisNetP4TinyCamPrvShadowTableFillEntry(XilVitisNetP4TinyCamPrvCtx *TinyCamPrvCtxPtr,
                                                          XilVitisNetP4TinyCamEntry *EntryPtr,
                                                          uint8_t *KeyPtr,
                                                          uint8_t *MaskPtr,
                                                          uint32_t Priority,
                                                          uint8_t *ResponsePtr);

XilVitisNetP4ReturnType XilVitisNetP4TinyCamPrvShadowTableUpdateEntry(XilVitisNetP4TinyCamPrvCtx *TinyCamPrvCtxPtr,
                                                            XilVitisNetP4TinyCamEntry *EntryPtr,
                                                            uint8_t *ResponsePtr);

XilVitisNetP4ReturnType XilVitisNetP4TinyCamPrvShadowTableEmptyEntry(XilVitisNetP4TinyCamPrvCtx *TinyCamPrvCtxPtr, XilVitisNetP4TinyCamEntry *EntryPtr);
XilVitisNetP4ReturnType GetHeapSizeTinyCam(XilVitisNetP4TinyCamPrvCtx** PrivateCtxPtrPtr,
                                      XilVitisNetP4EnvIf* EnvIfPtr,
                                      XilVitisNetP4CamConfig* ConfigPtr,
                                      XilVitisNetP4TinyCamEmulationMode EmulationMode,
                                      uint64_t* HeapSizePtr);

/****************************************************************************************************************************************************/
/* SECTION: TinyCAM  functions specific to TCAM Emulation Mode*/
/****************************************************************************************************************************************************/

/****************************************************************************************************************************************************/
/* SECTION: TinyCAM  functions specific to BCAM Emulation Mode*/
/****************************************************************************************************************************************************/

/****************************************************************************************************************************************************/
/* SECTION: TinyCam used to Entries to the CAM IP Table*/
/****************************************************************************************************************************************************/
XilVitisNetP4ReturnType XilVitisNetP4TinyCamPrvIpTableDeleteEntry(XilVitisNetP4TinyCamPrvCtx *TinyCamPrvCtxPtr, XilVitisNetP4TinyCamEntry *EntryPtr);

XilVitisNetP4ReturnType XilVitisNetP4TinyCamPrvIpTableWriteEntry(XilVitisNetP4TinyCamPrvCtx *TinyCamPrvCtxPtr, XilVitisNetP4TinyCamEntry *EntryPtr);

XilVitisNetP4ReturnType XilVitisNetP4TinyCamPrvIpTableRewriteEntry(XilVitisNetP4TinyCamPrvCtx *TinyCamPrvCtxPtr, XilVitisNetP4TinyCamEntry *EntryPtr);

XilVitisNetP4ReturnType XilVitisNetP4TinyCamPrvIpTableReadEntry(XilVitisNetP4TinyCamPrvCtx *TinyCamPrvCtxPtr, XilVitisNetP4TinyCamEntry *EntryPtr);

XilVitisNetP4ReturnType XilVitisNetP4TinyCamPrvIpTableCheckEntryMatches(XilVitisNetP4TinyCamPrvCtx *TinyCamPrvCtxPtr,
                                                              XilVitisNetP4TinyCamEntry *EntryPtr,
                                                              bool *EntryMatchesPtr);

XilVitisNetP4ReturnType XilVitisNetP4TinyCamPrvIpTableCheckEntryEmpty(XilVitisNetP4TinyCamPrvCtx *TinyCamPrvCtxPtr,
                                                            XilVitisNetP4TinyCamEntry *EntryPtr,
                                                            bool *EntryEmptyPtr);

#ifdef __cplusplus
}
#endif

#endif // CAM_PRIV_H