/*******************************************************************************
* Project Name      : CAN/LIN Protocol Stack
* Platform          : Arm
* Revision Number   : V1.0
* Compiled Version  : G32A1xxx_01-June-25
*
* Copyright (C) 2025 Geehy Semiconductor
*
* You may not use this file except in compliance with the GEEHY COPYRIGHT NOTICE
* (GEEHY SOFTWARE PACKAGE LICENSE).
*
* The program is only for reference, which is distributed in the hope that it
* will be useful and instructional for customers to develop their software.
* Unless required by applicable law or agreed to in writing, the program is
* distributed on an "AS IS" BASIS, WITHOUT ANY WARRANTY OR CONDITIONS OF ANY
* KIND, either express or implied. See the GEEHY SOFTWARE PACKAGE LICENSE for
* the governing permissions and limitations under the License.
*
*******************************************************************************/

#include "AES.h"
#ifdef EN_AES_SA_ALGORITHM_SW
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**************************************************************************
                    GLOBAL VARIABLE
**************************************************************************/
static const sint32 S[16][16] =
{
    { 0x63, 0x7C, 0x77, 0x7B, 0xF2, 0x6B, 0x6F, 0xC5, 0x30, 0x01, 0x67, 0x2B, 0xFE, 0xD7, 0xAB, 0x76 },
    { 0xCA, 0x82, 0xC9, 0x7D, 0xFA, 0x59, 0x47, 0xF0, 0xAD, 0xD4, 0xA2, 0xAF, 0x9C, 0xA4, 0x72, 0xC0 },
    { 0xB7, 0xFD, 0x93, 0x26, 0x36, 0x3F, 0xF7, 0xCC, 0x34, 0xA5, 0xE5, 0xF1, 0x71, 0xD8, 0x31, 0x15 },
    { 0x04, 0xC7, 0x23, 0xC3, 0x18, 0x96, 0x05, 0x9A, 0x07, 0x12, 0x80, 0xE2, 0xEB, 0x27, 0xB2, 0x75 },
    { 0x09, 0x83, 0x2C, 0x1A, 0x1B, 0x6E, 0x5A, 0xA0, 0x52, 0x3B, 0xD6, 0xB3, 0x29, 0xE3, 0x2F, 0x84 },
    { 0x53, 0xD1, 0x00, 0xED, 0x20, 0xFC, 0xB1, 0x5B, 0x6A, 0xCB, 0xBE, 0x39, 0x4A, 0x4C, 0x58, 0xCF },
    { 0xD0, 0xEF, 0xAA, 0xFB, 0x43, 0x4D, 0x33, 0x85, 0x45, 0xF9, 0x02, 0x7F, 0x50, 0x3C, 0x9F, 0xA8 },
    { 0x51, 0xA3, 0x40, 0x8F, 0x92, 0x9D, 0x38, 0xF5, 0xBC, 0xB6, 0xDA, 0x21, 0x10, 0xFF, 0xF3, 0xD2 },
    { 0xCD, 0x0C, 0x13, 0xEC, 0x5F, 0x97, 0x44, 0x17, 0xC4, 0xA7, 0x7E, 0x3D, 0x64, 0x5D, 0x19, 0x73 },
    { 0x60, 0x81, 0x4F, 0xDC, 0x22, 0x2A, 0x90, 0x88, 0x46, 0xEE, 0xB8, 0x14, 0xDE, 0x5E, 0x0B, 0xDB },
    { 0xE0, 0x32, 0x3A, 0x0A, 0x49, 0x06, 0x24, 0x5C, 0xC2, 0xD3, 0xAC, 0x62, 0x91, 0x95, 0xE4, 0x79 },
    { 0xE7, 0xC8, 0x37, 0x6D, 0x8D, 0xD5, 0x4E, 0xA9, 0x6C, 0x56, 0xF4, 0xEA, 0x65, 0x7A, 0xAE, 0x08 },
    { 0xBA, 0x78, 0x25, 0x2E, 0x1C, 0xA6, 0xB4, 0xC6, 0xE8, 0xDD, 0x74, 0x1F, 0x4B, 0xBD, 0x8B, 0x8A },
    { 0x70, 0x3E, 0xB5, 0x66, 0x48, 0x03, 0xF6, 0x0E, 0x61, 0x35, 0x57, 0xB9, 0x86, 0xC1, 0x1D, 0x9E },
    { 0xE1, 0xF8, 0x98, 0x11, 0x69, 0xD9, 0x8E, 0x94, 0x9B, 0x1E, 0x87, 0xE9, 0xCE, 0x55, 0x28, 0xDF },
    { 0x8C, 0xA1, 0x89, 0x0D, 0xBF, 0xE6, 0x42, 0x68, 0x41, 0x99, 0x2D, 0x0F, 0xB0, 0x54, 0xBB, 0x16 }
};

