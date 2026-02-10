/*
-- (c) Copyright 2019 Xilinx, Inc. All rights reserved.
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
-- Date           : $DateTime: 2021/03/11 06:56:30 $
-- Last Author    : $Author: dkiernan $
--
--------------------------------------------------------------------------------
-- Description : Implementation of the VitisNetP4target driver
--
--------------------------------------------------------------------------------
*/

/****************************************************************************************************************************************************/
/* SECTION: Header includes */
/****************************************************************************************************************************************************/

#include "vitisnetp4_target.h"
#include <string.h>

/****************************************************************************************************************************************************/
/* SECTION: Constants/macros */
/****************************************************************************************************************************************************/
#define XIL_VITIS_NET_P4_TARGET_SW_VERSION_MAJOR_NUM    (1)
#define XIL_VITIS_NET_P4_TARGET_SW_VERSION_MINOR_NUM    (0)
#define XIL_VITIS_NET_P4_TARGET_SW_VERSION_REVISION_NUM (0)

/****************************************************************************************************************************************************/
/* SECTION: Type defintions */
/****************************************************************************************************************************************************/

typedef struct XilVitisNetP4TargetTableCtx
{
    char *           NameStringPtr;
    uint32_t         NameSizeBytes;
    XilVitisNetP4TableCtx TableCtx;
} XilVitisNetP4TargetTableCtx;

typedef struct XilVitisNetP4TargetMgmtCtx
{
    XilVitisNetP4TargetBuildInfoCtx *BuildInfoCtxPtr;
    XilVitisNetP4TargetInterruptCtx *InterruptCtxPtr;
    XilVitisNetP4TargetCtrlCtx      *ControlCtxPtr;
} XilVitisNetP4TargetMgmtCtx;

struct XilVitisNetP4TargetPrivateCtx
{
    uint32_t                NumTables;
    XilVitisNetP4TargetTableCtx *TableCtxArrayPtr;
    XilVitisNetP4TargetMgmtCtx   MgmtCtx;
};

/****************************************************************************************************************************************************/
/* SECTION: Local function declarations*/
/****************************************************************************************************************************************************/

static XilVitisNetP4ReturnType XilVitisNetP4TargetTablesInit(XilVitisNetP4TargetPrivateCtx *CtxPtr, XilVitisNetP4EnvIf *EnvIfPtr, XilVitisNetP4TargetConfig *ConfigPtr);
static XilVitisNetP4ReturnType XilVitisNetP4TargetTablesExit(XilVitisNetP4TargetPrivateCtx *CtxPtr);
static XilVitisNetP4ReturnType XilVitisNetP4TargetMgmtInit(XilVitisNetP4TargetMgmtCtx *CtxPtr, XilVitisNetP4EnvIf *EnvIfPtr, XilVitisNetP4TargetConfig *ConfigPtr);
static XilVitisNetP4ReturnType XilVitisNetP4TargetMgmtExit(XilVitisNetP4TargetMgmtCtx *CtxPtr);
static XilVitisNetP4ReturnType XilVitisNetP4TargetCheckCompatibleHwVersion(XilVitisNetP4TargetBuildInfoCtx *BuildInfoCtxPtr);

/****************************************************************************************************************************************************/
/* SECTION: Global function definitions */
/****************************************************************************************************************************************************/

