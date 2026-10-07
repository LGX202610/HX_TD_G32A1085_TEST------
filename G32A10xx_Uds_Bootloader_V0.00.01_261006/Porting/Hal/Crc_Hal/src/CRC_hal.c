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

#include "CRC_hal.h"

#ifdef EN_CRC_HARDWARE
#include "Common_Hw.h"
#include "crc_cfg.h"
#endif

/**************************************************************************
                    GLOBAL VARIABLE
**************************************************************************/
#ifdef ALLOW_CRC_SW
static const uint16 gs_crc16DnpVal[256u] =
{
    0x0000, 0x365E, 0x6CBC, 0x5AE2, 0xD978, 0xEF26, 0xB5C4, 0x839A, 0xFF89, 0xC9D7, 0x9335, 0xA56B, 
    0x26F1, 0x10AF, 0x4A4D, 0x7C13, 0xB26B, 0x8435, 0xDED7, 0xE889, 0x6B13, 0x5D4D, 0x07AF, 0x31F1,
    0x4DE2, 0x7BBC, 0x215E, 0x1700, 0x949A, 0xA2C4, 0xF826, 0xCE78, 0x29AF, 0x1FF1, 0x4513, 0x734D, 
    0xF0D7, 0xC689, 0x9C6B, 0xAA35, 0xD626, 0xE078, 0xBA9A, 0x8CC4, 0x0F5E, 0x3900, 0x63E2, 0x55BC,
    0x9BC4, 0xAD9A, 0xF778, 0xC126, 0x42BC, 0x74E2, 0x2E00, 0x185E, 0x644D, 0x5213, 0x08F1, 0x3EAF, 
    0xBD35, 0x8B6B, 0xD189, 0xE7D7, 0x535E, 0x6500, 0x3FE2, 0x09BC, 0x8A26, 0xBC78, 0xE69A, 0xD0C4,
    0xACD7, 0x9A89, 0xC06B, 0xF635, 0x75AF, 0x43F1, 0x1913, 0x2F4D, 0xE135, 0xD76B, 0x8D89, 0xBBD7, 
    0x384D, 0x0E13, 0x54F1, 0x62AF, 0x1EBC, 0x28E2, 0x7200, 0x445E, 0xC7C4, 0xF19A, 0xAB78, 0x9D26,
    0x7AF1, 0x4CAF, 0x164D, 0x2013, 0xA389, 0x95D7, 0xCF35, 0xF96B, 0x8578, 0xB326, 0xE9C4, 0xDF9A, 
    0x5C00, 0x6A5E, 0x30BC, 0x06E2, 0xC89A, 0xFEC4, 0xA426, 0x9278, 0x11E2, 0x27BC, 0x7D5E, 0x4B00,
    0x3713, 0x014D, 0x5BAF, 0x6DF1, 0xEE6B, 0xD835, 0x82D7, 0xB489, 0xA6BC, 0x90E2, 0xCA00, 0xFC5E, 
    0x7FC4, 0x499A, 0x1378, 0x2526, 0x5935, 0x6F6B, 0x3589, 0x03D7, 0x804D, 0xB613, 0xECF1, 0xDAAF,
    0x14D7, 0x2289, 0x786B, 0x4E35, 0xCDAF, 0xFBF1, 0xA113, 0x974D, 0xEB5E, 0xDD00, 0x87E2, 0xB1BC, 
    0x3226, 0x0478, 0x5E9A, 0x68C4, 0x8F13, 0xB94D, 0xE3AF, 0xD5F1, 0x566B, 0x6035, 0x3AD7, 0x0C89,
    0x709A, 0x46C4, 0x1C26, 0x2A78, 0xA9E2, 0x9FBC, 0xC55E, 0xF300, 0x3D78, 0x0B26, 0x51C4, 0x679A, 
    0xE400, 0xD25E, 0x88BC, 0xBEE2, 0xC2F1, 0xF4AF, 0xAE4D, 0x9813, 0x1B89, 0x2DD7, 0x7735, 0x416B,
    0xF5E2, 0xC3BC, 0x995E, 0xAF00, 0x2C9A, 0x1AC4, 0x4026, 0x7678, 0x0A6B, 0x3C35, 0x66D7, 0x5089, 
    0xD313, 0xE54D, 0xBFAF, 0x89F1, 0x4789, 0x71D7, 0x2B35, 0x1D6B, 0x9EF1, 0xA8AF, 0xF24D, 0xC413,
    0xB800, 0x8E5E, 0xD4BC, 0xE2E2, 0x6178, 0x5726, 0x0DC4, 0x3B9A, 0xDC4D, 0xEA13, 0xB0F1, 0x86AF, 
    0x0535, 0x336B, 0x6989, 0x5FD7, 0x23C4, 0x159A, 0x4F78, 0x7926, 0xFABC, 0xCCE2, 0x9600, 0xA05E,
    0x6E26, 0x5878, 0x029A, 0x34C4, 0xB75E, 0x8100, 0xDBE2, 0xEDBC, 0x91AF, 0xA7F1, 0xFD13, 0xCB4D, 
    0x48D7, 0x7E89, 0x246B, 0x1235,
};