static const sint32 S2[16][16] =
{
    { 0x52, 0x09, 0x6A, 0xD5, 0x30, 0x36, 0xA5, 0x38, 0xBF, 0x40, 0xA3, 0x9E, 0x81, 0xF3, 0xD7, 0xFB },
    { 0x7C, 0xE3, 0x39, 0x82, 0x9B, 0x2F, 0xFF, 0x87, 0x34, 0x8E, 0x43, 0x44, 0xC4, 0xDE, 0xE9, 0xCB },
    { 0x54, 0x7B, 0x94, 0x32, 0xA6, 0xC2, 0x23, 0x3D, 0xEE, 0x4C, 0x95, 0x0B, 0x42, 0xFA, 0xC3, 0x4E },
    { 0x08, 0x2E, 0xA1, 0x66, 0x28, 0xD9, 0x24, 0xB2, 0x76, 0x5B, 0xA2, 0x49, 0x6D, 0x8B, 0xD1, 0x25 },
    { 0x72, 0xF8, 0xF6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xD4, 0xA4, 0x5C, 0xCC, 0x5D, 0x65, 0xB6, 0x92 },
    { 0x6C, 0x70, 0x48, 0x50, 0xFD, 0xED, 0xB9, 0xDA, 0x5E, 0x15, 0x46, 0x57, 0xA7, 0x8D, 0x9D, 0x84 },
    { 0x90, 0xD8, 0xAB, 0x00, 0x8C, 0xBC, 0xD3, 0x0A, 0xF7, 0xE4, 0x58, 0x05, 0xB8, 0xB3, 0x45, 0x06 },
    { 0xD0, 0x2C, 0x1E, 0x8F, 0xCA, 0x3F, 0x0F, 0x02, 0xC1, 0xAF, 0xBD, 0x03, 0x01, 0x13, 0x8A, 0x6B },
    { 0x3A, 0x91, 0x11, 0x41, 0x4F, 0x67, 0xDC, 0xEA, 0x97, 0xF2, 0xCF, 0xCE, 0xF0, 0xB4, 0xE6, 0x73 },
    { 0x96, 0xAC, 0x74, 0x22, 0xE7, 0xAD, 0x35, 0x85, 0xE2, 0xF9, 0x37, 0xE8, 0x1C, 0x75, 0xDF, 0x6E },
    { 0x47, 0xF1, 0x1A, 0x71, 0x1D, 0x29, 0xC5, 0x89, 0x6F, 0xB7, 0x62, 0x0E, 0xAA, 0x18, 0xBE, 0x1B },
    { 0xFC, 0x56, 0x3E, 0x4B, 0xC6, 0xD2, 0x79, 0x20, 0x9A, 0xDB, 0xC0, 0xFE, 0x78, 0xCD, 0x5A, 0xF4 },
    { 0x1F, 0xDD, 0xA8, 0x33, 0x88, 0x07, 0xC7, 0x31, 0xB1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xEC, 0x5F },
    { 0x60, 0x51, 0x7F, 0xA9, 0x19, 0xB5, 0x4A, 0x0D, 0x2D, 0xE5, 0x7A, 0x9F, 0x93, 0xC9, 0x9C, 0xEF },
    { 0xA0, 0xE0, 0x3B, 0x4D, 0xAE, 0x2A, 0xF5, 0xB0, 0xC8, 0xEB, 0xBB, 0x3C, 0x83, 0x53, 0x99, 0x61 },
    { 0x17, 0x2B, 0x04, 0x7E, 0xBA, 0x77, 0xD6, 0x26, 0xE1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0C, 0x7D }
};

