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

#include "common_types.h"
#include "toolchain.h"

#ifdef AUTOLIB_TESTING
    #include "addresschecker.h"
#endif

#include "autolibc.h"

/**************************************************************************
                    GLOBAL VARIABLE
**************************************************************************/
/** A Random value used for seed */
static uint32_t justANumber = 0x12345678U;

/**************************************************************************
                    GLOBAL FUNCTION
**************************************************************************/
/*!
* @brief         memory content copy function.
*
* @param[in]     None
* @param[out]    None
* @param[in,out] None
*
* @retval        None
*/
void *Momory_Copy_Function(void *des, const void *source, uint32_t dataLong)
{
    uint8_t addrOffset = 0u;
    uint32_t count = 0U;
    uint32_t targetCount = 0U;

    uint8_t *desAddr_8 = (uint8_t *) des;
    uint16_t *desAddr_16 = NULL;
    uint32_t *desAddr_32 = NULL;

    const uint8_t *sourceAddr_8 = (const uint8_t *) source;
    const uint16_t *sourceAddr_16 = NULL;
    const uint32_t *sourceAddr_32 = NULL;

#ifdef AUTOLIB_TESTING
    /** verify the memory content */
    CA_SetTest( 2U, 1U );
    CA_SetRange1( (AddrType)des, dataLong, 1U);
    CA_SetRange2( (AddrType)source, dataLong, 0U);
#endif

    if(dataLong >= 9U)
    {
        count = 0U;
        addrOffset = (uint8_t) ((AddrType) des - (AddrType) source);

        /** verify the last 1 bit whether is 0 or not */
        if ((addrOffset & 1U) == 0U)
        {
            if ((((AddrType)(&desAddr_8[count])) & 1U) != 0U)
            {
#ifdef AUTOLIB_TESTING
                CA_Check2( &desAddr_8[count], &sourceAddr_8[count], sizeof(*desAddr_8) );
#endif
                desAddr_8[count] = sourceAddr_8[count]; /** copy the front unaligned part */
                count = count + 1;
            }

            /** copy 2 Byte aligned part of other data */
            desAddr_16 = (uint16_t *) (&desAddr_8[count]);
            sourceAddr_16 = (const uint16_t *) (&sourceAddr_8[count]);
            dataLong = dataLong - count;
            targetCount = dataLong >> 1U;

            count = 0U;
            while(count < targetCount)
            {
#ifdef AUTOLIB_TESTING
                CA_Check2( (uint8_t *)&desAddr_16[count], (const uint8_t *)&sourceAddr_16[count], sizeof(*desAddr_16) );
#endif
                desAddr_16[count] = sourceAddr_16[count];
                count = count + 1;
            }

            /** copy the other unaligned byte */
            desAddr_8 = (uint8_t *) (&desAddr_16[count]);
            sourceAddr_8 = (const uint8_t *) (&sourceAddr_16[count]);
            dataLong = dataLong - (count * 2);

            count = 0U;
            while(count < dataLong)
            {
#ifdef AUTOLIB_TESTING
                CA_Check2( (uint8_t *)&desAddr_8[count], (const uint8_t *)&sourceAddr_8[count], sizeof(*desAddr_8) );
#endif
                desAddr_8[count] = sourceAddr_8[count];
                count = count + 1;
            }
        }
        else if ((addrOffset & 3U) == 0U) /** verify the last 2 bits are zero */
        {
            for ( ; ((((AddrType) (&desAddr_8[count])) & 3U) != 0U); count++)
            {
#ifdef AUTOLIB_TESTING
                CA_Check2( &desAddr_8[count], &sourceAddr_8[count], sizeof(*desAddr_8) );
#endif
                desAddr_8[count] = sourceAddr_8[count]; /** copy the front unaligned part */
            }

            desAddr_32 = (uint32_t *) (&desAddr_8[count]);
            sourceAddr_32 = (const uint32_t *) (&sourceAddr_8[count]);
            dataLong = dataLong - count;
            targetCount = dataLong >> 2U;

            count = 0U;
            while(count < targetCount)
            {
#ifdef AUTOLIB_TESTING
                CA_Check2( (uint8_t *)&desAddr_32[count], (const uint8_t *)&sourceAddr_32[count], sizeof(*desAddr_32) );
#endif
                desAddr_32[count] = sourceAddr_32[count];
                count = count + 1;
            }

            /** copy the rear unaligned bytes */
            desAddr_8 = (uint8_t *) (&desAddr_32[count]);
            sourceAddr_8 = (const uint8_t *) (&sourceAddr_32[count]);
            dataLong = dataLong - (count * 4);

            count = 0U;
            while(count < dataLong)
            {
#ifdef AUTOLIB_TESTING
                CA_Check2( &desAddr_8[count], &sourceAddr_8[count], sizeof(*desAddr_8) );
#endif
                desAddr_8[count] = sourceAddr_8[count];
                count = count + 1;
            }
        }
        else
        {
            for ( ; ((dataLong - count) != 0U) ; count++)
            {
#ifdef AUTOLIB_TESTING
                CA_Check2( &desAddr_8[count], &sourceAddr_8[count], sizeof(*desAddr_8) );
#endif
                desAddr_8[count] = sourceAddr_8[count];
            }
        }
    }
    else /** the data is too short */
    {
        count = 0U;
        while(count < dataLong)
        {
#ifdef AUTOLIB_TESTING
            CA_Check2( &desAddr_8[count], &sourceAddr_8[count], sizeof(*desAddr_8) );
#endif
            desAddr_8[count] = sourceAddr_8[count];
            count = count + 1;
        }
    }

    return des;
}

