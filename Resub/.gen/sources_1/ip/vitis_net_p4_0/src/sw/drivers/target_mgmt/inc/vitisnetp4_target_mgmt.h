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
-- Description : Header file for the VitisNetP4target management drivers
--
--
--------------------------------------------------------------------------------
*/
/**
 * Public API for a set of drivers that provide access to VitisNetP4 IP specific elements
 *
 * @file
 * @addtogroup vitisp4net_ip_specific_elements
 * @{
 * @details
 *
 * \section Introduction
 *
 * TheVitisNetP4target management is a collection of IP specific drivers that
 * - collect IP Build Information include the IP Version
 * - control the triggering ECC Error Interrupts to inform user when ECC Error
 *   information is available
 *
 * The build information driver provides the following functions to retrieve
 * build time information:
 *  - @ref XilVitisNetP4TargetBuildInfoInit()
 *  - @ref XilVitisNetP4TargetBuildInfoGetIpVersion()
 *  - @ref XilVitisNetP4TargetBuildInfoGetIpSettings()
 *  - @ref XilVitisNetP4TargetBuildInfoExit()
 *
 * The interrupt driver provides function to control the interrupts for single
 * or double bit ECC error detection for VitisNetP4target components. The components
 * that support ECC error detection are as follows:
 *  - P4 elements supported by VitisNetP4, see @ref target for further details
 *  - Internal Component's used in the VitisNetP4target
 *
 * The drivers interfaces provided provide the capability to enable, disable and
 * read plus clear the interrupt status for an individual element or Component.
 *
 * The P4 Element interrupt related functions accept a parameter called P4ElementId.
 * @ref target driver provides APIs that accepts a string containing the name of the
 * P4 element retrieves the corresponding ID.
 *
 * Note: Some configuration , e.g. type of memory, used for P4 elements result
 *       in P4 elements that do not support ECC error interrupts. For these cases
 *       driver will indicate the successful enabli ng of interrupts but no interrupts
 *       will be generated for these elements.
 *
 */

#ifndef VITISNETP4_TARGET_MGMT_H
#define VITISNETP4_TARGET_MGMT_H

/****************************************************************************************************************************************************/
/* SECTION: Header includes */
/****************************************************************************************************************************************************/
#include <stdbool.h>
#include "vitisnetp4_common.h"

