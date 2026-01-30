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
-- Revision       : $Revision: #1 $
-- Date           : $DateTime: 2021/03/10 09:21:01 $
-- Last Author    : $Author: jaimeh $
--
--------------------------------------------------------------------------------
-- Description : Header file for the VitisNetP4Target driver
--
--------------------------------------------------------------------------------
*/

/**
 * Public API of the VitisNetP4Target driver.
 *
 * @file vitisnetp4_target.h
 * @addtogroup target VitisNetP4target public API
 * @{
 * @details
 *
 * \section Introduction
 *
 * The target driver is the top level of theVitisNetP4control plane management
 * stack.  It implements two key functions designed to improve ease of use for
 * application development:
 *  - It provides functions to setup and teardown all control plane components
 *    in anVitisNetP4design
 *  - It provides an API which resolves the names of control plane components
 *    to an associated context structure, for use with the corresponding control plane driver
 *
 * To perform this functionality, the target driver consumes a configuration
 * structure that is automatically generated whenVitisNetP4is run. This
 * configuration structure describes all control plane manageable components
 * present in theVitisNetP4design.
 *
 * The control plane components can be either VitisNetP4 directly managed P4 elements or
 * IP specific elements
 *
 * The VitisNetP4 P4 elements available  inVitisNetP4are:
 *  - Tables
 *
 * The IP specific elements inVitisNetP4are:
 *  - Build information reader
 *  - Interrupt controller
 *
 *
 * \section overview_op Overview of Operation
 *
 * The driver provides the following functions for setup and teardown
 * respectively:
 *  - @ref XilVitisNetP4TargetInit()
 *  - @ref XilVitisNetP4TargetExit()
 *
 * Upon successful execution of @ref XilVitisNetP4TargetInit(), the application is
 * free to call any of the remaining functions.  The primary usecase is to
 * fetch a context structure for a given control plane management component by
 * specifying its name.  In the case of a table, the user would call
 * @ref XilVitisNetP4TargetGetTableByName(), which upon successful execution provides
 * the caller with a pointer to an @ref XilVitisNetP4TableCtx structure, which may
 * then be used with the functions offered by the @ref table driver.
 */

#ifndef VITISNETP4_TARGET_H
#define VITISNETP4_TARGET_H

/****************************************************************************************************************************************************/
/* SECTION: Header includes */
/****************************************************************************************************************************************************/
#include <stdbool.h>

#include "vitisnetp4_table.h"
#include "vitisnetp4_target_mgmt.h"

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************************************************************************************/
/* SECTION: Type definitions */
/****************************************************************************************************************************************************/

/** Wrapper structure to group table name with table configuration */
typedef struct XilVitisNetP4TargetTableConfig
{
    const char               *NameStringPtr; /**< Table control plane name */
    XilVitisNetP4TableConfig Config;         /**< Table configuration */
} XilVitisNetP4TargetTableConfig;


/** Top level configuration data structure describing all control plane manageable elements in anVitisNetP4design */
typedef struct XilVitisNetP4TargetConfig
{
    /****************************************************************************************************************************************************/
    /* Managed P4 Elements Configurations*/
    /****************************************************************************************************************************************************/
    XilVitisNetP4Endian              Endian;         /**< Global endianness setting - applies to all P4 element drivers instantiated by the target */
    uint32_t                         TableListSize;  /**< Number of tables present in the design */
    XilVitisNetP4TargetTableConfig   **TableListPtr; /**< Pointer to list holding the configuration of each table present */

    /****************************************************************************************************************************************************/
    /* IP specific elements Configurations*/
    /****************************************************************************************************************************************************/
    XilVitisNetP4TargetBuildInfoConfig   *BuildInfoPtr;      /**< Pointer to the configuration for the build information reader*/
    XilVitisNetP4TargetInterruptConfig   *InterruptPtr;      /**< Pointer to the configuration for the interrupt manager*/
    XilVitisNetP4TargetCtrlConfig        *CtrlConfigPtr;     /**< Pointer to the configuration for the packet control manager */
} XilVitisNetP4TargetConfig;

/** Forward declaration to support context structure declaration */
typedef struct XilVitisNetP4TargetPrivateCtx XilVitisNetP4TargetPrivateCtx;

