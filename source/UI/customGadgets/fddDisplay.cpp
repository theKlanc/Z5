#include "UI/customGadgets/fddDisplay.hpp"
#include "services.hpp"
#include "icecream.hpp"

fddDisplay::fddDisplay(point2D pos, int height, fdd f, fdd lowerBound, fdd upperBound, int marginSize, int puckSize, int textSize, std::string s)
{
	_fdd=f;
	_name = s;
	_marginSize = marginSize;
	_puckSize = puckSize;
	_textSize = textSize;
	_size.y = height;
	_size.x = height-marginSize-textSize+puckSize+marginSize;

	init(pos,_size,s);
	_lowerBound=lowerBound;
	_upperBound=upperBound;
}

void fddDisplay::update(const double &dt)
{

}

void fddDisplay::setFdd(fdd f)
{
	_fdd = f;
}

void fddDisplay::_draw_internal()
{
	fdd value = _fdd;

	{
		//bg
		HI2::drawRectangle({},_size.x,_size.y,HI2::Color::LightGrey);

		{//draw 2dbg
			point2D pos = {_marginSize+_marginSize+_puckSize,_marginSize+_marginSize+_puckSize};
			point2D size = {_size.y-(5*_marginSize) - _textSize - (2*_puckSize),_size.y-(5*_marginSize) - _textSize - (2*_puckSize)};
			HI2::drawRectangle(pos,size.x,size.y,HI2::Color::Grey);
			if(_lowerBound.y < 0 && _upperBound.y > 0){//draw from zero
				int zeroPixelOffsetY = ((double)size.y/((double)_upperBound.y - (double)_lowerBound.y))*double(_upperBound.y);
				int targetFromZeroOffsetY = -1 * ((double)size.y/((double)_upperBound.y - (double)_lowerBound.y))* (double)value.y;
				HI2::Color centerColor = HI2::Color::White;
				if(targetFromZeroOffsetY + zeroPixelOffsetY < 0){
					targetFromZeroOffsetY = -zeroPixelOffsetY;
					centerColor = HI2::Color::Green;
				}
				if(- targetFromZeroOffsetY + zeroPixelOffsetY < 0){
					targetFromZeroOffsetY = size.y - zeroPixelOffsetY;
					centerColor = HI2::Color::Red;
				}
				int zeroPixelOffsetX = ((double)size.x/((double)_upperBound.x - (double)_lowerBound.x))*double(_upperBound.x);
				int targetFromZeroOffsetX = -1 * ((double)size.x/((double)_upperBound.x - (double)_lowerBound.x))* (double)value.x;
				if(targetFromZeroOffsetX + zeroPixelOffsetX < 0){
					targetFromZeroOffsetX = -zeroPixelOffsetX;
					centerColor = HI2::Color::Green;
				}
				if(- targetFromZeroOffsetX + zeroPixelOffsetX < 0){
					targetFromZeroOffsetY = size.x - zeroPixelOffsetX;
					centerColor = HI2::Color::Red;
				}

				point2D centerPuckPos = pos + point2D{zeroPixelOffsetX-targetFromZeroOffsetX,zeroPixelOffsetY-targetFromZeroOffsetY};
				HI2::drawRectangle({centerPuckPos.x-(_puckSize/2),centerPuckPos.y-(_puckSize/2)},_puckSize,_puckSize,HI2::Color::Black);
				HI2::drawRectangle({centerPuckPos.x-(_puckSize/4),centerPuckPos.y-(_puckSize/4)},(_puckSize/2),(_puckSize/2),centerColor);
			}
			else{//draw upwards

			}

		}
		{//draw rotation


		}
		{//draw textFrame
			point2D pos = {_marginSize,_size.y-_marginSize-_textSize};
			point2D size = {_size.x-(2*_marginSize),_textSize};
			HI2::drawRectangle(pos,size.x,size.y,HI2::Color::White);
			HI2::drawText(*Services::fonts.getFont("lemon"),value.to_string(),pos,_textSize,HI2::Color::Black);
		}
		{//draw z
			point2D pos ={_size.x-_marginSize-_puckSize,_marginSize+_marginSize+_puckSize};
			point2D size = {_puckSize,_size.y-(5*_marginSize) - _textSize - (2*_puckSize)};
			HI2::drawRectangle(pos,size.x,size.y,HI2::Color::Grey);
			if(_lowerBound.z < 0 && _upperBound.z > 0){//draw from zero
				int zeroPixelOffset = ((double)size.y/((double)_upperBound.z - (double)_lowerBound.z))*double(_upperBound.z);
				int targetFromZeroOffset = -1 * ((double)size.y/((double)_upperBound.z - (double)_lowerBound.z))* (double)value.z;
				HI2::Color centerColor = HI2::Color::White;
				if(targetFromZeroOffset + zeroPixelOffset < 0){
					targetFromZeroOffset = -zeroPixelOffset;
					centerColor = HI2::Color::Green;
				}
				if(- targetFromZeroOffset + zeroPixelOffset < 0){
					targetFromZeroOffset = size.y - zeroPixelOffset;
					centerColor = HI2::Color::Red;
				}

				point2D centerPuckPos = pos + point2D{size.x/2,zeroPixelOffset+targetFromZeroOffset};
				HI2::drawRectangle({centerPuckPos.x-(_puckSize/2),centerPuckPos.y-(_puckSize/2)},_puckSize,_puckSize,HI2::Color::Black);
				HI2::drawRectangle({centerPuckPos.x-(_puckSize/4),centerPuckPos.y-(_puckSize/4)},(_puckSize/2),(_puckSize/2),centerColor);
			}
			else{//draw upwards

			}
		}
	}

}
