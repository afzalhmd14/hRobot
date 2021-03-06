#include <cstddef>
#include <cstdint>
#include "hFramework.h"

#ifndef DblMotorCtrlClass
#define DblMotorCtrlClass

class DblMotorCtrl
{
  private:
    float threshold;
    float kp_up;
    float ki_up;
    float kd_up;
    float kp_down;
    float ki_down;
    float kd_down;
    float error_saturate;
    float integrator_saturate_up;
    float integrator_saturate_down;

    float error1_integrator;
    float error1_deviator;
    float error1_last;
    float output1;
    float time_last;
    float output_saturate;

  public:
    DblMotorCtrl(float threshold_t, float kp_down_t, float ki_down_t, float kd_down_t, float kp_up_t, float ki_up_t, float kd_up_t, float error_saturate_t,
		 float integrator_saturate_down_t, float integrator_saturate_up_t);
    int update(float error1, float t_time);
    int updateUp(float error1, float t_time);
    int updateDown(float error1, float t_time);
    void makeOutput(float o1);
    void set_pid_valuesUp(float kp_t, float ki_t, float kd_t);
    void set_pid_valuesDown(float kp_t, float ki_t, float kd_t);
    void set_kp(float kp_t);
    void set_ki(float ki_t);
    void set_kd(float kd_t);
    void setErrorSaturate(float error_saturate_t);
    void setOutputSaturate(float output_saturate_t);
    void setThreshold(float threshold_t);
    float getErrorSaturate();
    float getOutputSaturate();
    float getThreshold();
};

#endif