/** Holds context information needed by the target driver's API */
typedef struct XilVitisNetP4TargetCtx
{
    XilVitisNetP4TargetPrivateCtx *PrivateCtxPtr;    /**< Internal context data used by driver implementation */
} XilVitisNetP4TargetCtx;

/****************************************************************************************************************************************************/
/* SECTION: Target function prototypes */
/****************************************************************************************************************************************************/

/**
 * Initialization function for the VitisNetP4Target driver.
 *
 * @param[in,out] CtxPtr        Pointer to an uninitializedVitisNetP4Target context structure instance.
 *                              Is populated by function execution.
 *
 * @param[in] EnvIfPtr          Pointer to environment interface definition
 *
 * @param[in] ConfigPtr         Pointer to driver configuration definition
 *
 * @return XIL_VITIS_NET_P4_SUCCESS if successful, otherwise an error code.
 */

XilVitisNetP4ReturnType XilVitisNetP4TargetInit(XilVitisNetP4TargetCtx *CtxPtr, XilVitisNetP4EnvIf *EnvIfPtr, XilVitisNetP4TargetConfig *ConfigPtr);

/**
 * Destroy the VitisNetP4Target driver instance.
 *
 * @param[in] CtxPtr            Pointer to the VitisNetP4Target instance context.
 *
 * @return XIL_VITIS_NET_P4_SUCCESS if successful, otherwise an error code.
 */

XilVitisNetP4ReturnType XilVitisNetP4TargetExit(XilVitisNetP4TargetCtx *CtxPtr);

/**
 * Get the Software version
 *
 * @param[in] CtxPtr            Pointer to the VitisNetP4Target instance context.
 *
 * @param[out] SwVersionPtr     Pointer to the structure that holds the software version values
 *
 * @return XIL_VITIS_NET_P4_SUCCESS if successful, otherwise an error code.
 */
XilVitisNetP4ReturnType XilVitisNetP4TargetGetSwVersion(XilVitisNetP4TargetCtx *CtxPtr, XilVitisNetP4Version *SwVersionPtr);

/****************************************************************************************************************************************************/
/* SECTION: Table helper function prototypes */
/****************************************************************************************************************************************************/

/**
 * @addtogroup target_hlp VitisNetP4target: table helper API
 * @{
 * A collection of helper functions for managing the tables present in the target.
 */

/**
 * Gets the context structure for the named table, if present in the target.
 *
 * @param[in] CtxPtr            Pointer to the VitisNetP4Target instance context.
 *
 * @param[in] TableNamePtr      Pointer to string holding table name.
 *
 * @param[out] TableCtxPtrPtr   Pointer to a context structure for the table instance.
 *
 * @return XIL_VITIS_NET_P4_SUCCESS if successful, otherwise an error code.
 */

XilVitisNetP4ReturnType XilVitisNetP4TargetGetTableByName(XilVitisNetP4TargetCtx *CtxPtr, char *TableNamePtr, XilVitisNetP4TableCtx **TableCtxPtrPtr);

/**
 * Gets the context structure for the indexed table, if present in the target.
 *
 * @param[in] CtxPtr            Pointer to the VitisNetP4Target instance context.
 *
 * @param[in] Index             Integer specifying the table instance to be retrieved.
 *
 * @param[out] TableCtxPtrPtr   Pointer to a context structure for the table instance.
 *
 * @return XIL_VITIS_NET_P4_SUCCESS if successful, otherwise an error code.
 */

XilVitisNetP4ReturnType XilVitisNetP4TargetGetTableByIndex(XilVitisNetP4TargetCtx *CtxPtr, uint32_t Index, XilVitisNetP4TableCtx **TableCtxPtrPtr);

/**
 * Gets the number of tables present in the target.
 *
 * @param[in] CtxPtr            Pointer to the VitisNetP4Target instance context.
 *
 * @param[out] NumTablesPtr     The number of tables present in the target.
 *
 * @return XIL_VITIS_NET_P4_SUCCESS if successful, otherwise an error code.
 */

XilVitisNetP4ReturnType XilVitisNetP4TargetGetTableCount(XilVitisNetP4TargetCtx *CtxPtr, uint32_t *NumTablesPtr);

