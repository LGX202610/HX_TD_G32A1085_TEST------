#ifndef HAL_DRV8718_H
#define HAL_DRV8718_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include "stdint.h"
#include "stdbool.h"


/* Define */
#define BDC_DRVIER_WRITE_REG 		0
#define BDC_DRVIER_READ_REG 		1



/* Enum */ 
typedef enum {
	IC_STAT1,       	// 00h IC_STAT1 	全局故障和警告指示的状态寄存器。
	VDS_STAT1,        // 01h  					半桥1-4的特定MOSFET VDS过电流故障指示状态寄存器。
	VDS_STAT2,				// 02							半桥5-8的特定MOSFET VDS过电流故障指示的状态寄存器
	VGS_STAT1,				// 03h						半桥1-4的特定MOSFET VGS栅极故障指示的状态寄存器
	VGS_STAT2,				// 04h						半桥5-8的特定MOSFET VGS栅极故障指示的状态寄存器
	IC_STAT2,					// 05h						用于特定欠压、过压、过热和接口故障指示的状态寄存器。
	IC_STAT3,					//06h							具有DRV8718-Q1或DRV8714-Q1的设备ID的状态寄存器。
	IC_CTRL1,					//07h							用于驱动器和诊断启用、SPI锁定和清除故障命令的控制寄存器
	IC_CTRL2,					//08h							引脚模式、电荷泵模式和看门狗的控制寄存器。
	BRG_CTRL1,				//09h							控制寄存器，用于设置半桥1-4的输出状态
	BRG_CTRL2,				//0ah							控制寄存器，用于设置半桥5-8的输出状态
	PWM_CTRL1,				//0bh							用于映射半桥1-4的输入PWM源的控制寄存器
	PWM_CTRL2,				//0ch							控制寄存器，用于映射半桥5-8的输入PWM源。
	PWM_CTRL3,				//0dh							控制寄存器，用于设置半桥1-8的PWM驱动MOSFET（高或低）
	PWM_CTRL4,				//0eh							控制寄存器，用于设置半桥1-8的PWM续流模式    0 内部生成反向PW    1续流二极管				默认0
	IDRV_CTRL1,				//0fh							控制寄存器，用于配置半桥1高侧和低侧栅极驱动器的源极和漏极电流。
	IDRV_CTRL2,				//10h							控制寄存器，用于配置半桥2高侧和低侧栅极驱动器的源极和漏极电流。
	IDRV_CTRL3,				//11h							控制寄存器，用于配置半桥3高侧和低侧栅极驱动器的源极和漏极电流。
	IDRV_CTRL4,				//12h							控制寄存器，用于配置半桥4高侧和低侧栅极驱动器的源极和漏极电流。
	IDRV_CTRL5,				//13h							控制寄存器，用于配置半桥5高侧和低侧栅极驱动器的源极和漏极电流。	
	IDRV_CTRL6,				//14h							控制寄存器，用于配置半桥6高侧和低侧栅极驱动器的源极和漏极电流。
	IDRV_CTRL7,				//15h							控制寄存器，用于配置半桥7高侧和低侧栅极驱动器的源极和漏极电流。
	IDRV_CTRL8,				//16h							控制寄存器，用于配置半桥8高侧和低侧栅极驱动器的源极和漏极电流。
	IDRV_CTRL9,				//17h							控制寄存器，以启用半桥1-8的超低源和汇电流设置。				0 标准值      1 低驱动电流    默认0
	DRV_CTRL1,				//18h 						控制寄存器，用于设置VGS和VDS监视器的操作模式和配置
	DRV_CTRL2,				//19h							控制寄存器，用于设置半桥1-4的tDRV、VGS驱动器和VDS监视器消隐时间。
	DRV_CTRL3,				//1ah							控制寄存器，用于设置半桥5-8的tDRV、VGS驱动器和VDS监视器消隐时间。
	DRV_CTRL4,				//1bh							控制寄存器设置VGS 死区时间tDEAD_D，用于半桥1-8的额外数字死区插入。
	DRV_CTRL5,				//1ch							控制寄存器，用于设置半桥1-8的VDS tDS_DG、过电流监视器的断电时间。
	DRV_CTRL6,				//1dh							控制寄存器，用于设置栅极下拉电流（IDRVN），以响应半桥的VDS过电流故障1-8			
	DRV_CTRL7,				//1eh							
	VDS_CTRL1,				//1fh							控制寄存器，用于设置半桥1和2的VDS过电流监视器电压阈值。					电压和内阻换算后可以作为保护电流
	VDS_CTRL2,				//20h							控制寄存器，用于设置半桥3和4的VDS过电流监视器电压阈值。					
	VDS_CTRL3,				//21h							控制寄存器，用于设置半桥5和6的VDS过电流监视器电压阈值。
	VDS_CTRL4,				//22h							控制寄存器，用于设置半桥7和8的VDS过电流监视器电压阈值。
	OLSC_CTRL1,				//23h							控制寄存器，用于启用和禁用半桥1-4的离线诊断电流源。
	OLSC_CTRL2,				//24h							控制寄存器，用于启用和禁用半桥5-8的离线诊断电流源。
	UVOV_CTRL,				//25h							控制寄存器，用于设置欠压和过压监测器配置。
	CSA_CTRL1,				//26h							分流放大器1和2的增益和参考电压控制寄存器
	CSA_CTRL2,				//27h							分流放大器1消隐配置的控制寄存器。
	CSA_CTRL3,				//28h							分流放大器2消隐配置的控制寄存器。
	RSVD_CTRL,				//29h							
	AGD_CTRL1,				//2ah							用于自适应栅极驱动电压阈值、下拉设置和有源半桥配置的控制寄存器。
	PDR_CTRL1,				//2bh							用于半桥1和2的tON_OFF传播延迟和预充电/放电最大电流的控制寄存器
	PDR_CTRL2,				//2ch							用于半桥3和4的tON_OFF传播延迟和预充电/放电最大电流的控制寄存器
	PDR_CTRL3,				//2dh							用于半桥5和6的tON_OFF传播延迟和预充电/放电最大电流的控制寄存器
	PDR_CTRL4,				//2eh							用于半桥7和8的tON_OFF传播延迟和预充电/放电最大电流的控制寄存器
	PDR_CTRL5,				//2fh							半桥1和2的充电和预充电初始设置控制寄存器
	PDR_CTRL6,				//30h							半桥3和4的充电和预充电初始设置控制寄存器
	PDR_CTRL7,				//31h							半桥5和6的充电和预充电初始设置控制寄存器	
	PDR_CTRL8,				//32h							半桥7和8的充电和预充电初始设置控制寄存器
	PDR_CTRL9,				//33h							控制寄存器，用于配置半桥1-4的PDR Kp环路控制器增益设置。
	PDR_CTRL10,				//34h							控制寄存器，用于配置半桥5-8的PDR Kp环路控制器增益设置。
	STC_CTRL1,				//35h							控制寄存器，用于配置半桥1和2的STC上升/下降时间和Kp环路控制器增益设置。
	STC_CTRL2,				//36h							控制寄存器，用于配置半桥3和4的STC上升/下降时间和Kp环路控制器增益设置。	
	STC_CTRL3,				//37h							控制寄存器，用于配置半桥5和6的STC上升/下降时间和Kp环路控制器增益设置。
	STC_CTRL4,				//38h							控制寄存器，用于配置半桥7和8的STC上升/下降时间和Kp环路控制器增益设置。
	DCC_CTRL1,				//39h							控制寄存器以启用DCC环路和半桥1-8的手动配置。
	PST_CTRL1,				//3ah							控制寄存器，用于配置半桥1-8的最大续流电流和充电后延迟。
	PST_CTRL2,				//3bh							控制寄存器，用于配置半桥1-8的充电后Kp环路控制器增益设置
	SGD_STAT1,				//3ch							指示半桥1-8电流极性的状态寄存器
	SGD_STAT2,				//3dh							指示半桥1-8 PDR环路控制中下溢和溢出的状态寄存器
	SGD_STAT3,				//3eh							半桥1-8的状态寄存器指示器STC上升和下降时间溢出
} DRV8718_REG;				





/* Struct */ 






void hal_drv8718_init(void);
void hal_drv8718_sleep(void);
void hal_drv8718_wakeup(void);
void drv8718_config_init(void);

void get_drv8718_vds(void);
void hal_drv8718_clr_flt(void);
void set_drv8718_nslp_mode(uint8_t mode);
void bdc_driver_transfer(uint8_t write_read, uint8_t addr, uint8_t tx_dat, uint8_t *rx_dat);


#ifdef __cplusplus
}
#endif

#endif /*  */
