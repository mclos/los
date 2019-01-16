//============================================================================
// Name        : test_gpu.c
// Author      : LP
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C, Ansi-style
//               create by los(lp). Author's mailbox:lgtbp@126.com
//============================================================================

#include"los_sys.h"
#include"los_gpu.h"
#include "string.h"
#include "stdio.h"
#include "ugui.h"
#include "ugui_config.h"
UG_RESULT _HW_DrawLine(UG_S16 x1, UG_S16 y1, UG_S16 x2, UG_S16 y2, UG_COLOR c) {
	los_draw_line (x1, y1, x2, y2, c);
}

UG_RESULT _HW_FillFrame(UG_S16 x1, UG_S16 y1, UG_S16 x2, UG_S16 y2, UG_COLOR c) {
	los_draw_frect (x1, y1, x2 - x1, y2 - y1, c);
}

void _HW_DrawPoint(UG_S16 x, UG_S16 y, UG_COLOR c) {
	los_draw_point (x, y, c);
}

#define MAX_OBJ_NUM 3
#define LCD_W        240
#define LCD_H        320
UG_GUI gui;
/* Window 1 */
UG_WINDOW window_1;
UG_OBJECT obj_buff_wnd_1[MAX_OBJ_NUM];
UG_BUTTON button1_1;
UG_BUTTON button1_2;

/* Window 2 */
UG_WINDOW window_2;
UG_OBJECT obj_buff_wnd_2[MAX_OBJ_NUM];
UG_BUTTON button2_1;
UG_TEXTBOX textbox2_1;
UG_TEXTBOX textbox2_2;

/* Callback function for the main menu */
void window_1_callback(UG_MESSAGE* msg) {
	if (msg->type == MSG_TYPE_OBJECT) {
		if (msg->id == OBJ_TYPE_BUTTON) {
			switch (msg->sub_id) {
			case BTN_ID_0: {
				if (msg->event == OBJ_EVENT_PRESSED) {
					UG_WindowShow(&window_2);
				}

				break;
			}
			case BTN_ID_1: // Show UI info
			{
				break;
			}

			}
		}
	}
}
/* Callback function for the info window */
void window_2_callback(UG_MESSAGE* msg) {
	if (msg->type == MSG_TYPE_OBJECT) {
		if (msg->id == OBJ_TYPE_BUTTON) {
			switch (msg->sub_id) {
			case BTN_ID_0: {
				if (msg->event == OBJ_EVENT_PRESSED) {
					UG_WindowShow(&window_1);
				}

				break;
			}
			}
		}
	}
}
int ugUI_main(void) {
	UG_Init(&gui, (void (*)(UG_S16, UG_S16, UG_COLOR)) _HW_DrawPoint, LCD_W,
	LCD_H);
	UG_DriverRegister(DRIVER_DRAW_LINE, _HW_DrawLine);
	UG_DriverRegister(DRIVER_FILL_FRAME, _HW_FillFrame);
	// UG_DriverRegister(DRIVER_FILL_AREA, _HW_FillAREA);
	UG_FillScreen( C_BLACK);
	/* Create Window 1 */
	UG_WindowCreate(&window_1, obj_buff_wnd_1, MAX_OBJ_NUM, window_1_callback);
	UG_WindowSetTitleText(&window_1, "uGUI @ los vm");
	UG_WindowSetTitleTextFont(&window_1, &FONT_12X20);

	/* Create some Buttons */
	UG_ButtonCreate(&window_1, &button1_1, BTN_ID_0, 10, 10, 120, 60);
	UG_ButtonCreate(&window_1, &button1_2, BTN_ID_1, 10, 80, 120, 130);

	/* Configure Button 1 */
	UG_ButtonSetFont(&window_1, BTN_ID_0, &FONT_12X20);
	UG_ButtonSetBackColor(&window_1, BTN_ID_0, C_LIME);
	UG_ButtonSetText(&window_1, BTN_ID_0, "About\nGUI");
	/* Configure Button 2 */
	UG_ButtonSetFont(&window_1, BTN_ID_1, &FONT_12X20);
	UG_ButtonSetBackColor(&window_1, BTN_ID_1, C_RED);
	UG_ButtonSetText(&window_1, BTN_ID_1, "NO");

	UG_WindowShow(&window_1);

	/* -------------------------------------------------------------------------------- */
	/* Create Window 2 (UI Info)                                                      */
	/* -------------------------------------------------------------------------------- */
	UG_WindowCreate(&window_2, obj_buff_wnd_2, MAX_OBJ_NUM, window_2_callback);
	UG_WindowSetTitleText(&window_2, "Info");
	UG_WindowSetTitleTextFont(&window_2, &FONT_12X20);
	UG_WindowResize(&window_2, 20, 40, 219, 279);
	/* Create Button 1 */
	UG_ButtonCreate(&window_2, &button2_1, BTN_ID_0, 40, 150,
			UG_WindowGetInnerWidth(&window_2) - 40, 200);
	UG_ButtonSetFont(&window_2, BTN_ID_0, &FONT_12X20);
	UG_ButtonSetText(&window_2, BTN_ID_0, "OK!");
	/* Create Textbox 1 */
	UG_TextboxCreate(&window_2, &textbox2_1, TXB_ID_0, 10, 10,
			UG_WindowGetInnerWidth(&window_2) - 10, 40);
	UG_TextboxSetFont(&window_2, TXB_ID_0, &FONT_12X20);
	UG_TextboxSetText(&window_2, TXB_ID_0, "uGUI v0.3");
	UG_TextboxSetAlignment(&window_2, TXB_ID_0, ALIGN_TOP_CENTER);

	/* Create Textbox 2 */
	UG_TextboxCreate(&window_2, &textbox2_2, TXB_ID_1, 10, 125,
			UG_WindowGetInnerWidth(&window_2) - 10, 135);
	UG_TextboxSetFont(&window_2, TXB_ID_1, &FONT_6X8);
	UG_TextboxSetText(&window_2, TXB_ID_1, "www.embeddedlightning.com");
	UG_TextboxSetAlignment(&window_2, TXB_ID_1, ALIGN_BOTTOM_CENTER);
	UG_TextboxSetForeColor(&window_2, TXB_ID_1, C_BLUE);
	UG_TextboxSetHSpace(&window_2, TXB_ID_1, 1);

	UG_Update();        //Windows refesh

}
void msg_cb(uint32_t id, uint32_t hram, uint32_t lram) {
	los_printf("[%d][%d][%d]\r\n", id, hram, lram);
	if (id == 0)
		UG_TouchUpdate(hram, lram, TOUCH_STATE_PRESSED); //触摸坐标更新TOUCH_STATE_RELEASED
	else
		UG_TouchUpdate(hram, lram, TOUCH_STATE_RELEASED);        //触摸坐标更新
	UG_Update();        //Windows refesh

}
int main() {
	los_printf("!!!Hello World!!!"); // prints !!!Hello World!!!
	ugUI_main();
	los_set_msg(msg_cb);
	while (1) {
		los_wait_event();
		los_delay_ms(5);

	}
	return 0;
}