static sint32 w[44];

static const sint32 Rcon[10] = { 0x01000000, 0x02000000, 0x04000000, 0x08000000, 0x10000000, 0x20000000,
                                 0x40000000, 0x80000000, 0x1B000000, 0x36000000};

static const sint32 colM[4][4] ={ {2, 3, 1, 1}, {1, 2, 3, 1}, {1, 1, 2, 3}, {3, 1, 1, 2} };

static const sint32 deColM[4][4] =
{ {0x0E, 0x0B, 0x0D, 0x09}, {0x09, 0x0E, 0x0B, 0x0D}, {0x0D, 0x09, 0x0E, 0x0B}, {0x0B, 0x0D, 0x09, 0x0E} };

/*******************************************************************************
                               LOCAL FUNCTIONS
*******************************************************************************/
static sint32 AES_MyLeftNibble(sint32 pNum)
{
    sint32 tempVal = pNum & 0x000000F0;
    return tempVal >> 4;
}

static sint32 AES_MyRightNibble(sint32 pNum)
{
    return pNum & 0x0000000F;
}

static sint32 AES_MyGetNumFromSBox(sint32 pIndex)
{
    sint32 r = AES_MyLeftNibble(pIndex);
    sint32 c = AES_MyRightNibble(pIndex);
    return S[r][c];
}

static sint32 AES_MyGetIntFromChar(sint8 inputChar)
{
    sint32 myRes = (sint32)inputChar;
    return myRes & 0x000000FF;
}

static void AES_MyConvertToIntArray(sint8 *pInStr, sint32 arrOut[4][4])
{
    /* Convert nested for loops to while loops */
    sint32 myCount = 0;
    sint32 rowIndex = 0;
    sint32 colIndex = 0;

    while (rowIndex < 4)
    {
        colIndex = 0;
        while (colIndex < 4)
        {
            arrOut[colIndex][rowIndex] = AES_MyGetIntFromChar(pInStr[myCount]);
            myCount++;
            colIndex++;
        }
        rowIndex++;
    }
}

static sint32 AES_GetIntFromStr(sint8 *pInput)
{
    /* Local variables for combination */
    sint32 vVal1, vVal2, vVal3, vVal4;

    /* Retrieve and shift vVal1 */
    vVal1 = AES_MyGetIntFromChar(pInput[0]);
    vVal1 = vVal1 << 24;

    /* Retrieve and shift vVal2 */
    vVal2 = AES_MyGetIntFromChar(pInput[1]);
    vVal2 = vVal2 << 16;

    /* Retrieve and shift vVal3 */
    vVal3 = AES_MyGetIntFromChar(pInput[2]);
    vVal3 = vVal3 << 8;

    /* Retrieve vVal4 directly */
    vVal4 = AES_MyGetIntFromChar(pInput[3]);

    /* Combine the four parts using bitwise OR */
    return (vVal1 | vVal2 | vVal3 | vVal4);
}

static void AES_SplitIntToArr(sint32 vNumber, sint32 vArray[4])
{
    /* Temporary variables to store shifted values */
    sint32 vA, vB, vC;

    /* Shift and mask the first byte */
    vA = vNumber >> 24;
    vArray[0] = vA & 0x000000FF;

    /* Shift and mask the second byte */
    vB = vNumber >> 16;
    vArray[1] = vB & 0x000000FF;

    /* Shift and mask the third byte */
    vC = vNumber >> 8;
    vArray[2] = vC & 0x000000FF;

    /* Directly mask the fourth byte */
    vArray[3] = vNumber & 0x000000FF;
}

