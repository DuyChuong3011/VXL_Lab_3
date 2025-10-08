#include "button.h"

int button1_flag = 0;
// Chống dội = cách lưu 3 lần trạng thái
int Key_reg_0 = NORMAL_STATE;
int Key_reg_1 = NORMAL_STATE;
int Key_reg_2 = NORMAL_STATE;

// Lưu trạng thái ổn đinh cuối cùng
int Key_reg_3 = NORMAL_STATE;
// đếm giờ khi ấn nút lâu
int TimerForKeyPress = 200;

int isButtonPressed(){
	if(button1_flag == 1){
		button1_flag =0;
		return 1;
	}
	else return 0;
}

// khi nút nhấn ổn định thì gọi
void subKeyProcess(){
	button1_flag = 1;
//	HAL_GPIO_TogglePin(LED_RED_GPIO_Port, LED_RED_Pin);
}

void getKeyInput(){
	Key_reg_0 = Key_reg_1;
	Key_reg_1 = Key_reg_2; // so sánh trạng thái 3 lần
	Key_reg_2 = HAL_GPIO_ReadPin(Button_1_GPIO_Port, Button_1_Pin);
	if((Key_reg_0 == Key_reg_1) && (Key_reg_1 == Key_reg_2)){
		if(Key_reg_3 != Key_reg_2) {
			Key_reg_3 = Key_reg_2;
			if(Key_reg_2 == PRESSED_STATE){
				//TODO
				subKeyProcess();
				TimerForKeyPress = 200;
			}
		}
		else{
			TimerForKeyPress--;
			if(TimerForKeyPress == 0) {
				// TODO
				if(Key_reg_2 == PRESSED_STATE)
				subKeyProcess();
				TimerForKeyPress = 200;
			}

		}
	}

}

