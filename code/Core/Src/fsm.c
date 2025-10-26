#include "fsm.h"
#include "software_timer.h"
#include "main.h"

int led_buffer[4] = {1, 2, 3, 4};
int led1_time = 0;
int led2_time = 0;
int r_time = 15;
int g_time = 12;
int y_time= 3;
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

void setTrafficLED(int r1, int g1, int y1, int r2, int g2, int y2) {
    HAL_GPIO_WritePin(GPIOA, RED_1_Pin, (r1 ? GPIO_PIN_SET : GPIO_PIN_RESET));
    HAL_GPIO_WritePin(GPIOA, GREEN_1_Pin, (g1 ? GPIO_PIN_SET : GPIO_PIN_RESET));
    HAL_GPIO_WritePin(GPIOA, YELLOW_1_Pin, (y1 ? GPIO_PIN_SET : GPIO_PIN_RESET));

    HAL_GPIO_WritePin(GPIOA, RED_2_Pin, (r2 ? GPIO_PIN_SET : GPIO_PIN_RESET));
    HAL_GPIO_WritePin(GPIOA, GREEN_2_Pin, (g2 ? GPIO_PIN_SET : GPIO_PIN_RESET));
    HAL_GPIO_WritePin(GPIOA, YELLOW_2_Pin, (y2 ? GPIO_PIN_SET : GPIO_PIN_RESET));
}

void enterState(int new_state, int t1, int t2,
                int r1, int g1, int y1, int r2, int g2, int y2) {
    status = new_state;
    led1_time = t1;
    led2_time = t2;
    setTrafficLED(r1, g1, y1, r2, g2, y2);
    setTimer1(1000); // 1 giây
}


void Scan_7seg(){
	if(timer2_flag == 1){
		setTimer2(50);
		update7SEG(idx);
		idx = (idx + 1) % 4;
	}
	else if(timer2_cnt ==0){
		setTimer2(50);
	}
}

void fsm_auto_2way_run(){
	if (timer1_flag) {
			// Giảm thời gian mỗi giây
			if (led1_time > 0) led1_time--;
			if (led2_time > 0) led2_time--;

			setTimer1(1000); // lặp lại 1 giây
		}

		switch (status) {
		case INIT:
			enterState(AUTO_R1_G2, r_time, g_time,
					   GPIO_PIN_RESET, GPIO_PIN_SET, GPIO_PIN_SET,
					   GPIO_PIN_SET, GPIO_PIN_RESET, GPIO_PIN_SET);
			break;

		case AUTO_R1_G2:
			if (led2_time <= 0) {
				enterState(AUTO_R1_Y2, led1_time, y_time,
						   GPIO_PIN_RESET, GPIO_PIN_SET, GPIO_PIN_SET,
						   GPIO_PIN_SET, GPIO_PIN_SET, GPIO_PIN_RESET);
			}
			break;

		case AUTO_R1_Y2:
			if (led2_time <= 0) {
				enterState(AUTO_G1_R2, g_time, r_time,
						   GPIO_PIN_SET, GPIO_PIN_RESET, GPIO_PIN_SET,
						   GPIO_PIN_RESET, GPIO_PIN_SET, GPIO_PIN_SET);
			}
			break;

		case AUTO_G1_R2:
			if (led1_time <= 0) {
				enterState(AUTO_Y1_R2, y_time, led2_time,
						   GPIO_PIN_SET, GPIO_PIN_SET, GPIO_PIN_RESET,
						   GPIO_PIN_RESET, GPIO_PIN_SET, GPIO_PIN_SET);
			}
			break;

		case AUTO_Y1_R2:
			if (led1_time <= 0) {
				enterState(AUTO_R1_G2, r_time, g_time,
						   GPIO_PIN_RESET, GPIO_PIN_SET, GPIO_PIN_SET,
						   GPIO_PIN_SET, GPIO_PIN_RESET, GPIO_PIN_SET);
			}
			break;

		default:
			// fallback an toàn: đưa về INIT nếu giá trị không hợp lệ
			enterState(INIT, 0, 0,
					   GPIO_PIN_SET, GPIO_PIN_SET, GPIO_PIN_SET,
					   GPIO_PIN_SET, GPIO_PIN_SET, GPIO_PIN_SET);
			break;
		}
	Scan_7seg();
}