XilVitisNetP4ReturnType XilVitisNetP4TargetInit(XilVitisNetP4TargetCtx *CtxPtr, XilVitisNetP4EnvIf *EnvIfPtr, XilVitisNetP4TargetConfig *ConfigPtr)
{
    XilVitisNetP4ReturnType        Result            = XIL_VITIS_NET_P4_SUCCESS;
    XilVitisNetP4TargetPrivateCtx *AllocTargetCtxPtr = NULL;

    if ((CtxPtr == NULL) || (EnvIfPtr == NULL) || (ConfigPtr == NULL))
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    Result = XilVitisNetP4ValidateEnvIf(EnvIfPtr);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        return Result;
    }

    /* Allocating top level target context structure */
    AllocTargetCtxPtr = calloc(1, sizeof(XilVitisNetP4TargetPrivateCtx));
    if (AllocTargetCtxPtr == NULL)
    {
        Result = XIL_VITIS_NET_P4_TARGET_ERR_MALLOC_FAILED;
        goto fail_target_ctx_alloc;
    }

    // Call Function, to init tables
    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        Result = XilVitisNetP4TargetTablesInit(AllocTargetCtxPtr, EnvIfPtr, ConfigPtr);
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        Result = XilVitisNetP4TargetMgmtInit(&(AllocTargetCtxPtr->MgmtCtx), EnvIfPtr, ConfigPtr);
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        /* Compatibility check can only be implement if the BuildInfo Driver is present */
        if (AllocTargetCtxPtr->MgmtCtx.BuildInfoCtxPtr != NULL)
        {
            Result = XilVitisNetP4TargetCheckCompatibleHwVersion(AllocTargetCtxPtr->MgmtCtx.BuildInfoCtxPtr);
        }
    }

    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        CtxPtr->PrivateCtxPtr = AllocTargetCtxPtr;
    }
    else
    {
        XilVitisNetP4TargetMgmtExit(&(AllocTargetCtxPtr->MgmtCtx));
        XilVitisNetP4TargetTablesExit(AllocTargetCtxPtr);
        free(AllocTargetCtxPtr);
        CtxPtr->PrivateCtxPtr = NULL;
    }

