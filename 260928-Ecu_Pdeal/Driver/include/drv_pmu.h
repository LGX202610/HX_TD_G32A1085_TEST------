#ifndef DRV_PMU_H
#define DRV_PMU_H

#ifdef __cplusplus
extern "C" {
#endif

void drv_pmu_enter_stop_1(void);
void drv_pmu_enter_stop_2(void);
void drv_pmu_enter_standby(void);
void drv_pmu_restore_clock(void);
void drv_pmu_gpio_sleep(void);

#ifdef __cplusplus
}
#endif

#endif /*  */
