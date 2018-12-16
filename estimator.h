#ifndef __TINYSYNC_ESTIMATOR_H__
#define __TINYSYNC_ESTIMATOR_H__

#include <stdint.h>

typedef enum tinysync_est_ret_t{
    TINYSYNC_EST_OK = 0,
    TINYSYNC_EST_FIRST = 1,
    TINYSYNC_EST_SECOND = 2,
    TINYSYNC_EST_NONLINEAR = 3,
    TINYSYNC_EST_INVALID = 4 // Should never occur
} tinysync_est_ret_t;

typedef struct tinysync_datapoint_t{
    uint64_t t_o; // t_1 when probe was sent (+ minimum known delay until response transmission)
    uint64_t t_b; // t_2 when response was sent
    uint64_t t_r; // t_1 when response was received
} tinysync_datapoint_t;

typedef struct tinysync_constraint_t{
    uint64_t t_1; // prober clock time
    uint64_t t_2; // responder clock time
} tinysync_constraint_t;

// Line t_1 = a(t_2) + b
typedef struct tinysync_line_t{
    double a; // slope of line
    double b; // offset of line
} tinysync_line_t;

// Stored set of constraints
typedef struct tinysync_constraints_t{
    tinysync_constraint_t b_1; // Constraint B_1
    tinysync_constraint_t b_2; // Constraint B_1
    tinysync_constraint_t a_1; // Constraint A_1
    tinysync_constraint_t a_2; // Constraint A_1
} tinysync_constraints_t;

// Stored set of 4 lines between 4 constraints
typedef struct tinysync_lineset_t{
    tinysync_line_t ba; // Line B_1 A_2
    tinysync_line_t ab; // Line A_1 B_2
    tinysync_line_t aa; // Line A_1 A_2
    tinysync_line_t bb; // Line B_1 B_2
} tinysync_lineset_t;

typedef struct tinysync_est_state_t{
    tinysync_constraints_t constraints;
    tinysync_lineset_t lineset;
    uint8_t init_counter;
} tinysync_est_state_t;

void tinysync_est_state_t_initialize(tinysync_est_state_t* state);
tinysync_est_ret_t tinysync_est_etimate(tinysync_est_state_t* state, 
                                        const tinysync_datapoint_t* new_datapoint);

#endif //__TINYSYNC_ESTIMATOR_H__