fail_target_ctx_alloc:
    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4TargetExit(XilVitisNetP4TargetCtx *CtxPtr)
{
    XilVitisNetP4ReturnType        Result        = XIL_VITIS_NET_P4_SUCCESS;
    XilVitisNetP4TargetPrivateCtx *PrivateCtxPtr = NULL;

    if ((CtxPtr == NULL))
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    if (CtxPtr->PrivateCtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_INVALID_CONTEXT;
    }

    PrivateCtxPtr = CtxPtr->PrivateCtxPtr;

    XilVitisNetP4TargetTablesExit(PrivateCtxPtr);
    XilVitisNetP4TargetMgmtExit(&(PrivateCtxPtr->MgmtCtx));
    free(CtxPtr->PrivateCtxPtr);
    CtxPtr->PrivateCtxPtr = NULL;

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4TargetGetSwVersion(XilVitisNetP4TargetCtx *CtxPtr, XilVitisNetP4Version *SwVersionPtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;

    if ((CtxPtr == NULL) || (SwVersionPtr == NULL))
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    if (CtxPtr->PrivateCtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_INVALID_CONTEXT;
    }

    SwVersionPtr->Major    = XIL_VITIS_NET_P4_TARGET_SW_VERSION_MAJOR_NUM;
    SwVersionPtr->Minor    = XIL_VITIS_NET_P4_TARGET_SW_VERSION_MINOR_NUM;
    SwVersionPtr->Revision = XIL_VITIS_NET_P4_TARGET_SW_VERSION_REVISION_NUM;

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4TargetGetTableByName(XilVitisNetP4TargetCtx *CtxPtr, char *TableNamePtr, XilVitisNetP4TableCtx **TableCtxPtrPtr)
{
    XilVitisNetP4ReturnType        Result;
    XilVitisNetP4TargetPrivateCtx *PrivateCtxPtr     = NULL;
    XilVitisNetP4TargetTableCtx *  TargetTableCtxPtr = NULL;
    uint32_t                  TableIndex        = 0;
    int                       StrcmpResult      = 0;

    if ((CtxPtr == NULL) || (TableNamePtr == NULL) || (TableCtxPtrPtr == NULL))
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    if (CtxPtr->PrivateCtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_INVALID_CONTEXT;
    }

    PrivateCtxPtr = CtxPtr->PrivateCtxPtr;

    /* Assumes no match by default - overriden if a match is found */
    Result = XIL_VITIS_NET_P4_TARGET_ERR_TABLE_NOT_FOUND;
    for (TableIndex = 0; TableIndex < PrivateCtxPtr->NumTables; TableIndex++)
    {
        TargetTableCtxPtr = &(PrivateCtxPtr->TableCtxArrayPtr[TableIndex]);
        StrcmpResult      = strcmp(TableNamePtr, TargetTableCtxPtr->NameStringPtr);
        if (StrcmpResult == 0)
        {
            *TableCtxPtrPtr = &(TargetTableCtxPtr->TableCtx);
            Result          = XIL_VITIS_NET_P4_SUCCESS;
            break;
        }
    }

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4TargetGetTableByIndex(XilVitisNetP4TargetCtx *CtxPtr, uint32_t Index, XilVitisNetP4TableCtx **TableCtxPtrPtr)
{
    XilVitisNetP4ReturnType        Result            = XIL_VITIS_NET_P4_SUCCESS;
    XilVitisNetP4TargetPrivateCtx *PrivateCtxPtr     = NULL;
    XilVitisNetP4TargetTableCtx *  TargetTableCtxPtr = NULL;

    if ((CtxPtr == NULL) || (TableCtxPtrPtr == NULL))
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    if (CtxPtr->PrivateCtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_INVALID_CONTEXT;
    }

    PrivateCtxPtr = CtxPtr->PrivateCtxPtr;
    if (Index >= PrivateCtxPtr->NumTables)
    {
        return XIL_VITIS_NET_P4_TARGET_ERR_TABLE_NOT_FOUND;
    }

    TargetTableCtxPtr = &(PrivateCtxPtr->TableCtxArrayPtr[Index]);
    *TableCtxPtrPtr   = &(TargetTableCtxPtr->TableCtx);
    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4TargetGetTableCount(XilVitisNetP4TargetCtx *CtxPtr, uint32_t *NumTablesPtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;

    if ((CtxPtr == NULL) || (NumTablesPtr == NULL))
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    if (CtxPtr->PrivateCtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_INVALID_CONTEXT;
    }

    *NumTablesPtr = CtxPtr->PrivateCtxPtr->NumTables;
    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4TargetGetTableName(XilVitisNetP4TargetCtx *CtxPtr,
                                              XilVitisNetP4TableCtx * TableCtxPtr,
                                              char *             TableNamePtr,
                                              uint32_t           TableNameNumBytes)
{
    XilVitisNetP4ReturnType        Result            = XIL_VITIS_NET_P4_SUCCESS;
    XilVitisNetP4TargetPrivateCtx *PrivateCtxPtr     = NULL;
    XilVitisNetP4TargetTableCtx *  TargetTableCtxPtr = NULL;
    uint32_t                  TableIndex        = 0;

    if ((CtxPtr == NULL) || (TableCtxPtr == NULL) || (TableNamePtr == NULL))
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    if (CtxPtr->PrivateCtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_INVALID_CONTEXT;
    }

    PrivateCtxPtr = CtxPtr->PrivateCtxPtr;

    /* Assumes no match by default - overriden if a match is found */
    Result = XIL_VITIS_NET_P4_TARGET_ERR_TABLE_NOT_FOUND;
    for (TableIndex = 0; TableIndex < PrivateCtxPtr->NumTables; TableIndex++)
    {
        TargetTableCtxPtr = &(PrivateCtxPtr->TableCtxArrayPtr[TableIndex]);
        if (TableCtxPtr == &(TargetTableCtxPtr->TableCtx))
        {
            Result = XIL_VITIS_NET_P4_SUCCESS;
            break;
        }
    }

    /* If a match is found, confirm the provided buffer can hold the name */
    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        if (TargetTableCtxPtr->NameSizeBytes > TableNameNumBytes)
        {
            Result = XIL_VITIS_NET_P4_TARGET_ERR_NAME_BUFFER_TOO_SMALL;
        }
    }

    /* Copy the name into the provided buffer */
    if (Result == XIL_VITIS_NET_P4_SUCCESS)
    {
        strncpy(TableNamePtr, TargetTableCtxPtr->NameStringPtr, TableNameNumBytes);
    }

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4TargetGetTableElementIdByName(XilVitisNetP4TargetCtx *CtxPtr, char *TableNamePtr, uint32_t *ElementIdPtr)
{
    XilVitisNetP4ReturnType        Result;
    XilVitisNetP4TargetPrivateCtx *PrivateCtxPtr     = NULL;
    XilVitisNetP4TargetTableCtx *  TargetTableCtxPtr = NULL;
    uint32_t                  TableIndex        = 0;
    int                       StrcmpResult      = 0;

    if ((CtxPtr == NULL) || (TableNamePtr == NULL) || (ElementIdPtr == NULL))
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    if (CtxPtr->PrivateCtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_INVALID_CONTEXT;
    }

    PrivateCtxPtr = CtxPtr->PrivateCtxPtr;

    /* Assumes no match by default - overriden if a match is found */
    Result = XIL_VITIS_NET_P4_TARGET_ERR_TABLE_NOT_FOUND;
    for (TableIndex = 0; TableIndex < PrivateCtxPtr->NumTables; TableIndex++)
    {
        TargetTableCtxPtr = &(PrivateCtxPtr->TableCtxArrayPtr[TableIndex]);
        StrcmpResult      = strcmp(TableNamePtr, TargetTableCtxPtr->NameStringPtr);
        if (StrcmpResult == 0)
        {
            /* Assumes TableIndex and ElementID are the same, which is valid until more P4 elements are supportted */
            *ElementIdPtr = TableIndex;
            Result        = XIL_VITIS_NET_P4_SUCCESS;
            break;
        }
    }

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4TargetGetBuildInfoDrv(XilVitisNetP4TargetCtx *CtxPtr, XilVitisNetP4TargetBuildInfoCtx **BuildInfoCtxPtrPtr)
{
    XilVitisNetP4ReturnType        Result = XIL_VITIS_NET_P4_SUCCESS;
    XilVitisNetP4TargetPrivateCtx *PrivateCtxPtr;

    if ((CtxPtr == NULL) || (BuildInfoCtxPtrPtr == NULL))
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    if (CtxPtr->PrivateCtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_INVALID_CONTEXT;
    }

    PrivateCtxPtr = CtxPtr->PrivateCtxPtr;

    if (PrivateCtxPtr->MgmtCtx.BuildInfoCtxPtr != NULL)
    {
        *BuildInfoCtxPtrPtr = PrivateCtxPtr->MgmtCtx.BuildInfoCtxPtr;
    }
    else
    {
        Result = XIL_VITIS_NET_P4_TARGET_ERR_MGMT_DRV_NOT_AVAILABLE;
    }

    return Result;
}

XilVitisNetP4ReturnType XilVitisNetP4TargetGetInterruptDrv(XilVitisNetP4TargetCtx *CtxPtr, XilVitisNetP4TargetInterruptCtx **InterruptCtxPtrPtr)
{
    XilVitisNetP4ReturnType        Result = XIL_VITIS_NET_P4_SUCCESS;
    XilVitisNetP4TargetPrivateCtx *PrivateCtxPtr;

    if ((CtxPtr == NULL) || (InterruptCtxPtrPtr == NULL))
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    if (CtxPtr->PrivateCtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_INVALID_CONTEXT;
    }

    PrivateCtxPtr = CtxPtr->PrivateCtxPtr;

    if (PrivateCtxPtr->MgmtCtx.InterruptCtxPtr != NULL)
    {
        *InterruptCtxPtrPtr = PrivateCtxPtr->MgmtCtx.InterruptCtxPtr;
    }
    else
    {
        Result = XIL_VITIS_NET_P4_TARGET_ERR_MGMT_DRV_NOT_AVAILABLE;
    }

    return Result;
}


XilVitisNetP4ReturnType XilVitisNetP4TargetGetCtrlDrv(XilVitisNetP4TargetCtx* CtxPtr, XilVitisNetP4TargetCtrlCtx** CtrlCtxPtrPtr)
{
    XilVitisNetP4ReturnType        Result = XIL_VITIS_NET_P4_SUCCESS;
    XilVitisNetP4TargetPrivateCtx* PrivateCtxPtr;

    if ((CtxPtr == NULL) || (CtrlCtxPtrPtr == NULL))
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_NULL_PARAM;
    }

    if (CtxPtr->PrivateCtxPtr == NULL)
    {
        return XIL_VITIS_NET_P4_GENERAL_ERR_INVALID_CONTEXT;
    }

    PrivateCtxPtr = CtxPtr->PrivateCtxPtr;

    if (PrivateCtxPtr->MgmtCtx.InterruptCtxPtr != NULL)
    {
        *CtrlCtxPtrPtr = PrivateCtxPtr->MgmtCtx.ControlCtxPtr;
    }
    else
    {
        Result = XIL_VITIS_NET_P4_TARGET_ERR_MGMT_DRV_NOT_AVAILABLE;
    }

    return Result;
}

/****************************************************************************************************************************************************/
/* SECTION: Local Function definitions */
/****************************************************************************************************************************************************/
static XilVitisNetP4ReturnType XilVitisNetP4TargetTablesInit(XilVitisNetP4TargetPrivateCtx *CtxPtr, XilVitisNetP4EnvIf *EnvIfPtr, XilVitisNetP4TargetConfig *ConfigPtr)
{
    XilVitisNetP4ReturnType         Result                 = XIL_VITIS_NET_P4_SUCCESS;
    uint32_t                   TableIndex             = 0;
    XilVitisNetP4TargetTableCtx *   TargetTableCtxArrayPtr = NULL;
    XilVitisNetP4TargetTableCtx *   TargetTableCtxPtr      = NULL;
    XilVitisNetP4TableConfig        TableConfig;
    XilVitisNetP4TargetTableConfig *TargetTableConfigPtr = NULL;

    /* Allocating array to hold table name/context pairs */
    TargetTableCtxArrayPtr = calloc(ConfigPtr->TableListSize, sizeof(XilVitisNetP4TargetTableCtx));
    if (TargetTableCtxArrayPtr == NULL)
    {
        Result = XIL_VITIS_NET_P4_TARGET_ERR_MALLOC_FAILED;
        goto fail_table_list_ctx_alloc;
    }

    /* Loop through the allocated array and set up each element */
    for (TableIndex = 0; TableIndex < ConfigPtr->TableListSize; TableIndex++)
    {
        /* Pulling currently indexed entries for convenience */
        TargetTableConfigPtr = ConfigPtr->TableListPtr[TableIndex];
        TargetTableCtxPtr    = &(TargetTableCtxArrayPtr[TableIndex]);

        /* Initialization of the table context structure - overrides the endian of the table with the global specified for the target */
        memcpy(&TableConfig, &(TargetTableConfigPtr->Config), sizeof(TableConfig));
        TableConfig.Endian = ConfigPtr->Endian;
        //Result             = XilVitisNetP4TableInit(&(TargetTableCtxPtr->TableCtx), EnvIfPtr, &TableConfig);
        Result             = XilVitisNetP4TableInit(((XilVitisNetP4TableCtx*)&(((XilVitisNetP4TargetTableCtx*)&(TargetTableCtxArrayPtr[TableIndex]))->TableCtx)), EnvIfPtr, &TableConfig);
        if (Result != XIL_VITIS_NET_P4_SUCCESS)
        {
            goto fail_table_init;
        }

        /* Storing a copy of the name associated with the context initialized above */
        TargetTableCtxPtr->NameSizeBytes = strlen(TargetTableConfigPtr->NameStringPtr) + 1;
        //TargetTableCtxPtr->NameStringPtr = calloc(1, TargetTableCtxPtr->NameSizeBytes);
        ((XilVitisNetP4TargetTableCtx*) &(TargetTableCtxArrayPtr[TableIndex]))->NameStringPtr = calloc(1, TargetTableCtxPtr->NameSizeBytes);
        if (TargetTableCtxPtr->NameStringPtr == NULL)
        {
            /* Initialize will allocate memory, so exit to release that memory */
            XilVitisNetP4TableExit(&(TargetTableCtxPtr->TableCtx));
            goto fail_table_init;
        }
        strncpy(TargetTableCtxPtr->NameStringPtr, TargetTableConfigPtr->NameStringPtr, TargetTableCtxPtr->NameSizeBytes);
    }

    /* All memory allocated successfully - complete structure connections */
    CtxPtr->TableCtxArrayPtr = TargetTableCtxArrayPtr;
    CtxPtr->NumTables        = ConfigPtr->TableListSize;

    return Result;

fail_table_init:
    if (TableIndex > 0)
    {
        while (TableIndex-- != 0)
        {
            TargetTableCtxPtr = &(TargetTableCtxArrayPtr[TableIndex]);
            /* This is a best attempt exit, error can still happen for low level driver exits*/
            XilVitisNetP4TableExit(&(TargetTableCtxPtr->TableCtx));
            free(TargetTableCtxPtr->NameStringPtr);
        }
    }
    free(TargetTableCtxArrayPtr);
fail_table_list_ctx_alloc:
    return Result;
}

static XilVitisNetP4ReturnType XilVitisNetP4TargetTablesExit(XilVitisNetP4TargetPrivateCtx *CtxPtr)
{
    XilVitisNetP4ReturnType      Result = XIL_VITIS_NET_P4_SUCCESS;
    XilVitisNetP4ReturnType      TableExitResult   = XIL_VITIS_NET_P4_SUCCESS;
    XilVitisNetP4TargetTableCtx *TargetTableCtxPtr = NULL;
    uint32_t                TableIndex        = 0;

    for (TableIndex = 0; TableIndex < CtxPtr->NumTables; TableIndex++)
    {
        TargetTableCtxPtr = &(CtxPtr->TableCtxArrayPtr[TableIndex]);
        TableExitResult   = XilVitisNetP4TableExit(&(TargetTableCtxPtr->TableCtx));
        /* Report the last error if any of the target tables exit but attempt to free all*/
        if (TableExitResult != XIL_VITIS_NET_P4_SUCCESS)
        {
            Result = TableExitResult;
        }
        free(TargetTableCtxPtr->NameStringPtr);
    }

    free(CtxPtr->TableCtxArrayPtr);

    return Result;
}

static XilVitisNetP4ReturnType XilVitisNetP4TargetMgmtInit(XilVitisNetP4TargetMgmtCtx *CtxPtr, XilVitisNetP4EnvIf *EnvIfPtr, XilVitisNetP4TargetConfig *ConfigPtr)
{
    XilVitisNetP4ReturnType          Result          = XIL_VITIS_NET_P4_SUCCESS;
    XilVitisNetP4TargetBuildInfoCtx *BuildInfoCtxPtr = NULL;
    XilVitisNetP4TargetInterruptCtx *InterruptCtxPtr = NULL;
    XilVitisNetP4TargetCtrlCtx      *ControlCtxPtr   = NULL;

    if (ConfigPtr->BuildInfoPtr != NULL)
    {
        BuildInfoCtxPtr = calloc(1, sizeof(XilVitisNetP4TargetBuildInfoCtx));
        if (BuildInfoCtxPtr == NULL)
        {
            Result = XIL_VITIS_NET_P4_TARGET_ERR_MALLOC_FAILED;
            goto fail_alloc_build_info;
        }

        Result = XilVitisNetP4TargetBuildInfoInit(BuildInfoCtxPtr, EnvIfPtr, ConfigPtr->BuildInfoPtr);
        if (Result != XIL_VITIS_NET_P4_SUCCESS)
        {
            goto fail_init_build_info;
        }
    }
    else
    {
        CtxPtr->BuildInfoCtxPtr = NULL;
    }

    if (ConfigPtr->InterruptPtr != NULL)
    {
        InterruptCtxPtr = calloc(1, sizeof(XilVitisNetP4TargetInterruptCtx));
        if (InterruptCtxPtr == NULL)
        {
            Result = XIL_VITIS_NET_P4_TARGET_ERR_MALLOC_FAILED;
            goto fail_init_interrupt;
        }

        Result = XilVitisNetP4TargetInterruptInit(InterruptCtxPtr, EnvIfPtr, ConfigPtr->InterruptPtr);
        if (Result != XIL_VITIS_NET_P4_SUCCESS)
        {
            goto fail_init_interrupt;
        }
    }
    else
    {
        CtxPtr->InterruptCtxPtr = NULL;
    }

    if (ConfigPtr->CtrlConfigPtr != NULL)
    {
        ControlCtxPtr = calloc(1, sizeof(XilVitisNetP4TargetCtrlCtx));
        if (ControlCtxPtr == NULL)
        {
            Result = XIL_VITIS_NET_P4_TARGET_ERR_MALLOC_FAILED;
            goto fail_init_interrupt;
        }

        Result = XilVitisNetP4TargetCtrlInit(ControlCtxPtr, EnvIfPtr, ConfigPtr->CtrlConfigPtr);
    }
    else
    {
        CtxPtr->ControlCtxPtr = NULL;
    }

    CtxPtr->BuildInfoCtxPtr = BuildInfoCtxPtr;
    CtxPtr->InterruptCtxPtr = InterruptCtxPtr;
    CtxPtr->ControlCtxPtr = ControlCtxPtr;

    return Result;

fail_init_interrupt:
    if (InterruptCtxPtr != NULL)
    {
        free(InterruptCtxPtr->PrivateCtxPtr);
    }
    free(InterruptCtxPtr);
    free(ControlCtxPtr);
    if (BuildInfoCtxPtr != NULL)
    {
        XilVitisNetP4TargetBuildInfoExit(BuildInfoCtxPtr);
    }

fail_init_build_info:
    free(BuildInfoCtxPtr);
fail_alloc_build_info:
    return Result;
}

static XilVitisNetP4ReturnType XilVitisNetP4TargetMgmtExit(XilVitisNetP4TargetMgmtCtx *CtxPtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;

    if (CtxPtr->BuildInfoCtxPtr != NULL)
    {
        XilVitisNetP4TargetBuildInfoExit(CtxPtr->BuildInfoCtxPtr);
        free(CtxPtr->BuildInfoCtxPtr);
        CtxPtr->BuildInfoCtxPtr = NULL;
    }

    if (CtxPtr->InterruptCtxPtr != NULL)
    {
        XilVitisNetP4TargetInterruptExit(CtxPtr->InterruptCtxPtr);
        free(CtxPtr->InterruptCtxPtr);
        CtxPtr->InterruptCtxPtr = NULL;
    }

    return Result;
}

static XilVitisNetP4ReturnType XilVitisNetP4TargetCheckCompatibleHwVersion(XilVitisNetP4TargetBuildInfoCtx *BuildInfoCtxPtr)
{
    XilVitisNetP4ReturnType Result = XIL_VITIS_NET_P4_SUCCESS;
    XilVitisNetP4Version IpVersion;

    /* If driver available get the IP Version */
    if (BuildInfoCtxPtr != NULL)
    {
        Result = XilVitisNetP4TargetBuildInfoGetIpVersion(BuildInfoCtxPtr, &IpVersion);
        if (Result != XIL_VITIS_NET_P4_SUCCESS)
        {
            return Result;
        }

        if (Result == XIL_VITIS_NET_P4_SUCCESS)
        {
            /* The software and hardware versions are compatible if the Major and Minor numbers are equal */
            if ((IpVersion.Major != XIL_VITIS_NET_P4_TARGET_SW_VERSION_MAJOR_NUM) || (IpVersion.Minor != XIL_VITIS_NET_P4_TARGET_SW_VERSION_MINOR_NUM))
            {
                Result = XIL_VITIS_NET_P4_TARGET_ERR_INCOMPATIBLE_SW_HW;
            }
        }
    }

    return Result;
}