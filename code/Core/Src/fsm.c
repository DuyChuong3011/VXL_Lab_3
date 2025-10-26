#include "fsm.h"
#include "software_timer.h"

int led_buffer[4] = {1, 2, 3, 4};
int led1_time = 0;
int led2_time = 0;
int r_time = 5;
int g_time = 3;
int y_time= 0;
int idx =0;

void display7SEG(int num) {
	char segNumber[10] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90};
	for (int i = 0; i < 7; ++i) {
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0 << i, (segNumber[num] >> i) & 1);
	}
}

void update7SEG(int index){
	led_buffer[0] = led1_time/10;
	led_buffer[1] = led1_time%10;
	led_buffer[2] = led2_time/10;
	led_buffer[3] = led2_time%10;

	for(int i=0; i< 4; i++)
		HAL_GPIO_WritePin(GPIOA, EN0_Pin << i, GPIO_PIN_SET);

	display7SEG(led_buffer[index]);
	HAL_GPIO_WritePin(GPIOA, EN0_Pin << index, GPIO_PIN_RESET);
}



void fsm_auto_2way_run(){
	if(timer2_flag == 1){
		setTimer2(250); // Đặt lại timer cho chu kỳ quét tiếp theo
		update7SEG(idx);
		idx = (idx + 1) % 4;
	}
	else if(timer2_cnt ==0){
		setTimer2(250);
	}
}
