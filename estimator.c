#include "estimator.h"

uint8_t tinysync_constraint_t_compare(tinysync_constraint_t p_1, tinysync_constraint_t p_2){
    return (p_1.t_1 == p_2.t_1 &&
            p_1.t_2 == p_2.t_2) ? 1 : 0;
}

void tinysync_est_state_t_initialize(tinysync_est_state_t * state){
    state->init_counter = 0;
}

void calculate_line(tinysync_constraint_t p_1, tinysync_constraint_t p_2, tinysync_line_t * line){
    line->a = (double)(p_2.t_1 - p_1.t_1) / (double)(p_2.t_2 - p_1.t_2);
    line->b = (double)(p_1.t_1) - line->a * (double)(p_1.t_2);
}

//estimator of linear clock drift and offset
tinysync_est_ret_t tinysync_est_etimate(tinysync_est_state_t * state, const tinysync_datapoint_t * new_datapoint){
    tinysync_est_ret_t ret = TINYSYNC_EST_OK;

    // default to keeping old points.
    // These new variables should only be written, not read, until after constraint selection
    tinysync_constraint_t a_1 = state->constraints.a_1;
    tinysync_constraint_t b_1 = state->constraints.b_1;
    tinysync_constraint_t a_2 = state->constraints.a_2;
    tinysync_constraint_t b_2 = state->constraints.b_2;

    // Convert probing datapoint to 2 points in the t_1-t_2 plane.
    tinysync_constraint_t a_3;
    tinysync_constraint_t b_3;
    a_3.t_1 = new_datapoint->t_r;
    a_3.t_2 = new_datapoint->t_b;
    b_3.t_1 = new_datapoint->t_o;
    b_3.t_2 = new_datapoint->t_b;

    // Handle constraint selection
    switch(state->init_counter){
    case 0: // First run: 1 data point, cannot estimate: directly to state and early return
        state->constraints.a_1 = a_3;
        state->constraints.b_1 = b_3;
        state->init_counter ++;
        return TINYSYNC_EST_FIRST;
    break;
    case 1: // Second run: 2 data points, can estimate
        a_2 = a_3;
        b_2 = b_3;
        state->init_counter ++;
        ret = TINYSYNC_EST_SECOND;
        break;
    default: // Algorithm initialized, normal behaviour

        // Handle new lower constraint (b_3)
        if        (b_3.t_1 < state->lineset.ab.b + state->lineset.ab.a * (double)(b_3.t_2)){
            // Lower constraint too low to be useful: do nothing
        } else if (b_3.t_1 > state->lineset.ba.b + state->lineset.ba.a * (double)(b_3.t_2)){
            // Lower constraint too high to maintain linear model: remove old constraints and add new.
            a_1 = state->constraints.a_2;
            b_1 = state->constraints.b_2;
            a_2 = a_3;
            b_2 = b_3;
            ret = TINYSYNC_EST_NONLINEAR;
            break;
        } else {
            // Lower constraint useful: update b_2
            b_2 = b_3;
            if ( (!tinysync_constraint_t_compare(state->constraints.a_1,state->constraints.a_2)) && (b_3.t_1 > state->lineset.aa.b + state->lineset.aa.a * (double)(b_3.t_2)) ){
                // a_2 is better than a_1 for this lower constraint: update a_1
                a_1 = state->constraints.a_2;
            }
        }

        // Handle new upper constraint (a_3)
        if        (a_3.t_1 > state->lineset.ba.b + state->lineset.ba.a * (double)(a_3.t_2)){
            // Upper constraint too high to be useful: do nothing
        } else if (a_3.t_1 < state->lineset.ab.b + state->lineset.ab.a * (double)(a_3.t_2)){
            // Upper constraint too low to maintain linear model: remove old constraints and add new.
            a_1 = state->constraints.a_2;
            b_1 = state->constraints.b_2;
            a_2 = a_3;
            b_2 = b_3;
            ret = TINYSYNC_EST_NONLINEAR;
            break;
        } else {
            // Upper constraint useful: update a_2
            a_2 = a_3;
            if ( (!tinysync_constraint_t_compare(state->constraints.b_1,state->constraints.b_2)) && (a_3.t_1 < state->lineset.bb.b + state->lineset.bb.a * (double)(a_3.t_2)) ){
                // b_2 is better than b_1 for this upper constraint: update b_1
                b_1 = state->constraints.b_2;
            }
        }
    }

    // Update stored points
    state->constraints.a_1 = a_1;
    state->constraints.b_1 = b_1;
    state->constraints.a_2 = a_2;
    state->constraints.b_2 = b_2;

    // Update lines
    calculate_line(a_1, a_2, &(state->lineset.aa));
    calculate_line(a_1, b_2, &(state->lineset.ab));
    calculate_line(b_1, a_2, &(state->lineset.ba));
    calculate_line(b_1, b_2, &(state->lineset.bb));

    return ret;
};