/**
 * Gets the name of the table from its context structure
 *
 * @param[in] CtxPtr                Pointer to the VitisNetP4Target instance context.
 *
 * @param[in] TableCtxPtr           Pointer to a table's context structure.
 *
 * @param[out] TableNamePtr         Pointer to char buffer to hold the table name.
 *
 * @param[in] TableNameNumBytes     The number of characters the table name buffer can hold including the null terminating character.
 *
 * @return XIL_VITIS_NET_P4_SUCCESS if successful, otherwise an error code.
 */

XilVitisNetP4ReturnType XilVitisNetP4TargetGetTableName(XilVitisNetP4TargetCtx *CtxPtr,
                                              XilVitisNetP4TableCtx *TableCtxPtr,
                                              char *TableNamePtr,
                                              uint32_t TableNameNumBytes);

/**
 * Gets the element identifier for the named table, if present in the target.
 *
 * @param[in] CtxPtr                Pointer to the VitisNetP4Target instance context.
 *
 * @param[in] TableNamePtr          Pointer to char buffer that holds the table name.
 *
 * @param[out] ElementIdPtr         Pointer to integer that stores the table identifier.
 *
 * @return XIL_VITIS_NET_P4_SUCCESS if successful, otherwise an error code
 */
XilVitisNetP4ReturnType XilVitisNetP4TargetGetTableElementIdByName(XilVitisNetP4TargetCtx *CtxPtr, char *TableNamePtr, uint32_t *ElementIdPtr);


/****************************************************************************************************************************************************/
/* SECTION:VitisNetP4Instance Management helper function prototypes */
/****************************************************************************************************************************************************/


/**
 * Gets the context structure for the build information driver.
 *
 * If the build information driver is not present, the function fails by returning error code XIL_VITIS_NET_P4_TARGET_ERR_MGMT_DRV_NOT_AVAILABLE.
 *
 * @param[in] CtxPtr                Pointer to the VitisNetP4Target instance context.
 *
 * @param[out] BuildInfoCtxPtrPtr    Pointer to a context structure for the build information driver instance.
 *
 * @return XIL_VITIS_NET_P4_SUCCESS if successful, otherwise an error code.
 */
XilVitisNetP4ReturnType XilVitisNetP4TargetGetBuildInfoDrv(XilVitisNetP4TargetCtx *CtxPtr, XilVitisNetP4TargetBuildInfoCtx **BuildInfoCtxPtrPtr);

/**
 * Gets the context structure for the management interrupt driver, if present in the target.
 *
 * If the interrupt driver is not present, the function fails by returning error code XIL_VITIS_NET_P4_TARGET_ERR_MGMT_DRV_NOT_AVAILABLE.
 *
 * @param[in] CtxPtr                Pointer to the VitisNetP4Target instance context.
 *
 * @param[out] InterruptCtxPtrPtr   Pointer to a context structure for the management interrupt driver instance.
 *
 * @return XIL_VITIS_NET_P4_SUCCESS if successful, otherwise an error code.
 */
XilVitisNetP4ReturnType XilVitisNetP4TargetGetInterruptDrv(XilVitisNetP4TargetCtx *CtxPtr, XilVitisNetP4TargetInterruptCtx **InterruptCntlrCtxPtrPtr);

/**
 * Gets the context structure for the management control driver, if present in the target.
 *
 * If the interrupt driver is not present, the function fails by returning error code XIL_VITIS_NET_P4_TARGET_ERR_MGMT_DRV_NOT_AVAILABLE.
 *
 * @param[in] CtxPtr                Pointer to the VitisNetP4Target instance context.
 *
 * @param[out] InterruptCtxPtrPtr   Pointer to a context structure for the management interrupt driver instance.
 *
 * @return XIL_VITIS_NET_P4_SUCCESS if successful, otherwise an error code.
 */
XilVitisNetP4ReturnType XilVitisNetP4TargetGetCtrlDrv(XilVitisNetP4TargetCtx* CtxPtr, XilVitisNetP4TargetCtrlCtx** CtrlCtxPtrPtr);


/** @} */

#ifdef __cplusplus
}
#endif

#endif

/** @} */
