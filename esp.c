#include "esp.h"

void init_esp(ESP* esp)
{
	//Initialize GDI Drawing
	esp->target = FindWindow(0, "Counter-Strike: Global Offensive");
	esp->desktop = GetDC(esp->target);
	GetWindowRect(FindWindow(NULL, "Counter-Strike: Global Offensive"), &esp->rect);
	esp->brush = CreateSolidBrush(RGB(255, 0, 0));
	esp->textcolor = RGB(255, 0, 0);
}

void draw_box_esp(ESP* esp, int x, int y, int w, int h)
{
	RECT rect = { x, y, x + w, y + h }; 
	FillRect(esp->desktop, &rect, esp->brush);
}

void draw_border_esp(ESP* esp, int x, int y, int w, int h, int thickness)
{
	draw_box_esp(esp, x, y, w, thickness);
	draw_box_esp(esp,  x, y, thickness, h);
	draw_box_esp(esp, (x + w), y, thickness, h);
	draw_box_esp(esp, x, y + h, w+thickness, thickness);
}

void draw_text_esp(ESP* esp, int x, int y, COLORREF color, const char* text)
{	
	SetTextAlign(esp->desktop,TA_CENTER|TA_NOUPDATECP);

	SetBkColor(esp->desktop,RGB(0,0,0));
	SetBkMode(esp->desktop,TRANSPARENT);

	SetTextColor(esp->desktop,color);

	SelectObject(esp->desktop,esp->font);

	TextOutA(esp->desktop,x,y,text,strlen(text));

	DeleteObject(esp->font);
}

void draw_esp(ESP* esp, int x, int y, float distance, int Health, int fFlags)
{
	//ESP RECTANGLE
	int width = 18100/distance;
	int height = 36000/distance;

	if(fFlags == 775)
	{
		width = 10950/distance;
	    height = 14000/distance;
	}

	draw_border_esp(esp, x-(width/2), y-(height+30), width, height, 2);

	std::stringstream ss, hp;
	ss << (int)distance/3;
	hp << Health;

	char * distanceInfo = new char[ss.str().size()+1];
	strcpy(distanceInfo, ss.str().c_str());
	
	char * HealthInfo = new char[hp.str().size()+1];
	strcpy(HealthInfo, hp.str().c_str());

	draw_text_esp(esp, x - 10, y-(height+50), esp->textcolor, "HP:");
	draw_text_esp(esp, x + 15, y-(height+50), esp->textcolor, HealthInfo);
	
	draw_text_esp(esp, x - 10, y-30, esp->textcolor, "Dist:");
	draw_text_esp(esp, x + 20, y-30, esp->textcolor, distanceInfo);

	delete [] HealthInfo, distanceInfo;

}