#ifdef __cplusplus
extern "C" {
#endif


/****************************************************************************************************************************************************/
/* SECTION: Constants */
/****************************************************************************************************************************************************/
typedef enum XilVitisNetP4TargetInterruptEccErrorType
{
    XIL_VITIS_NET_P4_INTERRUPT_ECC_ERROR_SINGLE_BIT,
    XIL_VITIS_NET_P4_INTERRUPT_ECC_ERROR_DOUBLE_BIT,
    XIL_VITIS_NET_P4_INTERRUPT_ECC_ERROR_ALL,
    XIL_VITIS_NET_P4_INTERRUPT_ECC_ERROR_TYPE_MAX  /**< For validation by driver - do not use */
} XilVitisNetP4TargetInterruptEccErrorType;

/****************************************************************************************************************************************************/
/* SECTION: Type definitions  */
/****************************************************************************************************************************************************/

typedef enum XilVitisNetP4TargetCtrlEngineId
{
    XIL_VITIS_NET_P4_TARGET_CTRL_ALL_ENGINES,
    XIL_VITIS_NET_P4_TARGET_CTRL_DEPARSER_ENGINE,
    XIL_VITIS_NET_P4_TARGET_CTRL_FIFO_ENGINE,
    XIL_VITIS_NET_P4_TARGET_CTRL_MA_ENGINE,
    XIL_VITIS_NET_P4_TARGET_CTRL_PARSER_ENGINE,
} XilVitisNetP4TargetCtrlEngineId;

/*
 * Context typedef structures
 */

/** Forward declaration to support context structure declaration */
typedef struct XilVitisNetP4TargetBuildInfoPrivateCtx XilVitisNetP4TargetBuildInfoPrivateCtx;

/** Holds context information needed by the VitisNetP4 instance information driver's API */
typedef struct XilVitisNetP4TargetBuildInfoCtx
{
    XilVitisNetP4TargetBuildInfoPrivateCtx *PrivateCtxPtr;  /**< Internal context data used by driver implementation */
} XilVitisNetP4TargetBuildInfoCtx;

/** Forward declaration to support context structure declaration */
typedef struct XilVitisNetP4TargetInterruptPrivateCtx XilVitisNetP4TargetInterruptPrivateCtx;

/** Holds context information needed by the VitisNetP4 instance information driver's API */
typedef struct XilVitisNetP4TargetInterruptCtx
{
    XilVitisNetP4TargetInterruptPrivateCtx *PrivateCtxPtr;  /**< Internal context data used by driver implementation */
} XilVitisNetP4TargetInterruptCtx;

/*
 * Configuration typedef structures
 */

/** Configuration that describes the block information of the VitisNetP4 instance */
typedef struct XilVitisNetP4TargetBuildInfoConfig
{
    XilVitisNetP4AddressType BaseAddr;   /**< Base address within the VitisNetP4 instance */
} XilVitisNetP4TargetBuildInfoConfig;


typedef struct XilVitisNetP4ComponentName
{
    char *NameStringPtr;
} XilVitisNetP4ComponentName;

/** Configuration that describes the interrupt controller of the VitisNetP4 instance  */
typedef struct XilVitisNetP4TargetInterruptConfig
{
    XilVitisNetP4AddressType      BaseAddr;           /**< Base address within the VitisNetP4 instance */
    uint32_t                 NumP4Elements;        /**< Number of P4 Elements present in the VitisNetP4 instance */
    uint32_t                 NumComponents;           /**< Number of internal Components present in the VitisNetP4 instance */
    XilVitisNetP4ComponentName    ComponentNameList[];     /**< List of Components name that are present in the VitisNetP4 instance */
} XilVitisNetP4TargetInterruptConfig;

/*
 * typedefs for API's
 */

typedef struct XilVitisNetP4TargetBuildInfoIpSettings
{
    uint32_t PacketRateMps;      /**< Packet rate in Mp/s*/
    uint32_t CamMemoryClockMhz;  /**< CAM memory clock in MHz*/
    uint32_t AxiStreamClockMhz;   /**< AXI Stream clock in MHz*/
} XilVitisNetP4TargetBuildInfoIpSettings;

typedef struct XilVitisNetP4TargetInterruptEccErrorStatus
{
    bool SingleBitErrorStatus; /**< */
    bool DoubleBitErrorStatus; /**< */
} XilVitisNetP4TargetInterruptEccErrorStatus;

/*** Control driver definitions****/

/** Forward declaration to support context structure declaration */
typedef struct XilVitisNetP4TargetCtrlPrivateCtx XilVitisNetP4TargetCtrlPrivateCtx;

/** Holds context information needed by the VitisNetP4 instance information driver's API */
typedef struct XilVitisNetP4TargetCtrlCtx
{
    XilVitisNetP4TargetCtrlPrivateCtx* PrivateCtxPtr;  /**< Internal context data used by driver implementation */
} XilVitisNetP4TargetCtrlCtx;

/*
 * Configuration typedef structures
 */

/** Configuration that describes the interrupt controller of the VitisNetP4 instance  */
typedef struct XilVitisNetP4TargetCtrlConfig
{
    XilVitisNetP4AddressType BaseAddr;           /**< Base address within the VitisNetP4 instance */
    uint32_t            NumP4Elements;        /**< Number of P4 Elements present in the VitisNetP4 instance */
    uint32_t            NumComponents;           /**< Number of internal FIFOs present in the VitisNetP4 instance */
    uint32_t            ClkInHz;            /* Clock in Hz */
    uint32_t            PktRatePerSec;      /* Packet rate in packets per second */
} XilVitisNetP4TargetCtrlConfig;

/****************************************************************************************************************************************************/
/* SECTION: Global Function prototypes */
/****************************************************************************************************************************************************/

/**
 * @addtogroup vitisp4net_build_info VitisNetP4Build Information driver public API
 * @{
 */

/**
 * Initialization function for the Build Information driver API
 *
 * @param[in] CtxPtr            Pointer to an uninitializedVitisNetP4 instance information context structure instance.
 *                              Is populated by function execution.
 *
 * @param[in] EnvIfPtr          Pointer to environment interface definition
 *
 * @param[in] ConfigPtr         Pointer to driver configuration definition
 *
 * @return XIL_VITIS_NET_P4_SUCCESS if successful, otherwise an error code
 */
XilVitisNetP4ReturnType XilVitisNetP4TargetBuildInfoInit(XilVitisNetP4TargetBuildInfoCtx *CtxPtr, XilVitisNetP4EnvIf *EnvIfPtr, XilVitisNetP4TargetBuildInfoConfig *ConfigPtr);

/**
 * Get the IP Version for the VitisNetP4 instance
 *
 * @param[in]  CtxPtr           Pointer to the VitisNetP4 instance information context.
 *
 * @param[out] IpVersionPtr     Pointer to a structure that stores the IP version.
 *
 * @return XIL_VITIS_NET_P4_SUCCESS if successful, otherwise an error code.
 */
XilVitisNetP4ReturnType XilVitisNetP4TargetBuildInfoGetIpVersion(XilVitisNetP4TargetBuildInfoCtx *CtxPtr, XilVitisNetP4Version *IpVersionPtr);

/**
 * Get the setting parameters for the VitisNetP4 instance
 *
 * @param[in]  CtxPtr           Pointer to the VitisNetP4 instance information context.
 *
 * @param[out] SettingsPtr    Pointer to a structure that stores the setting parameters.
 *
 * @return XIL_VITIS_NET_P4_SUCCESS if successful, otherwise an error code.
 */
XilVitisNetP4ReturnType XilVitisNetP4TargetBuildInfoGetIpSettings(XilVitisNetP4TargetBuildInfoCtx *CtxPtr, XilVitisNetP4TargetBuildInfoIpSettings *IpSettingsPtr);

/**
 * Destroy the Build Information instance
 *
 * All memory allocated by \ref XilVitisNetP4TargetBuildInfoInit is released.
 *
 * @param[in]  CtxPtr           Pointer to the VitisNetP4 instance information context.
 *
 * @return XIL_VITIS_NET_P4_SUCCESS if successful, otherwise an error code.
 */
XilVitisNetP4ReturnType XilVitisNetP4TargetBuildInfoExit(XilVitisNetP4TargetBuildInfoCtx *CtxPtr);

/** @} */

/**
 * @addtogroup vitisp4net_interrupt VitisNetP4Interrupt Driver public API
 * @{
    Manages the triggering of ECC Interrupts for P4 Elements and internal Components present in the VitisNetP4 instance.
 */

/**
 * Initialization function for the Interrupt driver API
 *
 * @param[in] CtxPtr            Pointer to an uninitialized interrupt controller context structure instance.
 *                              Is populated by function execution.
 *
 * @param[in] EnvIfPtr          Pointer to environment interface definition.
 *
 * @param[in] ConfigPtr         Pointer to driver configuration definition.
 *
 * @return XIL_VITIS_NET_P4_SUCCESS if successful, otherwise an error code.
 */
XilVitisNetP4ReturnType XilVitisNetP4TargetInterruptInit(XilVitisNetP4TargetInterruptCtx *CtxPtr, XilVitisNetP4EnvIf *EnvIfPtr, XilVitisNetP4TargetInterruptConfig *ConfigPtr);

/**
 * Gets the Number of P4 elements that have ECC interrupt controls
 *
 * @param[in]  CtxPtr           Pointer to the interrupt controller context.
 *
 * @param[out] NumP4ElementsPtr   Pointer to variable used to store the number of elements value.
 *
 * @return XIL_VITIS_NET_P4_SUCCESS if successful, otherwise an error code.
 */
XilVitisNetP4ReturnType XilVitisNetP4TargetInterruptGetP4ElementCount(XilVitisNetP4TargetInterruptCtx *CtxPtr, uint32_t *NumP4ElementsPtr);

/**
 * Gets the number of internal Components that have ECC interrupt controls
 *
 * @param[in]  CtxPtr           Pointer to the interrupt controller context.
 *
 * @param[out] NumFifosPtr      Pointer to variable used to store the number of Components.
 *
 * @return XIL_VITIS_NET_P4_SUCCESS if successful, otherwise an error code.
 */
XilVitisNetP4ReturnType XilVitisNetP4TargetInterruptGetComponentCount(XilVitisNetP4TargetInterruptCtx *CtxPtr, uint32_t * NumComponentPtr);

/**
 * Gets the index for the named Component, if present
 *
 * If the Component is not present, the function fails by returning error code XIL_VITIS_NET_P4_INTERRUPT_ERR_COMPONENT_NOT_FOUND.
 *
 * @param[in] CtxPtr            Pointer to the interrupt controller context.
 *
 * @param[in] ComponentNamePtr       Pointer to char buffer that holds the Component name.
 *
 * @param[out] IndexPtr         Pointer to variable used to store the number of Components.
 *
 * @return XIL_VITIS_NET_P4_SUCCESS if successful, otherwise an error code.
 */
XilVitisNetP4ReturnType XilVitisNetP4TargetInterruptGetComponentIndexByName(XilVitisNetP4TargetInterruptCtx *CtxPtr, char * ComponentNamePtr, uint32_t *IndexPtr);

/**
 * Get the Component name for a specific index, supports iterative liner search for all Component names present
 *
 * If the entry is not found, the function fails with error code XIL_VITIS_NET_P4_INTERRUPT_ERR_COMPONENT_NOT_FOUND.
 *
 * The IndexPtr argument enables iterative search for Component names. The first time the function is
 * called the integer at IndexPtr should be set to zero. To find all Component names, the function is
 * called multiple times, each call returns a single Component name.
 *
 * For every consecutive call the returned IndexPtr value must be used again as input argument.
 * When XIL_VITIS_NET_P4_INTERRUPT_ERR_COMPONENT_NOT_FOUND is returned no more FIFOs are present.
 *
 * @param[in] CtxPtr            Pointer to the interrupt controller context.
 *
 * @param[in,out] IndexPtr      Index of the Component. Used for consecutive get operations, set to 0 for
 *                              first get.
 *
 * @param[out] ComponentNamePtr      Pointer to char buffer to hold the table name.
 *
 * @param[in] ComponentNameNumBytes  The number of characters the Component name buffer can hold including the null terminating character.
 *
 * @return XIL_VITIS_NET_P4_SUCCESS if successful, otherwise an error code.
 */
XilVitisNetP4ReturnType XilVitisNetP4TargetInterruptGetComponentNameByIndex(XilVitisNetP4TargetInterruptCtx *CtxPtr, uint32_t *IndexPtr, char *ComponentNamePtr, uint32_t ComponentNameNumBytes);

/**
 * Enable ECC Error interrupts for a specified P4 element
 *
 * If the element identifier does not exist, the function fails by returning error code XIL_VITIS_NET_P4_INTERRUPT_ERR_INVALID_ELEMENT_ID.
 *
 * @param[in] CtxPtr            Pointer to the interrupt controller context.
 *
 * @param[in] P4ElementId         Stores the P4 element identifier.
 *
 * @param[in] EccErrorType      Selects which ECC interrupts are enabled for this element.
 *
 * @return XIL_VITIS_NET_P4_SUCCESS if successful, otherwise an error code.
 */
XilVitisNetP4ReturnType XilVitisNetP4TargetInterruptEnableP4ElementEccErrorById(XilVitisNetP4TargetInterruptCtx *CtxPtr, uint32_t ElementId, XilVitisNetP4TargetInterruptEccErrorType EccErrorType);

/**
 * Disable ECC Error interrupts for a specified P4 element
 *
 * If the element identifier does not exist, the function fails by returning error code XIL_VITIS_NET_P4_INTERRUPT_ERR_INVALID_ELEMENT_ID.
 *
 * @param[in]   CtxPtr           Pointer to the interrupt controller context.
 *
 * @param[in]   P4ElementId       Stores the P4 element identifier.
 *
 * @param[in] EccErrorType      Selects which ECC interrupts are disabled for this element.
 *
 * @return XIL_VITIS_NET_P4_SUCCESS if successful, otherwise an error code.
 */
XilVitisNetP4ReturnType XilVitisNetP4TargetInterruptDisableP4ElementEccErrorById(XilVitisNetP4TargetInterruptCtx *CtxPtr, uint32_t ElementId, XilVitisNetP4TargetInterruptEccErrorType EccErrorType);

/**
 * Get the ECC Error interrupt status for a specified P4 element
 *
 * If the element identifier does not exist, the function fails by returning error code XIL_VITIS_NET_P4_INTERRUPT_ERR_INVALID_ELEMENT_ID.
 *
 * @param[in]   CtxPtr          Pointer to the interrupt controller context.
 *
 * @param[in]   P4ElementId       Stores the P4 element identifier.
 *
 * @param[out]  StatusPtr       Pointer to structure that stores the ECC interrupt status.
 *
 * @return XIL_VITIS_NET_P4_SUCCESS if successful, otherwise an error code.
 */
XilVitisNetP4ReturnType XilVitisNetP4TargetInterruptGetP4ElementEccErrorStatusById(XilVitisNetP4TargetInterruptCtx *CtxPtr, uint32_t ElementId, XilVitisNetP4TargetInterruptEccErrorStatus *StatusPtr);

/**
 * Clear the ECC Error interrupt status for a specified P4 element
 *
 * If the element identifier does not exist, the function fails by returning error code XIL_VITIS_NET_P4_INTERRUPT_ERR_INVALID_ELEMENT_ID.
 *
 * @param[in]  CtxPtr           Pointer to the interrupt controller context.
 *
 * @param[in] P4ElementId         Stores the P4 element identifier.
 *
 * @param[in] EccErrorType      Selects which ECC interrupts are cleared for this element.
 *
 * @return XIL_VITIS_NET_P4_SUCCESS if successful, otherwise an error code.
 */
XilVitisNetP4ReturnType XilVitisNetP4TargetInterruptClearP4ElementEccErrorStatusById(XilVitisNetP4TargetInterruptCtx *CtxPtr, uint32_t ElementId, XilVitisNetP4TargetInterruptEccErrorType EccErrorType);

/**
 * Enable ECC Error interrupts for a specified Component
 *
 * If a Component does not exist at the specified index, the function fails by returning error code XIL_VITIS_NET_P4_INTERRUPT_ERR_INVALID_COMPONENT_INDEX.
 *
 * @param[in] CtxPtr        Pointer to the interrupt controller context.
 *
 * @param[in] ComponentIndex     Stores the Component index.
 *
 * @param[in] EccErrorType  Selects which ECC interrupts are enabled for this Component.
 *
 * @return XIL_VITIS_NET_P4_SUCCESS if successful, otherwise an error code.
 */
XilVitisNetP4ReturnType XilVitisNetP4TargetInterruptEnableComponentEccErrorByIndex(XilVitisNetP4TargetInterruptCtx *CtxPtr, uint32_t ComponentIndex, XilVitisNetP4TargetInterruptEccErrorType EccErrorType);

/**
 * Disable ECC Error interrupts for a specified Component
 *
 * If the element identifier does not exist, the function fails by returning error code XIL_VITIS_NET_P4_INTERRUPT_ERR_INVALID_COMPONENT_INDEX.
 *
 * @param[in] CtxPtr        Pointer to the interrupt controller context.
 *
 * @param[in] ComponentIndex     Stores the Component index.
 *
 * @param[in] EccErrorType  Selects which ECC interrupts are disabled for this Component.
 *
 * @return XIL_VITIS_NET_P4_SUCCESS if successful, otherwise an error code.
 */
XilVitisNetP4ReturnType XilVitisNetP4TargetInterruptDisableComponentEccErrorByIndex(XilVitisNetP4TargetInterruptCtx *CtxPtr, uint32_t ComponentIndex, XilVitisNetP4TargetInterruptEccErrorType EccErrorType);

/**
 * Get the ECC Error interrupt status for a specified Component
 *
 * If the element identifier does not exist, the function fails by returning error code XIL_VITIS_NET_P4_INTERRUPT_ERR_INVALID_COMPONENT_INDEX.
 *
 * @param[in]  CtxPtr       Pointer to the interrupt controller context.
 *
 * @param[in]  ComponentIndex    Stores the Component index.
 *
 * @param[out] StatusPtr    Pointer to structure that stores the ECC interrupt status.
 *
 * @return XIL_VITIS_NET_P4_SUCCESS if successful, otherwise an error code.
 */
XilVitisNetP4ReturnType XilVitisNetP4TargetInterruptGetComponentEccErrorStatusByIndex(XilVitisNetP4TargetInterruptCtx *CtxPtr, uint32_t ComponentIndex, XilVitisNetP4TargetInterruptEccErrorStatus *StatusPtr);

/**
 * Clear the ECC Error interrupt status for a specified Component
 *
 * If the element identifier does not exist, the function fails by returning error code XIL_VITIS_NET_P4_INTERRUPT_ERR_INVALID_COMPONENT_INDEX.
 *
 * @param[in] CtxPtr        Pointer to the interrupt controller context.
 *
 * @param[in] ComponentIndex     Stores the Component index.
 *
 * @param[in] EccErrorType  Selects which ECC interrupts are cleared for this element.
 *
 * @return XIL_VITIS_NET_P4_SUCCESS if successful, otherwise an error code.
 */
XilVitisNetP4ReturnType XilVitisNetP4TargetInterruptClearComponentEccErrorStatusByIndex(XilVitisNetP4TargetInterruptCtx *CtxPtr, uint32_t ComponentIndex, XilVitisNetP4TargetInterruptEccErrorType EccErrorType);

/**
 * Destroy the Interrupt instance
 *
 * All memory allocated by \ref XilVitisNetP4TargetInterruptInit is released.
 *
 * @param[in]  CtxPtr           Pointer to the VitisNetP4 instance information context
 *
 * @return XIL_VITIS_NET_P4_SUCCESS if successful, otherwise an error code
 */
XilVitisNetP4ReturnType XilVitisNetP4TargetInterruptExit(XilVitisNetP4TargetInterruptCtx *CtxPtr);

/**
 * Initialization function for the Target Control driver API
 *
 * @param[in,out] CtxPtr        Pointer to an uninitialized Control Register context structure instance.
 *                              Is populated by function execution.
 *
 * @param[in] EnvIfPtr          Pointer to environment interface definition
 *
 * @param[in] ConfigPtr         Pointer to Target Control driver configuration definition
 *
 *
 * @return XIL_VITIS_NET_P4_SUCCESS if successful, otherwise an error code.
 */
XilVitisNetP4ReturnType XilVitisNetP4TargetCtrlInit(XilVitisNetP4TargetCtrlCtx* CtxPtr,
                                          XilVitisNetP4EnvIf* EnvIfPtr,
                                          XilVitisNetP4TargetCtrlConfig* ConfigPtr);

/**
 * Destroy the Target Control instance
 *
 * All memory allocated by \ref XilVitisNetP4TargetCtrlInit is released.
 *
 * @param[in] CtxPtr    Pointer to the BCAM instance context
 *
 * @return XIL_VITIS_NET_P4_SUCCESS if successful, otherwise an error code.
 */
XilVitisNetP4ReturnType XilVitisNetP4TargetCtrlExit(XilVitisNetP4TargetCtrlCtx* CtxPtr);


/**
 * Retrieve the element count
 *
 * @param[in]   CtxPtr         Pointer to the control register context.
 *
 * @param[in]   NumP4ElementsPtr  Pointer to the number of elements
 *
 * @ret
 */
XilVitisNetP4ReturnType XilVitisNetP4TargetCtrlGetP4ElementCount(XilVitisNetP4TargetCtrlCtx* CtxPtr, uint32_t* NumElementsPtr);

/**
 * Retrieve the clock in Hz
 *
 * If the engine type identifier does not exist, the function fails by returning error code.
 *
 * @param[in]   CtxPtr         Pointer to the control register context.
 *
 * @param[in]   ClkInHzPtr  Pointer to the clock in Hz
 *
 * @ret
 */
XilVitisNetP4ReturnType XilVitisNetP4TargetCtrlGetClkInHz(XilVitisNetP4TargetCtrlCtx* CtxPtr, uint32_t* ClkInHzPtr);

/**
 * Retrieve the number of components
 *
 *
 * @param[in]   CtxPtr         Pointer to the control register context.
 *
 * @param[in]   NumComponentsPtr  Pointer to the number of components
 *
 * @ret
 */
XilVitisNetP4ReturnType XilVitisNetP4TargetCtrlGetNumComponents(XilVitisNetP4TargetCtrlCtx* CtxPtr, uint32_t* NumComponentsPtr);

/**
 * Retrieve the packet rate per second
 *
 *
 * @param[in]   CtxPtr         Pointer to the control register context.
 *
 * @param[in]   PktRatePerSecPtr  Pointer to the packet rate per second
 *
 * @ret
 */
XilVitisNetP4ReturnType XilVitisNetP4TargetCtrlGetPktRatePerSec(XilVitisNetP4TargetCtrlCtx* CtxPtr, uint32_t* PktRatePerSecPtr);

/**
 * Retrieve the packet rate to clock ratio
 *
 *
 * @param[in]   CtxPtr         Pointer to the control register context.
 *
 * @param[in]   Pr2ClkRatioPtr  Pointer to the packet rate to clock ratio
 *
 * @ret
 */
XilVitisNetP4ReturnType XilVitisNetP4TargetCtrlGetPr2ClkRatio(XilVitisNetP4TargetCtrlCtx* CtxPtr, float* Pr2ClkRatioPtr);

/**
 * Resets an VITISNETP4 engine
 *
 * If the engine type identifier does not exist, the function fails by returning error code.
 *
 * @param[in]   CtxPtr         Pointer to the control register context.
 *
 * @param[in]   EngineId      type of reset
 *
 * @return XIL_VITIS_NET_P4_SUCCESS if successful, otherwise an error code.
 */
XilVitisNetP4ReturnType XilVitisNetP4TargetCtrlSoftResetEngine(XilVitisNetP4TargetCtrlCtx* CtxPtr, XilVitisNetP4TargetCtrlEngineId EngineId);

/**
 * Set the maximum Start of Frame (SOF) counter per interval
 *
 * @param[in]   CtxPtr  Pointer to the control register context.
 *
 * @param[in]   PacketRateMargin  Packet rate Margin. Integer value for the number of hundreds of a percentage e.g. 512 is 5.12% 
 *
 * @return XIL_VITIS_NET_P4_SUCCESS if successful, otherwise an error code.
 */
XilVitisNetP4ReturnType XilVitisNetP4TargetCtrlSetPacketRateLimitMargin(XilVitisNetP4TargetCtrlCtx* CtxPtr, uint32_t PacketRateMargin);

/**
 * Get the maximum Start of Frame (SOF) counter per interval
 *
 * @param[in]   CtxPtr         Pointer to the control register context.
 *
 * @param[in]   MaxSofCounterPerInvlPtr      Retrieved 16 bit maximum SOF counter per interval value
 *
 * @return XIL_VITIS_NET_P4_SUCCESS if successful, otherwise an error code.
 */
XilVitisNetP4ReturnType XilVitisNetP4TargetCtrlGetPacketRateLimitMargin(XilVitisNetP4TargetCtrlCtx* CtxPtr, uint32_t* PacketRateMarginPtr);

/**
 * Start injecting ECC bit errors for the IP Component
 *
 * If the ComponentIndex does not exist and XIL_VITIS_NET_P4_INTERRUPT_ERR_INVALID_COMPONENT_INDEX is returned
 *
 * @param[in] CtxPtr         Pointer to the control register context.
 *
 * @param[in] ComponentIndex     Stores the FIFO index.
 *
 * @param[in] EccErrorType  Selects which ECC interrupts are disabled for this FIFO.
 *
 * @return XIL_VITIS_NET_P4_SUCCESS if successful, otherwise an error code.
 */
XilVitisNetP4ReturnType XilVitisNetP4TargetCtrlIpComponentEnableInjectEccError(XilVitisNetP4TargetCtrlCtx* CtxPtr,
                                                                     uint32_t ComponentIndex,
                                                                     XilVitisNetP4TargetInterruptEccErrorType EccErrorType);


/**
 * Stop injecting ECC bit errors for the IP Component
 *
 * If the ComponentIndex does not exist and XIL_VITIS_NET_P4_INTERRUPT_ERR_INVALID_COMPONENT_INDEX is returned
 *
 * @param[in] CtxPtr         Pointer to the control register context.
 *
 * @param[in] ComponentIndex     Stores the FIFO index.
 *
 * @param[in] EccErrorType  Selects which ECC interrupts are disabled for this FIFO.
 *
 * @return XIL_VITIS_NET_P4_SUCCESS if successful, otherwise an error code.
 */
XilVitisNetP4ReturnType XilVitisNetP4TargetCtrlIpComponentDisableInjectEccError(XilVitisNetP4TargetCtrlCtx* CtxPtr,
                                                                      uint32_t ComponentIndex,
                                                                      XilVitisNetP4TargetInterruptEccErrorType EccErrorType);


/**
 * Start injecting ECC bit errors for the P4 specific P4ElementId
 *
 * If the P4ElementId does not exist and error code is returned
 *
 * @param[in] CtxPtr         Pointer to the control register context.
 *
 * @param[in] P4ElementId         Stores the P4 element identifier.
 *
 * @param[in] EccErrorType      Selects which ECC interrupts are enabled for this element.
 *
 * @return XIL_VITIS_NET_P4_SUCCESS if successful, otherwise an error code.
 */
XilVitisNetP4ReturnType XilVitisNetP4TargetCtrlP4ElementEnableInjectEccError(XilVitisNetP4TargetCtrlCtx* CtxPtr,
                                                                   uint32_t P4ElementId,
                                                                   XilVitisNetP4TargetInterruptEccErrorType EccErrorType);

/**
 * Stop injecting ECC bit errors for the P4 specific P4ElementId
 *
 * If the P4ElementId does not exist and error code is returned
 *
 * @param[in] CtxPtr         Pointer to the control register context.
 *
 * @param[in] P4ElementId         Stores the P4 element identifier.
 *
 * @param[in] EccErrorType      Selects which ECC interrupts are enabled for this element.
 *
 * @return XIL_VITIS_NET_P4_SUCCESS if successful, otherwise an error code.
 */
XilVitisNetP4ReturnType XilVitisNetP4TargetCtrlP4ElementDisableInjectEccError(XilVitisNetP4TargetCtrlCtx* CtxPtr,
                                                                    uint32_t P4ElementId,
                                                                    XilVitisNetP4TargetInterruptEccErrorType EccErrorType);


/** @} */


#ifdef __cplusplus
}
#endif

#endif

/** @} */
