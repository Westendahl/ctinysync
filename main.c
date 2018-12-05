// main.c for testing library
#include <stdint.h>
#include "estimator.h"
#include <stdio.h>
int main(){
    printf("%s\n",__BASE_FILE__);
    tinysync_est_state_t state;
    tinysync_est_state_t_initialize(&state);
    tinysync_datapoint_t d1;
    d1.t_o = 1;
    d1.t_b = 2;
    d1.t_r = 3;
    tinysync_est_ret_t ret = tinysync_est_etimate(&state, &d1);
    printf("tiny sync test:%u %f %f %f %f\n",ret, state.lineset.ab.a, state.lineset.ba.a, state.lineset.ab.b, state.lineset.ba.b);
    d1.t_o = 4;
    d1.t_b = 5;
    d1.t_r = 6;
    ret = tinysync_est_etimate(&state, &d1);
    printf("tiny sync test:%u %f %f %f %f\n",ret, state.lineset.ab.a, state.lineset.ba.a, state.lineset.ab.b, state.lineset.ba.b);
    d1.t_o = 7;
    d1.t_b = 8;
    d1.t_r = 9;
    ret = tinysync_est_etimate(&state, &d1);
    printf("tiny sync test:%u %f %f %f %f\n",ret, state.lineset.ab.a, state.lineset.ba.a, state.lineset.ab.b, state.lineset.ba.b);
    d1.t_o = 10;
    d1.t_b = 11;
    d1.t_r = 12;
    ret = tinysync_est_etimate(&state, &d1);
    printf("tiny sync test:%u %f %f %f %f\n",ret, state.lineset.ab.a, state.lineset.ba.a, state.lineset.ab.b, state.lineset.ba.b);
    d1.t_o = 13;
    d1.t_b = 14;
    d1.t_r = 15;
    ret = tinysync_est_etimate(&state, &d1);
    printf("tiny sync test:%u %f %f %f %f\n",ret, state.lineset.ab.a, state.lineset.ba.a, state.lineset.ab.b, state.lineset.ba.b);
    
    while(1){}

    return 0;
}
