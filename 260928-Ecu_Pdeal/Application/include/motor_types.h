#ifndef MOTOR_TYPES_H
#define MOTOR_TYPES_H


#ifdef __cplusplus
extern "C" {
#endif



/*************** 电机转向 ***************/
typedef enum
{

	MOTOR_DIR_STOP  = 0,

	MOTOR_DIR_CCW   = 1,

	MOTOR_DIR_CW    = 2,

	MOTOR_DIR_BRAKE = 3

} MotorDir_t;






/*************** 电机实例 ID ***************/
typedef enum
{

	MOTOR_INST_L = 0,

	MOTOR_INST_R = 1,

	MOTOR_INST_MAX

} MotorInstId_t;





/*************** pwm_comp 满量程 ***************/
#define MOTOR_PWM_MAX		(1000u - 1u)





#ifdef __cplusplus

}

#endif



#endif /* MOTOR_TYPES_H */

