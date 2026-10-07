
/* Includes */
#include "hal_drv8718.h"

#include "drv_spi.h"
#include "drv_gpio.h"



/**********************************************************
  * @brief	DRV8718 初始化
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void hal_drv8718_init(void)
{
	drv_bdc_driver_gpio_init();
	set_drv8718_nslp_mode(1);
	
	drv_spi1_init();
	
	drv8718_config_init();
}


/**********************************************************
  * @brief	DRV8718 休眠
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void hal_drv8718_sleep(void)
{
	set_drv8718_nslp_mode(0);
	drv_spi1_sleep();
}


/**********************************************************
  * @brief	DRV8718 唤醒
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void hal_drv8718_wakeup(void)
{
	set_drv8718_nslp_mode(1);
	drv_spi1_wakeup();
}



/**********************************************************
  * @brief	DRV8718 读写
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void bdc_driver_transfer(uint8_t write_read, uint8_t addr, uint8_t tx_dat, uint8_t *rx_dat)
{
	uint8_t txDataBuffer[2] = {0x00,0x00};	

	/* SDI 16位帧：B15=0，B14=W0(0写1读)，B13-B8=地址，B7-B0=数据 */
	/*************** 操作：读写 ***************/
	if(write_read==BDC_DRVIER_WRITE_REG)
	{
		txDataBuffer[1]&=~0x40;					/* 清B14，写寄存器 */
	}
	else if(write_read==BDC_DRVIER_READ_REG)
	{
		txDataBuffer[1]|=0x40;					/* 置B14，读寄存器 */
	}

	/*************** 地址 ***************/
	txDataBuffer[1]|=(addr&0x3f);			/* 高字节放W0+A5~A0，16位MSB先发时先出线 */

	/*************** 参数 ***************/
	txDataBuffer[0]=tx_dat;					/* 低字节放数据D7~D0 */

	/*************** 传输 ***************/
	drv_spi1_rw_data(txDataBuffer, rx_dat, 2);
}


uint8_t bdc_drvier_rx_buf[2] = {0x00, 0x00};
uint8_t vds_stat1_num[2] = {0x00, 0x00};
uint8_t vds_stat2_num[2] = {0x00, 0x00};
uint8_t vgs_stat1_num[2] = {0x00, 0x00};
uint8_t vgs_stat2_num[2] = {0x00, 0x00};
/**********************************************************
  * @brief	DRV8718 初始化配置
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void drv8718_config_init(void)
{
	uint32_t cnt_delay=0;
	/******************** 上电启用离线诊断 ********************/
	bdc_driver_transfer(BDC_DRVIER_WRITE_REG, IC_CTRL1 ,0x46, bdc_drvier_rx_buf);		//EN_DRV=0关驱动，EN_OLSC=1开离线诊断，独立半桥，解锁寄存器
	
	bdc_driver_transfer(BDC_DRVIER_WRITE_REG, OLSC_CTRL1 ,0xFF, bdc_drvier_rx_buf);	//HB1-4上拉/下拉诊断电流源全开
	bdc_driver_transfer(BDC_DRVIER_WRITE_REG, OLSC_CTRL2 ,0xFF, bdc_drvier_rx_buf);	//HB5-8上拉/下拉诊断电流源全开
	
	do{cnt_delay++;}while(cnt_delay>2000);
//	SysTick_Delay_ms(1);															//等诊断稳定后再读，空载可先关掉
	
	/******************** 回读VDS/VGS寄存器 ********************/
	bdc_driver_transfer(BDC_DRVIER_READ_REG, VDS_STAT1, 0, vds_stat1_num);		//读HB1-4各管VDS过流标志
	bdc_driver_transfer(BDC_DRVIER_READ_REG, VDS_STAT2, 0, vds_stat2_num);		//读HB5-8各管VDS过流标志
	bdc_driver_transfer(BDC_DRVIER_READ_REG, VGS_STAT1, 0, vgs_stat1_num);		//读HB1-4各管VGS栅极故障标志
	bdc_driver_transfer(BDC_DRVIER_READ_REG, VGS_STAT2, 0, vgs_stat2_num);		//读HB5-8各管VGS栅极故障标志
	
	
	bdc_driver_transfer(BDC_DRVIER_WRITE_REG, OLSC_CTRL1 ,0x00, bdc_drvier_rx_buf);	//关掉HB1-4诊断电流源
	bdc_driver_transfer(BDC_DRVIER_WRITE_REG, OLSC_CTRL2 ,0x00, bdc_drvier_rx_buf);	//关掉HB5-8诊断电流源
	
	bdc_driver_transfer(BDC_DRVIER_WRITE_REG, IC_CTRL1 ,0x07, bdc_drvier_rx_buf);			//关离线诊断，保持关驱动，CLR_FLT清一次故障
	
	
	
	/******************** 关闭诊断，启用输出 ********************/
	bdc_driver_transfer(BDC_DRVIER_WRITE_REG, BRG_CTRL1 ,0xff, bdc_drvier_rx_buf);				//HB1-4均为11b，跟PWM输入控制
	bdc_driver_transfer(BDC_DRVIER_WRITE_REG, PWM_CTRL1 ,0b00011011, bdc_drvier_rx_buf);	//HB1=IN1，HB2=IN2，HB3=IN3，HB4=IN4
	
