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
-- Description : An example C program based on the FiveTuple example design that
--       demonstrates target driver usage.
--
--------------------------------------------------------------------------------
*/

/*
 * Target driver sample based on the Five Tuple example for the VITISNETP4 that
 * illustrates how some basic table operations that can be achieved using the
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
#include "vitisnetp4_0_defs.h"

#include <stdio.h>
#include <stdlib.h>

/****************************************************************************************************************************************************/
/* SECTION: Constants/macros */
/****************************************************************************************************************************************************/

#define EXAMPLE_NUM_TABLE_ENTRIES (4)

#define DISPLAY_ERROR(ErrorCode)  printf("Error Code is value %s", XilVitisNetP4ReturnTypeToString(ErrorCode))

#define CONVERT_BITS_TO_BYTES(NumBits) ((NumBits/XIL_VITIS_NET_P4_BITS_PER_BYTE) + ((NumBits % XIL_VITIS_NET_P4_BITS_PER_BYTE)? 1 : 0))

/****************************************************************************************************************************************************/
/* SECTION: Local function declarations*/
/****************************************************************************************************************************************************/
static void DisplayVitisNetP4Versions(XilVitisNetP4TargetCtx *CtxPtr);
/****************************************************************************************************************************************************/
/* SECTION: Global variables */
/****************************************************************************************************************************************************/

/* Key and Responses based on the five Tuple, using Big Endian array */

uint8_t FiveTupleKeyArray[EXAMPLE_NUM_TABLE_ENTRIES][13] = {
    // Entry 1 : InsertVLAN
    // key :[ ipv4.src=fd5f0bc8 ipv4.dst=9aaa2010 ipv4.protocol=11 src_port=2411 dst_port=6cc1 ]
    {0xfd, 0x5f, 0x0b, 0xc8, 0x9a, 0xaa, 0x20, 0x10, 0x11, 0x24, 0x11, 0x6c, 0xc1},
    //Entry 2 : InsertVLAN
    // key :[ ipv4.src=7cf6cd9c ipv4.dst=cc930a03 ipv4.protocol=11 src_port=211e dst_port=5b98 ]
    {0x7c, 0xf6, 0xcd, 0x9c, 0xcc, 0x93, 0x0a, 0x03, 0x11, 0x21, 0x1e, 0x5b, 0x98},
    // Entry 3 : InsertVLAN
    // key :[ ipv4.src=e90a5c71 ipv4.dst=6353a5ca ipv4.protocol=11 src_port=7320 dst_port=85bb ]
    {0xe9, 0x0a, 0x5c, 0x71, 0x63, 0x53, 0xa5, 0xca, 0x11, 0x73, 0x20, 0x85, 0xbb},
    // Entry 4 : InsertVLAN
    // key :[ ipv4.src=ad8575e6 ipv4.dst=ad8575e6 ipv4.protocol=11 src_port=9e55 dst_port=51fa ]
    {0xad, 0x85, 0x75, 0xe6, 0xcc, 0x3d, 0x03, 0xd7, 0x11, 0x9e, 0x55, 0x51, 0xfa}
};

/*
 * The corresponding Action Parameters used to create the loop
 * Note: The Action Parameters are concatenated with the Action Id to construct the table response of the Match-Action unit
 */
uint8_t FiveTupleActionParamsArray[EXAMPLE_NUM_TABLE_ENTRIES][2] = {
    // Entry 1 : InsertVLAN
    // response :[ pcp=1 cfi=1 vid=111 ]
    {0x31, 0x11},
    // Entry 2 : InsertVLAN
    // response :[ pcp=1 cfi=1 vid=222 ]
    {0x32, 0x22},
    // Entry 3 : InsertVLAN
    //response :[ pcp=4 cfi=0 vid=333 ]
    {0x83, 0x33},
    // Entry 4 : InsertVLAN
    // response :[ pcp=4 cfi=0 vid=444 ]
    {0x84, 0x44}
};


/****************************************************************************************************************************************************/
/* SECTION: Entry point */
/****************************************************************************************************************************************************/