/*!
* @brief         Set memory seed.
*
* @param[in]     None
* @param[out]    None
* @param[in,out] justANumber
*
* @retval        None
*/
void Memory_Set_Seed(uint32_t num)
{
    justANumber = ((num == 0U) ? (num + 1) : (num)); /** num should not be 0 */
}

/*!
* @brief         fill a memory as dataLong.
*
* @param[in]     None
* @param[out]    None
* @param[in,out] None
*
* @retval        None
*/
void *Momory_Fill_Function(void *des, uint8_t contentByte, uint32_t dataLong)
{
    uint8_t *desAddr_8 = (uint8_t *) des;
    uint32_t *desAddr_32 = NULL;
    uint32_t count = 0u;
    uint32_t targetCount = 0u;
    uint32_t memoryContent = 0u;

#ifdef AUTOLIB_TESTING
    CA_SetTest( 1U, 0U );
    CA_SetRange1( (AddrType)des, dataLong, 1U);
#endif

    if(dataLong >= 7U) 
    {
        for(count = 0U; (0U != ((AddrType) (&desAddr_8[count]) & 3U)); count++)
        {
#ifdef AUTOLIB_TESTING
            CA_Check1( &desAddr_8[count], sizeof(*desAddr_8) );
#endif
            desAddr_8[count] = contentByte; /** set front unaligned bytes */
        }

        desAddr_32 = (uint32_t *) (&desAddr_8[count]);
        dataLong = dataLong - count;
        targetCount = dataLong >> 2U;
        memoryContent = 0x01010101U * contentByte;

        count = 0U;
        while(count < targetCount)
        {
#ifdef AUTOLIB_TESTING
            CA_Check1( (uint8_t *)&desAddr_32[count], sizeof(*desAddr_32) );
#endif
            desAddr_32[count] = memoryContent;
            count = count + 1;
        }

        /** write other unaligned bytes */
        desAddr_8 = (uint8_t *)(&desAddr_32[count]);
        dataLong = dataLong - (count * 4);

        count = 0U;
        while(count < dataLong)
        {
#ifdef AUTOLIB_TESTING
            CA_Check1( &desAddr_8[count], sizeof(*desAddr_8) );
#endif
            desAddr_8[count] = contentByte;
            count = count + 1;
        }
    }
    else /** the data is too short */
    {
        count = 0U;
        while(count < dataLong)
        {
#ifdef AUTOLIB_TESTING
            CA_Check1( &desAddr_8[count], sizeof(*desAddr_8) );
#endif
            desAddr_8[count] = contentByte;
            count = count + 1;
        }
    }

    return des;
}

/*!
* @brief         Memory Generate Next Value
*
* @param[in]     None
* @param[out]    None
* @param[in,out] justANumber
*
* @retval        uint32_t
*/
uint32_t Memory_Generate_Next_Value(void)
{
    return ((justANumber >> 1U) ^ ((0U - (justANumber & 1U)) & 0x80200003U));
}
