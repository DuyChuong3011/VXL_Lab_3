#include "fsm.h"

int led1_time = 0;
int led2_time = 0;

static int blink_state = 0;
static int mode2_init = 1; // cờ để tắt đèn ngay khi vào MODE_2

int cnt=1;
int flag=1;

void setTrafficLED(int r1, int g1, int y1, int r2, int g2, int y2) {
    HAL_GPIO_WritePin(GPIOA, RED_1_Pin, (r1 ? GPIO_PIN_SET : GPIO_PIN_RESET));
    HAL_GPIO_WritePin(GPIOA, GREEN_1_Pin, (g1 ? GPIO_PIN_SET : GPIO_PIN_RESET));
    HAL_GPIO_WritePin(GPIOA, YELLOW_1_Pin, (y1 ? GPIO_PIN_SET : GPIO_PIN_RESET));

    HAL_GPIO_WritePin(GPIOA, RED_2_Pin, (r2 ? GPIO_PIN_SET : GPIO_PIN_RESET));
    HAL_GPIO_WritePin(GPIOA, GREEN_2_Pin, (g2 ? GPIO_PIN_SET : GPIO_PIN_RESET));
    HAL_GPIO_WritePin(GPIOA, YELLOW_2_Pin, (y2 ? GPIO_PIN_SET : GPIO_PIN_RESET));
}

void display7segment(int num, int id) {
    if (num < 0) num = 0;
    if (num > 9) num = 9;

    const uint8_t segNumber[10] = {
        0xC0, // 0
        0xF9, // 1
        0xA4, // 2
        0xB0, // 3
        0x99, // 4
        0x92, // 5
        0x82, // 6
        0xF8, // 7
        0x80, // 8
        0x90  // 9
    };

    for (int i = 0; i < 7; ++i) {
        GPIO_PinState state = (((segNumber[num] >> i) & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);

        if (id == 0) {
            HAL_GPIO_WritePin(GPIOB, (GPIO_PIN_0 << i), state);   // PB0..PB6
        } else if (id == 1) {
            HAL_GPIO_WritePin(GPIOB, (GPIO_PIN_7 << i), state);   // PB7..PB13
        } else {
            HAL_GPIO_WritePin(GPIOA, (GPIO_PIN_1 << i), state);   // PA1..PA7
        }
    }
}

void enterState(int new_state, int t1, int t2,
                int r1, int g1, int y1, int r2, int g2, int y2) {
    status = new_state;
    led1_time = t1;
    led2_time = t2;
    setTrafficLED(r1, g1, y1, r2, g2, y2);
    display7segment(led1_time, 0);
    display7segment(led2_time, 1);
    setTimer1(1000); // 1 giây
}

void check_button(){
	if(isButtonPressed()){
		mode = MODE_1 + flag;
		flag = (flag+1)%2;
	}
}

void mode_normal(){
	display7segment(1, 2);
	mode2_init = 1;
	    if (timer1_flag) {
	        // Giảm thời gian mỗi giây
	        if (led1_time > 0) led1_time--;
	        if (led2_time > 0) led2_time--;

	        display7segment(led1_time, 0);
	        display7segment(led2_time, 1);

	        setTimer1(1000); // lặp lại 1 giây
	    }

	    switch (status) {
	    case INIT:
	        enterState(AUTO_R1_G2, 5, 3,
	                   GPIO_PIN_RESET, GPIO_PIN_SET, GPIO_PIN_SET,
	                   GPIO_PIN_SET, GPIO_PIN_RESET, GPIO_PIN_SET);
	        break;

	    case AUTO_R1_G2:
	        if (led2_time <= 0) {
	            enterState(AUTO_R1_Y2, led1_time, 2,
	                       GPIO_PIN_RESET, GPIO_PIN_SET, GPIO_PIN_SET,
	                       GPIO_PIN_SET, GPIO_PIN_SET, GPIO_PIN_RESET);
	        }
	        break;

	    case AUTO_R1_Y2:
	        if (led2_time <= 0) {
	            enterState(AUTO_G1_R2, 3, 5,
	                       GPIO_PIN_SET, GPIO_PIN_RESET, GPIO_PIN_SET,
	                       GPIO_PIN_RESET, GPIO_PIN_SET, GPIO_PIN_SET);
	        }
	        break;

	    case AUTO_G1_R2:
	        if (led1_time <= 0) {
	            enterState(AUTO_Y1_R2, 2, led2_time,
	                       GPIO_PIN_SET, GPIO_PIN_SET, GPIO_PIN_RESET,
	                       GPIO_PIN_RESET, GPIO_PIN_SET, GPIO_PIN_SET);
	        }
	        break;

	    case AUTO_Y1_R2:
	        if (led1_time <= 0) {
	            enterState(AUTO_R1_G2, 5, 3,
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
}

void fsm_auto_2way_run() {
	switch (mode) {
	case MODE_1:
		mode_normal();
		check_button();
		break;
	case MODE_2:
		display7segment(2, 2);
		if (mode2_init) {
		    // Tắt tất cả LED
		    setTrafficLED(GPIO_PIN_SET, GPIO_PIN_SET, GPIO_PIN_SET,
		                  GPIO_PIN_SET, GPIO_PIN_SET, GPIO_PIN_SET);

		    blink_state = 0;     // 0 = RED1 sáng, RED2 tắt
		    mode2_init = 0;      // không reset lại nữa
		    setTimer1(500);      // 0.5s để tạo tần số 2Hz
		    display7segment(cnt%10, 0);
		    display7segment(cnt/10, 1);
		}

		if (timer1_flag) {
		    if (blink_state == 0) {
		        // RED1 sáng, RED2 tắt
		        HAL_GPIO_WritePin(GPIOA, RED_1_Pin, GPIO_PIN_RESET);
		        HAL_GPIO_WritePin(GPIOA, RED_2_Pin, GPIO_PIN_SET);
		        blink_state = 1;
		    } else {
		        // RED1 tắt, RED2 sáng
		        HAL_GPIO_WritePin(GPIOA, RED_1_Pin, GPIO_PIN_SET);
		        HAL_GPIO_WritePin(GPIOA, RED_2_Pin, GPIO_PIN_RESET);
		        blink_state = 0;
		    }

		    setTimer1(500+cnt); // lặp lại sau 0.5s
		}
		check_button();
	    break;
	default: break;
	}
}