// ==================== CRC32查表法（快速） ====================
// 生成CRC32查找表（以太网/802.3标准，多项式0x04C11DB7） lu-
static const uint32_t crc32_table[256] = {
    0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA,
    0x076DC419, 0x706AF48F, 0xE963A535, 0x9E6495A3,
    0x0EDB8832, 0x79DCB8A4, 0xE0D5E91E, 0x97D2D988,
    0x09B64C2B, 0x7EB17CBD, 0xE7B82D07, 0x90BF1D91,
    0x1DB71064, 0x6AB020F2, 0xF3B97148, 0x84BE41DE,
    0x1ADAD47D, 0x6DDDE4EB, 0xF4D4B551, 0x83D385C7,
    0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC,
    0x14015C4F, 0x63066CD9, 0xFA0F3D63, 0x8D080DF5,
    0x3B6E20C8, 0x4C69105E, 0xD56041E4, 0xA2677172,
    0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B,
    0x35B5A8FA, 0x42B2986C, 0xDBBBC9D6, 0xACBCF940,
    0x32D86CE3, 0x45DF5C75, 0xDCD60DCF, 0xABD13D59,
    0x26D930AC, 0x51DE003A, 0xC8D75180, 0xBFD06116,
    0x21B4F4B5, 0x56B3C423, 0xCFBA9599, 0xB8BDA50F,
    0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924,
    0x2F6F7C87, 0x58684C11, 0xC1611DAB, 0xB6662D3D,
    0x76DC4190, 0x01DB7106, 0x98D220BC, 0xEFD5102A,
    0x71B18589, 0x06B6B51F, 0x9FBFE4A5, 0xE8B8D433,
    0x7807C9A2, 0x0F00F934, 0x9609A88E, 0xE10E9818,
    0x7F6A0DBB, 0x086D3D2D, 0x91646C97, 0xE6635C01,
    0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E,
    0x6C0695ED, 0x1B01A57B, 0x8208F4C1, 0xF50FC457,
    0x65B0D9C6, 0x12B7E950, 0x8BBEB8EA, 0xFCB9887C,
    0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3, 0xFBD44C65,
    0x4DB26158, 0x3AB551CE, 0xA3BC0074, 0xD4BB30E2,
    0x4ADFA541, 0x3DD895D7, 0xA4D1C46D, 0xD3D6F4FB,
    0x4369E96A, 0x346ED9FC, 0xAD678846, 0xDA60B8D0,
    0x44042D73, 0x33031DE5, 0xAA0A4C5F, 0xDD0D7CC9,
    0x5005713C, 0x270241AA, 0xBE0B1010, 0xC90C2086,
    0x5768B525, 0x206F85B3, 0xB966D409, 0xCE61E49F,
    0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4,
    0x59B33D17, 0x2EB40D81, 0xB7BD5C3B, 0xC0BA6CAD,
    0xEDB88320, 0x9ABFB3B6, 0x03B6E20C, 0x74B1D29A,
    0xEAD54739, 0x9DD277AF, 0x04DB2615, 0x73DC1683,
    0xE3630B12, 0x94643B84, 0x0D6D6A3E, 0x7A6A5AA8,
    0xE40ECF0B, 0x9309FF9D, 0x0A00AE27, 0x7D079EB1,
    0xF00F9344, 0x8708A3D2, 0x1E01F268, 0x6906C2FE,
    0xF762575D, 0x806567CB, 0x196C3671, 0x6E6B06E7,
    0xFED41B76, 0x89D32BE0, 0x10DA7A5A, 0x67DD4ACC,
    0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5,
    0xD6D6A3E8, 0xA1D1937E, 0x38D8C2C4, 0x4FDFF252,
    0xD1BB67F1, 0xA6BC5767, 0x3FB506DD, 0x48B2364B,
    0xD80D2BDA, 0xAF0A1B4C, 0x36034AF6, 0x41047A60,
    0xDF60EFC3, 0xA867DF55, 0x316E8EEF, 0x4669BE79,
    0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236,
    0xCC0C7795, 0xBB0B4703, 0x220216B9, 0x5505262F,
    0xC5BA3BBE, 0xB2BD0B28, 0x2BB45A92, 0x5CB36A04,
    0xC2D7FFA7, 0xB5D0CF31, 0x2CD99E8B, 0x5BDEAE1D,
    0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x026D930A,
    0x9C0906A9, 0xEB0E363F, 0x72076785, 0x05005713,
    0x95BF4A82, 0xE2B87A14, 0x7BB12BAE, 0x0CB61B38,
    0x92D28E9B, 0xE5D5BE0D, 0x7CDCEFB7, 0x0BDBDF21,
    0x86D3D2D4, 0xF1D4E242, 0x68DDB3F8, 0x1FDA836E,
    0x81BE16CD, 0xF6B9265B, 0x6FB077E1, 0x18B74777,
    0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C,
    0x8F659EFF, 0xF862AE69, 0x616BFFD3, 0x166CCF45,
    0xA00AE278, 0xD70DD2EE, 0x4E048354, 0x3903B3C2,
    0xA7672661, 0xD06016F7, 0x4969474D, 0x3E6E77DB,
    0xAED16A4A, 0xD9D65ADC, 0x40DF0B66, 0x37D83BF0,
    0xA9BCAE53, 0xDEBB9EC5, 0x47B2CF7F, 0x30B5FFE9,
    0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6,
    0xBAD03605, 0xCDD70693, 0x54DE5729, 0x23D967BF,
    0xB3667A2E, 0xC4614AB8, 0x5D681B02, 0x2A6F2B94,
    0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 0x2D02EF8D,
};

