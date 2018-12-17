// main.c for testing library
#include <stdint.h>
#include "estimator.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define S 1000000L
#define MS 1000L

#define TEST_N 10000L // number of iterations to run test
#define TEST_A12 1.4 // Clock skew
#define TEST_B12 5L*S // Clock offset
#define TEST_INTERVAL 1*S // Test interval, t_2 units
#define TEST_INTERVAL_RAND 100*MS // error magnitude of same
#define TEST_DELAY_B 50*MS // delay from t_o to t_b
#define TEST_DELAY_B_RAND 15*MS // error magnitude of same
#define TEST_DELAY_R 50*MS // delay from t_b to t_r
#define TEST_DELAY_R_RAND 15*MS // error magnitude of same
#define TEST_NONLINEAR_T_2_START 500*S // Time of nonlinearity event
#define TEST_NONLINEAR_A12         1.6 // new A12 after nonlinearity

// Generate a random (uniform) number with a maximum magnitude of 'range'
int64_t random_error(uint64_t range){
    if(range == 0){
        return 0;
    }else{
        uint64_t magnitude = rand() % range;
        int8_t sign = (rand() % 2 ? 1 : -1);
        return sign * magnitude;
    }
}

// Stored clocks of t_1 and t_2, for testing
typedef struct test_moment_t{
    uint64_t t_1; // Current state of node 1's timer
    uint64_t t_2; // Current state of node 2's timer, and true wall time
} test_moment_t;

void test_advance_timestamp(tinysync_datapoint_t* d, test_moment_t* now){
    double test_a12 = (now->t_2 < TEST_NONLINEAR_T_2_START) ? TEST_A12 : 
                                                                 TEST_NONLINEAR_A12;

    int64_t error = random_error(TEST_INTERVAL_RAND);
    now->t_1 += (TEST_INTERVAL + error) * test_a12;
    now->t_2 += (TEST_INTERVAL + error);
    d->t_o = now->t_1;

    error = random_error(TEST_DELAY_B_RAND);
    now->t_1 += (TEST_DELAY_B + error) * test_a12;
    now->t_2 += (TEST_DELAY_B + error);
    d->t_b = now->t_2;

    error = random_error(TEST_DELAY_R_RAND);
    now->t_1 += (TEST_DELAY_R + error) * test_a12;
    now->t_2 += (TEST_DELAY_R + error);
    d->t_r = now->t_1;
}

int main(){
    tinysync_est_state_t state;
    tinysync_est_state_t_initialize(&state); // Set initialization counter to zero

    test_moment_t now = {.t_1 = TEST_B12, // Start off the clocks with an offset
                         .t_2 = 0};
    tinysync_datapoint_t d1;

    for(uint64_t i=0; i<TEST_N; i++){
        test_advance_timestamp(&d1, &now);
        tinysync_est_ret_t ret = tinysync_est_etimate(&state, &d1);
        double b_12_exp = (state.lineset.ba.a + state.lineset.ab.a) / 2.0;
        double a_12_exp = (state.lineset.ba.b + state.lineset.ab.b) / 2.0;
        uint64_t estimated_t_2 = (uint64_t)( (((double)now.t_1) - a_12_exp) / b_12_exp );
        uint64_t max_t_2 = (uint64_t)( (((double)now.t_1) - state.lineset.ba.b) / state.lineset.ba.a );
        uint64_t min_t_2 = (uint64_t)( (((double)now.t_1) - state.lineset.ab.b) / state.lineset.ab.a );
        printf("test %u %.9g %.9g %.9g %.9g %u %.9g %.9g %.9g %.9g %u %d %f %d %d %d\n",
                                       ret, // Return code
                                       state.lineset.ba.a, // drift lower limit
                                       state.lineset.ab.a, // drift upper limit
                                       state.lineset.ba.b, // offset upper limit
                                       state.lineset.ab.b, // offset lower limit
                                       now.t_2,
                                       b_12_exp, // Expected offset
                                       fabs(b_12_exp - TEST_B12), // B12 absolute error
                                       a_12_exp, // Expected skew
                                       fabs(a_12_exp - TEST_A12), // A12 absolute error
                                       estimated_t_2,
                                       estimated_t_2 - now.t_2, // Error of t_2 estimate
                                       fabs((int64_t)estimated_t_2 - (int64_t)now.t_2), // Absolute same
                                       max_t_2 - now.t_2, //Error of max; always positive if done right
                                       min_t_2 - now.t_2, //Error of min; always negative if done right
                                       (max_t_2 - min_t_2) / 2  //Maximum possible absolute error
                                       );
    }
    return 0;
}
