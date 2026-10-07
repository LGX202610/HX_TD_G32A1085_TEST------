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

#include "ZLGKey.h"

/**************************************************************************
                    GLOBAL FUNCTION
**************************************************************************/
#ifdef ALLOW_ZLG_ZXDOC_SA_ALGORITHM
/*!
* @brief          This function modifies key data based on input buffer.
*
* @param[in]      pInBuf     --input buffer
                  iLen       --buffer length
                  pOutBuf    --output buffer
* @param[out]     None
* @param[in,out]  None
*
* @retval         None
*/
void ZLG_MyModifiedKey(sint8 *pInBuf, sint32 iLen, sint8 *pOutBuf)
{
    /* Local loop counter */
    sint32 lCounter = 0;

    /* Reverse original if condition */
    if ((iLen != 0) && ((iLen % 16) == 0))
    {
        /* Convert for loop to while loop */
        lCounter = 0;
        while (lCounter < iLen)
        {
            pOutBuf[lCounter] = pInBuf[lCounter] - 1;
            lCounter++;
        }
    }
    else
    {
        return;
    }
}
#endif