static void AES_LeftShiftArray(sint32 vArr[4], sint32 vStep)
{
    sint32 vTemp[4];
    sint32 vIdx = 0;
    sint32 vIndexVal;

    vIdx = 0;
    while (vIdx < 4)
    {
        vTemp[vIdx] = vArr[vIdx];
        vIdx++;
    }

    if ((vStep % 4) != 0)
    {
        vIndexVal = vStep % 4;
    }
    else
    {
        vIndexVal = 0;
    }

    vIdx = 0;
    while (vIdx < 4)
    {
        vArr[vIdx] = vTemp[vIndexVal];
        vIndexVal++;
        vIndexVal = vIndexVal % 4;
        vIdx++;
    }
}

static sint32 AES_MergeArrToInt(sint32 intArr[4])
{
    return ((intArr[0] << 24) | (intArr[1] << 16) | (intArr[2] << 8) | (intArr[3]));
}

static sint32 DeSens_ComputeT(sint32 pNumber, sint32 pRound)
{
    sint32 vTempArr[4];
    sint32 vIdx = 0;
    sint32 vResult;
    
    AES_SplitIntToArr(pNumber, vTempArr);
    AES_LeftShiftArray(vTempArr, 1);
    
    vIdx = 0;
    while (vIdx < 4)
    {
        vTempArr[vIdx] = AES_MyGetNumFromSBox(vTempArr[vIdx]);
        vIdx++;
    }
    
    vResult = AES_MergeArrToInt(vTempArr);
    return (vResult ^ Rcon[pRound]);
}


static void AES_MyExtendKey(sint8 *pKey)
{
    /* Local variables */
    sint32 iIndex = 0;
    sint32 jIndex = 0;

    iIndex = 0;
    while (iIndex < 4)
    {
        w[iIndex] = AES_GetIntFromStr(pKey + iIndex * 4);
        iIndex++;
    }

    iIndex = 4;
    jIndex = 0;

    while (iIndex < 44)
    {
        if ((iIndex % 4) != 0)
        {
            /* Original else part */
            w[iIndex] = w[iIndex - 4] ^ w[iIndex - 1];
        }
        else
        {
            /* Original if part */
            w[iIndex] = w[iIndex - 4] ^ DeSens_ComputeT(w[iIndex - 1], jIndex);
            jIndex++;
        }

        iIndex++;
    }
}

static void AES_MyAddRoundKey(sint32 pMatrix[4][4], sint32 rCount)
{
    /* Local temporary array */
    sint32 tempArray[4];
    /* Local loop variables */
    sint32 outerIndex = 0;
    sint32 innerIndex = 0;

    outerIndex = 0;
    while (outerIndex < 4)
    {
        /* Split integer into array using the external function */
        AES_SplitIntToArr(w[rCount * 4 + outerIndex], tempArray);

        innerIndex = 0;
        while (innerIndex < 4)
        {
            pMatrix[innerIndex][outerIndex] =
                pMatrix[innerIndex][outerIndex] ^ tempArray[innerIndex];
            innerIndex++;
        }

        outerIndex++;
    }
}

static void AES_SubstituteBytes(sint32 pMatrix[4][4])
{
    sint32 vRow = 0;
    sint32 vCol = 0;

    vRow = 0;
    while (vRow < 4)
    {
        vCol = 0;
        while (vCol < 4)
        {
            pMatrix[vRow][vCol] = AES_MyGetNumFromSBox(pMatrix[vRow][vCol]);
            vCol++;
        }
        vRow++;
    }
}

static void AES_ShiftRows(sint32 pMatrix[4][4])
{
    sint32 vRow2[4], vRow3[4], vRow4[4];
    /* Loop counter */
    sint32 vIdx = 0;

    vIdx = 0;
    while (vIdx < 4)
    {
        vRow2[vIdx] = pMatrix[1][vIdx];
        vRow3[vIdx] = pMatrix[2][vIdx];
        vRow4[vIdx] = pMatrix[3][vIdx];
        vIdx++;
    }

    /* Perform cycle shifts on each row */
    AES_LeftShiftArray(vRow2, 1);
    AES_LeftShiftArray(vRow3, 2);
    AES_LeftShiftArray(vRow4, 3);

    vIdx = 0;
    while (vIdx < 4)
    {
        pMatrix[1][vIdx] = vRow2[vIdx];
        pMatrix[2][vIdx] = vRow3[vIdx];
        pMatrix[3][vIdx] = vRow4[vIdx];
        vIdx++;
    }
}

