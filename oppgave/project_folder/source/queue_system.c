
#include "queue_system.h"
#include "enums.h"
#include "elevator.h"
#include "driver/channels.h"
#include "driver/io.h"
#include <stdio.h>

#define ORDER_INSIDE 0
#define ORDER_UP 1
#define ORDER_DOWN 2



int check_orders_for_elevator[NUMBER_OF_FLOORS][NUMBER_OF_BUTTONS]={
    {BUTTON_COMMAND1, BUTTON_UP1, BUTTON_DOWN1},
    {BUTTON_COMMAND2, BUTTON_UP2, BUTTON_DOWN2},
    {BUTTON_COMMAND3, BUTTON_UP3, BUTTON_DOWN3},
    {BUTTON_COMMAND4, BUTTON_UP4, BUTTON_DOWN4}
};

int lights_for_order[NUMBER_OF_FLOORS][NUMBER_OF_BUTTONS]={
    {LIGHT_COMMAND1, LIGHT_UP1, LIGHT_DOWN1},
    {LIGHT_COMMAND2, LIGHT_UP2, LIGHT_DOWN2},
    {LIGHT_COMMAND3, LIGHT_UP3, LIGHT_DOWN3},
    {LIGHT_COMMAND4, LIGHT_UP4, LIGHT_DOWN4}
};

void queue_system_check_for_orders(bool **order_state){
    for (int floor=0; floor<NUMBER_OF_FLOORS;floor++){
        for (int j=0; j<NUMBER_OF_BUTTONS; j++){
            if (io_read_bit(check_orders_for_elevator[floor][j])){
                if (j==0){
                    order_state[floor][ORDER_INSIDE]=true;
                }
                else if (j==1){
                    order_state[floor][ORDER_UP]=true;
                }
                else if (j==2){
                    order_state[floor][ORDER_DOWN]=true;
                }
            }
        }
    }
}

bool check_above(bool **order_state, floor_enum current_floor){
    if(current_floor == floor_4){
        return false;
        }
    for(int floor = (current_floor + 1); floor < 4; floor++){
        for(int state = 0; state < 2; state++){
            if(order_state[floor][state] == true){
                return true;
            }
        }
    }
    return false;
}

bool check_below(bool **order_state, floor_enum current_floor){
    if(current_floor == floor_1){
        return false;
    }
    for(int floor = (current_floor - 1); floor > -1; floor--){
        for(int state = 0; state < 3; state += 2){
            if(order_state[floor][state] == true){
                return true;
            }
        }
    }
    return true;
}

bool queue_system_check_if_stop(elevator_state_machine* state, floor_enum current_floor, int** order_state){
    if((order_state[current_floor][ORDER_INSIDE] || order_state[current_floor][ORDER_UP]) && *(state) == move_up){
        order_state[current_floor][ORDER_INSIDE] = false; 
        if(!check_above(order_state, current_floor)){
            order_state[current_floor][ORDER_UP] = false;
            *(state) = idle;
        }
        return true;
    }
    if((order_state[current_floor][ORDER_INSIDE] || order_state[current_floor][ORDER_DOWN]) && *(state) == move_down){
        order_state[current_floor][ORDER_INSIDE] = false; 
        if(!check_below(order_state, current_floor)){
            order_state[current_floor][ORDER_DOWN] = false;
            *(state) = idle;
        }
        return true;
    }
    return false;
}

void queue_system_clear_all_orders(int** order_state){
    for(int floor = 0; floor < NUMBER_OF_FLOORS; floor++){
        for(int state = 0; state < 3; state++){
            order_state[floor][state] = false;
        }
    }
}

floor_enum queue_system_return_floor(floor_enum current_floor){
    if(hardware_read_floor_sensor(floor_1)){
        return floor_1;
    }
    else if(hardware_read_floor_sensor(floor_2)){
        return floor_2;
        
    }
    else if(hardware_read_floor_sensor(floor_3)){
        return floor_3;  
    }
    else if(hardware_read_floor_sensor(floor_4)){
        return floor_4;
    }
    else{
        return current_floor;
    }
}


void queue_system_update_floor_ligths(floor_enum current_floor){
    if(current_floor == undefined_floor){
        hardware_command_floor_indicator_on(floor_1);
    }
    else{
        hardware_command_floor_indicator_on(current_floor);
    }
}


void queue_system_set_queue_and_light(){
    if(hardware_read_order(floor_1, HARDWARE_ORDER_UP)){
        hardware_command_order_light(floor_1, HARDWARE_ORDER_UP, 1);
    }
    if(hardware_read_order(floor_2, HARDWARE_ORDER_UP)){
        hardware_command_order_light(floor_2, HARDWARE_ORDER_UP, 1);
    }
    if(hardware_read_order(floor_2, HARDWARE_ORDER_DOWN)){
        hardware_command_order_light(floor_2, HARDWARE_ORDER_DOWN, 1);
    }
    if(hardware_read_order(floor_3, HARDWARE_ORDER_UP)){
        hardware_command_order_light(floor_3, HARDWARE_ORDER_UP, 1);
    }
    if(hardware_read_order(floor_3, HARDWARE_ORDER_DOWN)){
        hardware_command_order_light(floor_3, HARDWARE_ORDER_DOWN, 1);
    }
    if(hardware_read_order(floor_4, HARDWARE_ORDER_DOWN)){
        hardware_command_order_light(floor_4, HARDWARE_ORDER_DOWN, 1);
    }
} //legg floors i liste og kjør for-løkke

void queue_system_set_queue_and_light_inside_elevator(){
    if(hardware_read_order(floor_1, HARDWARE_ORDER_INSIDE)){
        hardware_command_order_light(floor_1, HARDWARE_ORDER_INSIDE, 1);
    }
    if(hardware_read_order(floor_2, HARDWARE_ORDER_INSIDE)){
        hardware_command_order_light(floor_2, HARDWARE_ORDER_INSIDE, 1);
    }
    if(hardware_read_order(floor_3, HARDWARE_ORDER_INSIDE)){
        hardware_command_order_light(floor_3, HARDWARE_ORDER_INSIDE, 1);
    }
    if(hardware_read_order(floor_4, HARDWARE_ORDER_INSIDE)){
        hardware_command_order_light(floor_4, HARDWARE_ORDER_INSIDE, 1);
    }
}

void queue_system_set_state(bool** order_state,floor_enum current_floor, elevator_state_machine* state){
    if(check_above(order_state, current_floor)){
        *(state) = move_up;
    }
    if(check_below(order_state, current_floor)){
        *(state) = move_down;
    }
}