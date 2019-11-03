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
	case HI2::KEY_ACCEPT:
		return '\n';
	default:
		return '_';
	}
}

std::string utils::getString(unsigned long long buttons, std::string str)
{
	for(unsigned long long i = 0; i <= BIT(63ULL); i*=2){
		if(buttons & i){
			if((HI2::BUTTON)i == HI2::BUTTON::KEY_BACKSPACE)
			{
				str.erase(str.size()-1,1);
			}
			else if((HI2::BUTTON)i == HI2::BUTTON::KEY_ACCEPT){}
			else if((HI2::BUTTON)i == HI2::BUTTON::KEY_ESCAPE){}
			else
				str+=getChar((HI2::BUTTON)i);
		}
	}
	return str;
}