static sint32 AES_MyMergedGFMul(sint32 pFactor, sint32 pVal)
{
    /* Local result variable */
    sint32 finalRes = 0;

    switch (pFactor)
    {
        case 1:
        {
            finalRes = pVal;
        }
        break;

        case 2:
        {
            sint32 tmpVal = pVal << 1;
            sint32 checkBit = tmpVal & 0x00000100;
            if (checkBit == 0)
            {
                /* do nothing */
            }
            else
            {
                tmpVal = tmpVal & 0x000000FF;
                tmpVal = tmpVal ^ 0x1B;
            }
            finalRes = tmpVal;
        }
        break;

        case 3:
        {
            sint32 tmpVal = pVal << 1;
            sint32 checkBit = tmpVal & 0x00000100;
            if (checkBit == 0)
            {
                /* do nothing */
            }
            else
            {
                tmpVal = tmpVal & 0x000000FF;
                tmpVal = tmpVal ^ 0x1B;
            }
            finalRes = tmpVal ^ pVal;
        }
        break;

        case 0x9:
        {
            sint32 step1 = pVal << 1;
            sint32 bit1 = step1 & 0x00000100;
            if (bit1 == 0)
            {
                /* do nothing */
            }
            else
            {
                step1 = step1 & 0x000000FF;
                step1 = step1 ^ 0x1B;
            }
            sint32 step2 = step1 << 1;
            sint32 bit2 = step2 & 0x00000100;
            if (bit2 == 0)
            {
                /* do nothing */
            }
            else
            {
                step2 = step2 & 0x000000FF;
                step2 = step2 ^ 0x1B;
            }
            sint32 step3 = step2 << 1;
            sint32 bit3 = step3 & 0x00000100;
            if (bit3 == 0)
            {
                /* do nothing */
            }
            else
            {
                step3 = step3 & 0x000000FF;
                step3 = step3 ^ 0x1B;
            }
            finalRes = step3 ^ pVal;
        }
        break;

        case 0xB:
        {
            sint32 mul2Val = pVal << 1;
            sint32 chkVal = mul2Val & 0x00000100;
            if (chkVal == 0)
            {
                /* do nothing */
            }
            else
            {
                mul2Val = mul2Val & 0x000000FF;
                mul2Val = mul2Val ^ 0x1B;
            }

            sint32 step1 = pVal << 1;
            sint32 bit1 = step1 & 0x00000100;
            if (bit1 == 0)
            {
                /* do nothing */
            }
            else
            {
                step1 = step1 & 0x000000FF;
                step1 = step1 ^ 0x1B;
            }
            sint32 step2 = step1 << 1;
            sint32 bit2 = step2 & 0x00000100;
            if (bit2 == 0)
            {
                /* do nothing */
            }
            else
            {
                step2 = step2 & 0x000000FF;
                step2 = step2 ^ 0x1B;
            }

            sint32 step3 = step2 << 1;
            sint32 bit3 = step3 & 0x00000100;
            if (bit3 == 0)
            {
                /* do nothing */
            }
            else
            {
                step3 = step3 & 0x000000FF;
                step3 = step3 ^ 0x1B;
            }
            sint32 gfMul9Val = step3 ^ pVal;

            finalRes = gfMul9Val ^ mul2Val;
        }
        break;

        case 0xD:
        {
            sint32 step1 = pVal << 1;
            sint32 bit1 = step1 & 0x00000100;
            if (bit1 == 0)
            {
                /* do nothing */
            }
            else
            {
                step1 = step1 & 0x000000FF;
                step1 = step1 ^ 0x1B;
            }
            sint32 step2 = step1 << 1;
            sint32 bit2 = step2 & 0x00000100;
            if (bit2 == 0)
            {
                /* do nothing */
            }
            else
            {
                step2 = step2 & 0x000000FF;
                step2 = step2 ^ 0x1B;
            }
            sint32 step3 = step2 << 1;
            sint32 bit3 = step3 & 0x00000100;
            if (bit3 == 0)
            {
                /* do nothing */
            }
            else
            {
                step3 = step3 & 0x000000FF;
                step3 = step3 ^ 0x1B;
            }
            sint32 gfMul12Val = step3 ^ step2;

            finalRes = gfMul12Val ^ pVal;
        }
        break;

        case 0xE:
        {
            sint32 step1 = pVal << 1;
            sint32 bit1 = step1 & 0x00000100;
            if (bit1 == 0)
            {
                /* do nothing */
            }
            else
            {
                step1 = step1 & 0x000000FF;
                step1 = step1 ^ 0x1B;
            }
            sint32 step2 = step1 << 1;
            sint32 bit2 = step2 & 0x00000100;
            if (bit2 == 0)
            {
                /* do nothing */
            }
            else
            {
                step2 = step2 & 0x000000FF;
                step2 = step2 ^ 0x1B;
            }
            /* step2 = GFMul4(s) */

            sint32 step3 = step2 << 1;
            sint32 bit3 = step3 & 0x00000100;
            if (bit3 == 0)
            {
                /* do nothing */
            }
            else
            {
                step3 = step3 & 0x000000FF;
                step3 = step3 ^ 0x1B;
            }
            sint32 gfMul12Val = step3 ^ step2;

            sint32 mul2Val = pVal << 1;
            sint32 chkVal = mul2Val & 0x00000100;
            if (chkVal == 0)
            {
                /* do nothing */
            }
            else
            {
                mul2Val = mul2Val & 0x000000FF;
                mul2Val = mul2Val ^ 0x1B;
            }

            finalRes = gfMul12Val ^ mul2Val;
        }
        break;

        default:
        {
            /* do nothing */
        }
        break;
    }

#if defined(__GNUC__) && !defined(__ARMCC_VERSION)
    #pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#elif defined(__ICCARM__)
    #pragma diag_suppress=Pe080
#else
    #pragma clang diagnostic ignored "-Wuninitialized"
#endif

    return finalRes;
}

