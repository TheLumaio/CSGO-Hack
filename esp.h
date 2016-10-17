#ifndef ESP_H
#define ESP_H

#include <sstream>
#include <iostream>
#include <stdio.h>
#include <windows.h>

typedef struct ESP {
	HDC desktop;
	HBRUSH brush;
	HFONT font;
	HWND target,handle;
	RECT rect;
	COLORREF textcolor;
} ESP;

void init_esp(ESP*);
void draw_box_esp(ESP*, int, int, int, int);
void draw_border_esp(ESP*, int, int, int, int, int);
void draw_text_esp(ESP*, int, int, COLORREF, const char*);
void draw_esp(ESP*, int, int, float, int, int);

#endif /* end of include guard: ESP_H */
