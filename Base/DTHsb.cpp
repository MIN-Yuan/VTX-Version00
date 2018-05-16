#include "stdafx.h"
#include "math.h"
#include "DTHsb.h"
#include "DTStr.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


COLORREF HSB2RGB(double hue, double saturation, double brightness)
{
	 double r, g, b;
	 double h, f, p, q, t;

	 hue /= 360;
	 saturation /= 100;
	 brightness /= 100;
	 
	 if (saturation == 0){
		 r = g = b = (int)floor(brightness * 255.0 + 0.5);
	 }else{
		 h = (hue - floor(hue)) * 6.0;
		 f = h - floor(h);
		 p = brightness * (1.0 - saturation);
		 q = brightness * (1.0 - saturation * f);
		 t = brightness * (1.0 - (saturation * (1.0 - f)));
		 switch ((int)floor(h)){
		 case 0:
			 r = floor(brightness * 255.0 + 0.5);
			 g = floor(t * 255.0 + 0.5);
			 b = floor(p * 255.0 + 0.5);
			 break;
		 case 1:
			 r = floor(q * 255.0 + 0.5);
			 g = floor(brightness * 255.0 + 0.5);
			 b = floor(p * 255.0 + 0.5);
			 break;
		 case 2:
			 r = floor(p * 255.0 + 0.5);
			 g = floor(brightness * 255.0 + 0.5);
			 b = floor(t * 255.0 + 0.5);
			 break;
		 case 3:
			 r = floor(p * 255.0 + 0.5);
			 g = floor(q * 255.0 + 0.5);
			 b = floor(brightness * 255.0 + 0.5);
			 break;
		 case 4:
			 r = floor(t * 255.0 + 0.5);
			 g = floor(p * 255.0 + 0.5);
			 b = floor(brightness * 255.0 + 0.5);
			 break;
		 case 5:
			 r = floor(brightness * 255.0 + 0.5);
			 g = floor(p * 255.0 + 0.5);
			 b = floor(q * 255.0 + 0.5);
			 break;
		 }
	 }

	 return RGB(r,g,b); 
 }

 


COLORREF HSB2RGB(CDTStr str)
{
	CHAR * divider=",";

	if(str.GetDividedStringCount(divider)<3){
		return RGB(255,0,255);
	}

	return HSB2RGB(
		str.GetDividedString(0,divider).ToInteger(0,10),
		str.GetDividedString(1,divider).ToInteger(0,10),
		str.GetDividedString(2,divider).ToInteger(0,10)
		);
}