static void AES_MyMixColumns(sint32 pMatrix[4][4])
{
    /* Local temporary array for backup */
    sint32 localTemp[4][4];
    /* Loop counters */
    sint32 outerIndex = 0;
    sint32 innerIndex = 0;

    outerIndex = 0;
    while (outerIndex < 4)
    {
        innerIndex = 0;
        while (innerIndex < 4)
        {
            localTemp[outerIndex][innerIndex] = pMatrix[outerIndex][innerIndex];
            innerIndex++;
        }
        outerIndex++;
    }

    outerIndex = 0;
    while (outerIndex < 4)
    {
        innerIndex = 0;
        while (innerIndex < 4)
        {
            pMatrix[outerIndex][innerIndex] =
                AES_MyMergedGFMul(colM[outerIndex][0], localTemp[0][innerIndex]) ^
                AES_MyMergedGFMul(colM[outerIndex][1], localTemp[1][innerIndex]) ^
                AES_MyMergedGFMul(colM[outerIndex][2], localTemp[2][innerIndex]) ^
                AES_MyMergedGFMul(colM[outerIndex][3], localTemp[3][innerIndex]);
            innerIndex++;
        }
        outerIndex++;
    }
}

static void AES_MyConvertArrayStr(sint32 pMatrix[4][4], sint8 *pBuf)
{
    /* Loop index variables */
    sint32 outerIdx = 0;
    sint32 innerIdx = 0;

    outerIdx = 0;
    while (outerIdx < 4)
    {
        innerIdx = 0;
        while (innerIdx < 4)
        {
            *pBuf++ = (sint8)(pMatrix[innerIdx][outerIdx]);
            innerIdx++;
        }
        outerIdx++;
    }
}

static sint32 AES_CheckKeyLen(sint32 pLen)
{
    return (pLen == 16) ? 1 : 0;
}