int main(void)
{
    XilVitisNetP4EnvIf EnvIf;
    XilVitisNetP4TargetCtx FiveTupleTargetCtx;
    XilVitisNetP4TableCtx *FiveTupleTableCtxPtr;
    XilVitisNetP4ReturnType Result;
    uint32_t Index;
    uint32_t ActionId;
    uint8_t ReadParamActionsBuffer[2];
    uint32_t ReadActionId;

    Result = XilVitisNetP4StubEnvIf(&EnvIf);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        DISPLAY_ERROR(Result);
        goto exit_example;
    }

    printf("Initialize the Target Driver\n\r");
    Result = XilVitisNetP4TargetInit(&FiveTupleTargetCtx, &EnvIf, &(XilVitisNetP4TargetConfig_0));
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

    printf("Get Table Handle\n\r");
    /* For the FiveTuple example only a single table exists*/
    Result = XilVitisNetP4TargetGetTableByName(&FiveTupleTargetCtx, "FiveTuple", &FiveTupleTableCtxPtr);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        DISPLAY_ERROR(Result);
        goto target_exit;
    }

    printf("Get ActionId\n\r");
    /*
     * NOTE: the action Id for all entries in the Five Tuple has only one actions
     */
    Result = XilVitisNetP4TableGetActionId(FiveTupleTableCtxPtr, "InsertVLAN", &ActionId);
    if (Result != XIL_VITIS_NET_P4_SUCCESS)
    {
        DISPLAY_ERROR(Result);
        goto target_exit;
    }


    for (Index = 0; Index < EXAMPLE_NUM_TABLE_ENTRIES; Index++)
    {
        printf("Insert table entry %d\n\r", Index);

        Result = XilVitisNetP4TableInsert(FiveTupleTableCtxPtr,
                                     FiveTupleKeyArray[Index],
                                     NULL, // MaskPtr not used for a table with mode of BCAM
                                     0x0, // Priority is ignored for a table with mode of BCAM
                                     ActionId,
                                     FiveTupleActionParamsArray[Index]);
        if (Result != XIL_VITIS_NET_P4_SUCCESS)
        {
            DISPLAY_ERROR(Result);
            goto target_exit;
        }

        Result = XilVitisNetP4TableGetByKey(FiveTupleTableCtxPtr,
                                       FiveTupleKeyArray[Index],
                                       NULL, // MaskPtr not used for a table with mode of BCAM
                                       0x0, // Priority is ignored for a table with mode of BCAM
                                       &ReadActionId,
                                       ReadParamActionsBuffer);
        if (Result == XIL_VITIS_NET_P4_SUCCESS)
        {
            printf("For table entry %d the Action Parameters are 0x%02X, 0x%02X and Action Id is %d\n\r",
                   Index,
                   ReadParamActionsBuffer[0],
                   ReadParamActionsBuffer[1],
                   ReadActionId);
        }
        else
        {
            DISPLAY_ERROR(Result);
            goto target_exit;
        }

        printf("Updating the Response for table entry %d\n\r", Index);
        ReadParamActionsBuffer[0]++;
        ReadParamActionsBuffer[1]++;
        Result = XilVitisNetP4TableUpdate(FiveTupleTableCtxPtr,
                                     FiveTupleKeyArray[Index],
                                     NULL, // MaskPtr not used for a table with mode of BCAM
                                     ActionId,
                                     ReadParamActionsBuffer);
        if (Result != XIL_VITIS_NET_P4_SUCCESS)
        {
            DISPLAY_ERROR(Result);
            goto target_exit;
        }

        Result = XilVitisNetP4TableGetByKey(FiveTupleTableCtxPtr,
                                       FiveTupleKeyArray[Index],
                                       NULL, // MaskPtr not used for a table with mode of BCAM
                                       0x0, // Priority is ignored for a table with mode of BCAM
                                       &ReadActionId,
                                       ReadParamActionsBuffer);
        if (Result == XIL_VITIS_NET_P4_SUCCESS)
        {
            printf("For table entry %d the Action Parameters are 0x%02X, 0x%02X and Action Id is %d\n\r",
                   Index,
                   ReadParamActionsBuffer[0],
                   ReadParamActionsBuffer[1],
                   ReadActionId);
        }
        else
        {
            DISPLAY_ERROR(Result);
            goto target_exit;
        }
    }

    for (Index = 0; Index < EXAMPLE_NUM_TABLE_ENTRIES; Index++)
    {
        printf("Delete table entry %d\n\r", Index);
        Result = XilVitisNetP4TableDelete(FiveTupleTableCtxPtr, FiveTupleKeyArray[Index], NULL);

        if (Result == XIL_VITIS_NET_P4_SUCCESS)
        {
            /* Not neccessary but checking if the key can be found to demo the usage */
            Result = XilVitisNetP4TableGetByKey(FiveTupleTableCtxPtr,
                                           FiveTupleKeyArray[Index],
                                           NULL, // MaskPtr not used for a table with mode of BCAM
                                           0x0, // Priority is ignored for a table with mode of BCAM
                                           &ReadActionId,
                                           ReadParamActionsBuffer);
            if (Result != XIL_VITIS_NET_P4_CAM_ERR_KEY_NOT_FOUND)
            {
                printf("Error table entry %d is present\n\r", Index);
            }
            else
            {
                printf("Table entry %d successfully deleted\n\r", Index);
            }
        }
        else
        {
            DISPLAY_ERROR(Result);
            goto target_exit;
        }
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

