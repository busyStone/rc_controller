
#include "cmsis_os.h"

#include "ui_frame.h"
#include "menu_logic.h"

uint16_t menu_page_splash_tbl_size_get(void);
void menu_page_splash_event_process
(
    uint32_t event,
    uint16_t panel_id
);

static uint8_t skyborne_bmp[]=
{
	/*--  宽度x高度=39x30  --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=39x32  --*/
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,
    0x03,0x07,0x1F,0x1F,0x1F,0x3F,0x1F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,
    0x3F,0x3E,0x7E,0x7C,0x78,0xF0,0xC0,0x00,0x00,0x00,0x01,0x03,0x07,0x0F,0x1F,0x1F,
    0x3F,0x7F,0x7F,0x7F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFC,0xFE,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1C,0x7C,
    0xF0,0xF0,0xE0,0xE0,0xC0,0xC0,0x80,0x80,0x80,0xC0,0xC0,0xC0,0xE0,0xE0,0xF0,0xF8,
    0xF8,0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,0xF8,0xF8,0xF0,0xE0,0xC0,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x48,0xEC,0xA4,0xBC,0x00,0xFC,0x30,0xF8,0xCC,0xE0,
    0x7C,0xFC,0xC0,0x00,0x78,0xFC,0xFC,0xFC,0x78,0xF8,0xDC,0xCC,0xF8,0x78,0xFC,0xF0,
    0xFC,0x7C,0xFC,0xF8,0x38,0xFC,0xF8,0xFC,0xFC,0xFC,0xCC,0x00,
};

UI_FRAME_PANEL_STRU g_page_splash_tbl[] = 
{
    /* x   y   w   h   pid  datype                      ditype                               content*/
    {86,   16, 39, 32, 1,   UI_FRAME_PANEL_TYPE_GRAPH, UI_FRAME_PANEL_DISPLAY_STATE_NORMAL, skyborne_bmp},
};

uint16_t menu_page_splash_tbl_size_get(void){
    return sizeof(g_page_splash_tbl) / sizeof(g_page_splash_tbl[0]);
}

void menu_page_splash_event_process
(
    uint32_t event,
    uint16_t panel_id
)
{
    panel_id = panel_id;
    event = event;

    return;
}