void AES_MyProcessAES(sint8 *pData, sint32 pDataLen, sint8 *pKeyData, sint8 *pCipherBuf)
{
    sint32 localMatrix[4][4];
    sint32 outerLoop = 0;
    sint32 blockIdx = 0;
    sint32 localKeySize = 16;

    if (!(pDataLen == 0 || (pDataLen % 16) != 0))
    {
        /* Do nothing */
    }
    else
    {
        return;
    }

    if (AES_CheckKeyLen(localKeySize))
    {
        /* Do nothing */
    }
    else
    {
        return;
    }

    /* Extend the key */
    AES_MyExtendKey(pKeyData);

    /* Process 16-byte blocks */
    blockIdx = 0;
    while (blockIdx < pDataLen)
    {
        /* Convert input buffer to 4x4 matrix */
        AES_MyConvertToIntArray(pData + blockIdx, localMatrix);

        /* Initial AddRoundKey operation */
        AES_MyAddRoundKey(localMatrix, 0);

        outerLoop = 1;
        while (outerLoop < 10)
        {
            AES_SubstituteBytes(localMatrix);
            AES_ShiftRows(localMatrix);
            AES_MyMixColumns(localMatrix);
            AES_MyAddRoundKey(localMatrix, outerLoop);
            outerLoop++;
        }

        /* Final round */
        AES_SubstituteBytes(localMatrix);
        AES_ShiftRows(localMatrix);
        AES_MyAddRoundKey(localMatrix, 10);

        /* Convert the matrix back into output buffer */
        AES_MyConvertArrayStr(localMatrix, pCipherBuf + blockIdx);

        /* Move to the next 16-byte block */
        blockIdx += 16;
    }
}

static sint32 AES_MyGetValueFromBox(sint32 index)
{
    return S2[AES_MyLeftNibble(index)][AES_MyRightNibble(index)];
}

static void AES_MyTransformBytes(sint32 pBox[4][4])
{
    /* Local index variables */
    sint32 rowIndex = 0;
    sint32 colIndex = 0;

    rowIndex = 0;
    while (rowIndex < 4)
    {
        colIndex = 0;
        while (colIndex < 4)
        {
            pBox[rowIndex][colIndex] = AES_MyGetValueFromBox(pBox[rowIndex][colIndex]);
            colIndex++;
        }
        rowIndex++;
    }
}

static void AES_MyRightShift(sint32 pArr[4], sint32 pStep)
{
    sint32 localTemp[4];
    sint32 idx = 0;
    sint32 myIndex = 0;

    idx = 0;
    while (idx < 4)
    {
        localTemp[idx] = pArr[idx];
        idx++;
    }

    myIndex = (pStep % 4 == 0) ? 0 : (pStep % 4);
    myIndex = 3 - myIndex;

    idx = 3;
    while (idx >= 0)
    {
        pArr[idx] = localTemp[myIndex];
        myIndex--;
        myIndex = (myIndex == -1) ? 3 : myIndex;
        idx--;
    }
}

static void AES_MyRearrangeRows(sint32 pMatrix[4][4])
{
    sint32 localRow2[4], localRow3[4], localRow4[4];
    sint32 localIdx = 0;

    localIdx = 0;
    while (localIdx < 4)
    {
        localRow2[localIdx] = pMatrix[1][localIdx];
        localRow3[localIdx] = pMatrix[2][localIdx];
        localRow4[localIdx] = pMatrix[3][localIdx];
        localIdx++;
    }

    AES_MyRightShift(localRow2, 1);
    AES_MyRightShift(localRow3, 2);
    AES_MyRightShift(localRow4, 3);

    localIdx = 0;
    while (localIdx < 4)
    {
        pMatrix[1][localIdx] = localRow2[localIdx];
        pMatrix[2][localIdx] = localRow3[localIdx];
        pMatrix[3][localIdx] = localRow4[localIdx];
        localIdx++;
    }
}