/*******************************************************************************
                               LOCAL FUNCTIONS
*******************************************************************************/
/*!
* @brief    Using software lookup table to create CRC.
*/
static void CRC_HAL_GenerateSwCrc16(const uint8 *pDataBuffer, const uint32 dataLength, uint32 *pCurrentCrc)
{
    uint16 tempCrcVal = 0u;
    uint32 loopIndex = 0u;

#if (defined FALSH_MEMORY_CONTINUE) && (FALSH_MEMORY_CONTINUE == 0u)
    tempCrcVal = (uint16)(*pCurrentCrc);
#endif

    while (loopIndex < dataLength)
    {
        tempCrcVal = (uint16)((tempCrcVal >> 8) ^ gs_crc16DnpVal[(tempCrcVal ^ pDataBuffer[loopIndex]) & 0x00FF]);

        loopIndex++;
    }

    *pCurrentCrc = (uint32)((~tempCrcVal) & 0xFFFFu);
}

// ==================== CRC32计算函数 lu-====================
static void CRC32_GenerateSwCrc32(const uint8_t* pDataBuffer, uint32_t dataLength, uint32_t* pCurrentCrc)
{
     //uint32_t tempCrcVal = *pCurrentCrc;
     uint32_t tempCrcVal = 0xFFFFFFFFu;
    uint32_t loopIndex = 0u;

    while (loopIndex < dataLength)
    {
        tempCrcVal = crc32_table[(tempCrcVal ^ pDataBuffer[loopIndex]) & 0xFFu] ^ (tempCrcVal >> 8);
        loopIndex++;
    }

    *pCurrentCrc = tempCrcVal ^ 0xFFFFFFFFu;  // 结果异或
}
#endif