//	bdc_driver_transfer(BDC_DRVIER_WRITE_REG, BRG_CTRL2 ,0x00, bdc_drvier_rx_buf);				//HB5-8保持Hi-Z，暂不用
//	bdc_driver_transfer(BDC_DRVIER_WRITE_REG, PWM_CTRL2 ,0b10111011, bdc_drvier_rx_buf);	//HB5=IN3，HB6=IN4，HB7=IN3，HB8=IN4
	
	bdc_driver_transfer(BDC_DRVIER_WRITE_REG, PWM_CTRL3 ,0x00, bdc_drvier_rx_buf);				//HB1-8高边作驱动管，低边作续流管

	bdc_driver_transfer(BDC_DRVIER_WRITE_REG, DRV_CTRL1 ,0x21, bdc_drvier_rx_buf);			//VGS/VDS锁存故障，只关对应半桥；握手监视仍开
	bdc_driver_transfer(BDC_DRVIER_WRITE_REG, DRV_CTRL4 ,0x55, bdc_drvier_rx_buf);			//HB1-8数字死区2us
		
	bdc_driver_transfer(BDC_DRVIER_WRITE_REG, UVOV_CTRL ,0xF6, bdc_drvier_rx_buf);			//PVDD欠压自恢复，关掉PVDD过压检测，VCP欠压自恢复
	bdc_driver_transfer(BDC_DRVIER_WRITE_REG, CSA_CTRL1 ,0x36, bdc_drvier_rx_buf);			//1b两路偏置AREF/8；CSA1增益40V/V，CSA2增益40V/V
	
	
	bdc_driver_transfer(BDC_DRVIER_WRITE_REG, IC_CTRL1 ,0x87, bdc_drvier_rx_buf);			//EN_DRV=1使能驱动，关离线诊断，CLR_FLT清故障
	bdc_driver_transfer(BDC_DRVIER_WRITE_REG, IC_CTRL2 ,0x42, bdc_drvier_rx_buf);			//多功能脚改为nFLT开漏故障输出
	
	get_drv8718_vds();																		//再读VDS/VGS四本状态寄存器
	
	bdc_driver_transfer(BDC_DRVIER_READ_REG, IC_STAT1 ,0, bdc_drvier_rx_buf);			//读全局状态：FAULT/POR/DS_GS/UV/OV
	bdc_driver_transfer(BDC_DRVIER_READ_REG, IC_STAT2 ,0, bdc_drvier_rx_buf);			//读明细：PVDD_UV/OV、VCP_UV、过温、看门狗、SCLK_FLT

//	bdc_driver_transfer(BDC_DRVIER_WRITE_REG, IC_CTRL1 ,0x87, bdc_drvier_rx_buf);		//禁用输出驱动器，启用离线诊断，46开路检测优先级高于86驱动
}




/**********************************************************
  * @brief	DRV8718 回读VDS故障状态
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void get_drv8718_vds(void)
{
	bdc_driver_transfer(BDC_DRVIER_READ_REG, VDS_STAT1, 0, vds_stat1_num);		//VDS_STAT1
	bdc_driver_transfer(BDC_DRVIER_READ_REG, VDS_STAT2, 0, vds_stat2_num);		//VDS_STAT2
	bdc_driver_transfer(BDC_DRVIER_READ_REG, VGS_STAT1, 0, vgs_stat1_num);		//VGS_STAT1
	bdc_driver_transfer(BDC_DRVIER_READ_REG, VGS_STAT2, 0, vgs_stat2_num);		//VGS_STAT2
}



/**********************************************************
  * @brief	DRV8718 清除CLR_FLT标志位
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void hal_drv8718_clr_flt(void)
{
	bdc_driver_transfer(BDC_DRVIER_WRITE_REG, IC_CTRL1 ,0x87, bdc_drvier_rx_buf);			//清除故障
}



/**********************************************************
  * @brief	设置 8718 休眠状态
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void set_drv8718_nslp_mode(uint8_t mode)
{
	if(mode==0)
	{
		drv_set_bdc_nslp_pin_low();
	}
	else
	{
		drv_set_bdc_nslp_pin_high();
	}
}


/**********************************************************
  * @brief	DRV8718 使能通道
  * @param  
  * @retval 
  * @note		
 **********************************************************/
void enable_drv8718_ch(void)
{

}


