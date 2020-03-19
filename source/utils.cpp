#include "utils.hpp"

char utils::getChar(HI2::BUTTON b)
{
	switch(b){
	case HI2::KEY_Q:
		return 'q';
	case HI2::KEY_W:
		return 'w';
	case HI2::KEY_E:
		return 'e';
	case HI2::KEY_R:
		return 'r';
	case HI2::KEY_T:
		return 't';
	case HI2::KEY_Y:
		return 'y';
	case HI2::KEY_U:
		return 'u';
	case HI2::KEY_I:
		return 'i';
	case HI2::KEY_O:
		return 'o';
	case HI2::KEY_P:
		return 'p';
	case HI2::KEY_A:
		return 'a';
	case HI2::KEY_S:
		return 's';
	case HI2::KEY_D:
		return 'd';
	case HI2::KEY_F:
		return 'f';
	case HI2::KEY_G:
		return 'g';
	case HI2::KEY_H:
		return 'h';
	case HI2::KEY_J:
		return 'j';
	case HI2::KEY_K:
		return 'k';
	case HI2::KEY_L:
		return 'l';
	case HI2::KEY_Z:
		return 'z';
	case HI2::KEY_X:
		return 'x';
	case HI2::KEY_C:
		return 'c';
	case HI2::KEY_V:
		return 'v';
	case HI2::KEY_B:
		return 'b';
	case HI2::KEY_N:
		return 'n';
	case HI2::KEY_M:
		return 'm';
	case HI2::KEY_ENTER:
		return '\n';
	case HI2::KEY_SPACE:
		return ' ';
	case HI2::KEY_0:
		return '0';
	case HI2::KEY_1:
		return '1';
	case HI2::KEY_2:
		return '2';
	case HI2::KEY_3:
		return '3';
	case HI2::KEY_4:
		return '4';
	case HI2::KEY_5:
		return '5';
	case HI2::KEY_6:
		return '6';
	case HI2::KEY_7:
		return '7';
	case HI2::KEY_8:
		return '8';
	case HI2::KEY_9:
		return '9';
	case HI2::KEY_DASH:
		return '-';
	default:
		return '\0';
	}
}

std::string utils::getString(const std::bitset<HI2::BUTTON_SIZE>& buttons,bool caps, std::string str)
{
	for(unsigned long long i = 0; i < HI2::BUTTON_SIZE; i++){
		if(buttons[i]){
			if((HI2::BUTTON)i == HI2::BUTTON::KEY_BACKSPACE)
			{
				if(str.size()>0)
					str.erase(str.size()-1,1);
			}
			else if((HI2::BUTTON)i == HI2::BUTTON::KEY_ENTER){}
			else if((HI2::BUTTON)i == HI2::BUTTON::KEY_ESCAPE){}
			else{
				char c = getChar((HI2::BUTTON)i);
				if(c!='\0')
					str+=caps?toupper(c):c;
			}
		}
	}
	return str;
}