#ifdef EN_CRC_HARDWARE
/*!
* @brief    Using MCU hardware to create CRC
*/
static void CRC_HAL_GenerateHwCrc16(const uint8_t *pBuffer, const uint32_t bufferSize, uint32_t *pCrcValue)
{
    uint32_t tempCrcResult = 0u;

#ifdef FALSH_MEMORY_CONTINUE
#if (FALSH_MEMORY_CONTINUE == TRUE)
    crc1_UserConfig0.seed = *pCrcValue;
#endif
#endif
    CRC_DRV_Init(INST_CRC1, &crc1_UserConfig0);
    CRC_DRV_WriteData(INST_CRC1, pBuffer, bufferSize);
    tempCrcResult = CRC_DRV_GetCrcResult(INST_CRC1);
    *pCrcValue = tempCrcResult;
}
#endif

/*******************************************************************************
                               GLOBAL FUNCTIONS
*******************************************************************************/
/*!
* @brief    This function init this module.
*/
boolean CRC_HAL_Init(void)
{
#ifdef EN_CRC_HARDWARE
#ifdef USING_HARDWARE_CRC
    PCM->CRCCLK.reg |= PCC_PCCn_CGC_MASK;
    CRC_DRV_Init(INST_CRC1, &crc1_InitConfig0);
#endif
#endif
    return TRUE;
}

/*!
* @brief    This function use MCU hardware to create CRC.
*/
void CRC_HAL_CreatHw(const uint8 *dataBuf, const uint32 dataLen, uint32 *curCrc)
{
#ifdef EN_CRC_HARDWARE
    CRC_HAL_GenerateHwCrc16(dataBuf, dataLen, curCrc);
#elif (defined ALLOW_CRC_SW)
    //CRC_HAL_GenerateSwCrc16(dataBuf, dataLen, curCrc);
    CRC32_GenerateSwCrc32(dataBuf, dataLen, curCrc);
#else
    #error "Non CRC module enabled!"
#endif
}

/*!
* @brief    This function use software lookup table or calculate to create CRC.
*/
void CRC_HAL_CreatSw(const uint8 *dataBuf, const uint32 dataLen, uint32 *curCrc)
{
#ifdef ALLOW_CRC_SW
    //CRC_HAL_GenerateSwCrc16(dataBuf, dataLen, curCrc);
    CRC32_GenerateSwCrc32(dataBuf, dataLen, curCrc);
#endif
}

// ==================== CRC32计算函数 lu-====================
uint32_t crc32_calc(const uint8_t* data, uint32_t len)
{
    uint32_t crc = 0xFFFFFFFF;
    
    for (uint32_t i = 0; i < len; i++) {
        crc = crc32_table[(crc ^ data[i]) & 0xFF] ^ (crc >> 8);
    }
    
    return crc ^ 0xFFFFFFFF;
}
// ==================== 测试函数 ====================
void crc32_test(void)
{
    const uint8_t test_data[] = "123456789";
	uint32_t crc=0xFFFFFFFF;
	CRC32_GenerateSwCrc32(test_data, 9 , &crc);
    //crc = crc32_calc(test_data, 9);
    
    if (crc == 0xCBF43926) {
        printf("CRC32 OK! 0x%08X\n", crc);
    } else {
        printf("CRC32 fail! 0x%08X\n", crc);
    }
}