static void AES_rearrangeColumns(sint32 matrix[4][4])
{
    sint32 tempMatrix[4][4];
    sint32 outerIndex, innerIndex;

    outerIndex = 0;
    while (outerIndex < 4)
    {
        innerIndex = 0;
        while (innerIndex < 4)
        {
            tempMatrix[outerIndex][innerIndex] = matrix[outerIndex][innerIndex];
            innerIndex++;
        }
        outerIndex++;
    }

    /* Reassign values based on Galois Field multiplication */
    outerIndex = 0;
    while (outerIndex < 4)
    {
        innerIndex = 0;
        while (innerIndex < 4)
        {
            matrix[outerIndex][innerIndex] = 
                AES_MyMergedGFMul(deColM[outerIndex][0], tempMatrix[0][innerIndex]) 
                ^ AES_MyMergedGFMul(deColM[outerIndex][1], tempMatrix[1][innerIndex])
                ^ AES_MyMergedGFMul(deColM[outerIndex][2], tempMatrix[2][innerIndex]) 
                ^ AES_MyMergedGFMul(deColM[outerIndex][3], tempMatrix[3][innerIndex]);
            innerIndex++;
        }
        outerIndex++;
    }
}

static void AES_combineMatrixValues(sint32 xMatrix[4][4], sint32 yMatrix[4][4])
{
    /* Declare iteration variables */
    sint32 outerIndex, innerIndex;

    /* Initialize outer index */
    outerIndex = 0;
    while (outerIndex < 4)
    {
        /* Initialize inner index */
        innerIndex = 0;
        while (innerIndex < 4)
        {
            /* Perform XOR operation */
            xMatrix[outerIndex][innerIndex] = xMatrix[outerIndex][innerIndex] ^ yMatrix[outerIndex][innerIndex];

            innerIndex++;
        }
        outerIndex++;
    }
}

static void AES_transformMatrix4W(sint32 paramIndex, sint32 outputMatrix[4][4])
{
    sint32 localIndex = paramIndex * 4;
    sint32 tempColOne[4], tempColTwo[4], tempColThree[4], tempColFour[4];
    sint32 counter = 0;
    
    AES_SplitIntToArr(w[localIndex], tempColOne);
    AES_SplitIntToArr(w[localIndex + 1], tempColTwo);
    AES_SplitIntToArr(w[localIndex + 2], tempColThree);
    AES_SplitIntToArr(w[localIndex + 3], tempColFour);

    while (counter < 4)
    {
        outputMatrix[counter][0] = tempColOne[counter];
        outputMatrix[counter][1] = tempColTwo[counter];
        outputMatrix[counter][2] = tempColThree[counter];
        outputMatrix[counter][3] = tempColFour[counter];
        counter++;
    }
}

/*******************************************************************************
                               GLOBAL FUNCTIONS
*******************************************************************************/
void AES_aesDecryptCore(sint8 *paramIn, sint32 paramInLen, sint8 *paramKey, sint8 *paramOut)
{
    sint32 lengthOfKey = 16;
    sint32 outerCounter, innerCounter;
    sint32 matrixC[4][4];
    sint32 matrixW[4][4];

    if (paramInLen != 0 && (paramInLen % 16) == 0)
    {
        /* Do nothing */
    }
    else
    {
        return;
    }

    if (AES_CheckKeyLen(lengthOfKey))
    {
        /* Do nothing */
    }
    else
    {
        return;
    }

    AES_MyExtendKey(paramKey);
    outerCounter = 0;
    while (outerCounter < paramInLen)
    {
        AES_MyConvertToIntArray(paramIn + outerCounter, matrixC);

        AES_MyAddRoundKey(matrixC, 10);
        innerCounter = 9;
        while (innerCounter >= 1)
        {
            AES_MyTransformBytes(matrixC);
            AES_MyRearrangeRows(matrixC);
            AES_rearrangeColumns(matrixC);
            AES_transformMatrix4W(innerCounter, matrixW);
            AES_rearrangeColumns(matrixW);
            AES_combineMatrixValues(matrixC, matrixW);
            innerCounter--;
        }
        AES_MyTransformBytes(matrixC);
        AES_MyRearrangeRows(matrixC);
        AES_MyAddRoundKey(matrixC, 0);
        AES_MyConvertArrayStr(matrixC, paramOut + outerCounter);
        outerCounter += 16;
    }
}


#endif


