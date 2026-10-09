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

#ifndef AUTOLIBC_H_
#define AUTOLIBC_H_

/**************************************************************************
                    FUNCTION DECLARATION
**************************************************************************/
/** memory content copy function */
void *Momory_Copy_Function(void *des, const void *source, uint32_t dataLong);

/** Set memory seed */
void Memory_Set_Seed(uint32_t num);

/** fill a memory as dataLong */
void *Momory_Fill_Function(void *des, uint8_t contentByte, uint32_t dataLong);

/** Memory Generate Next Value */
uint32_t Memory_Generate_Next_Value(void);

#endif
