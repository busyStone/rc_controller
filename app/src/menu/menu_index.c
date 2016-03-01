
#include "cmsis_os.h"

#include <stdio.h>
#include <string.h>

#include "ui_frame.h"
#include "menu_logic.h"
#include "communicate.h"

#include "ADCs.h"
#include "lcd.h"


uint16_t menu_page_index_tbl_size_get(void);
void menu_page_index_event_process
(
    uint32_t event,
    uint16_t panel_id
);

uint8_t _menu_page_index_sw_tag_get(uint8_t val);
void _menu_page_index_local_baterry_draw(uint16_t panel_id, UI_FRAME_PANEL_STRU* panel);
void _menu_page_index_plane_baterry_draw(uint16_t panel_id, UI_FRAME_PANEL_STRU* panel);
void _menu_page_index_rssi_draw(uint16_t panel_id, UI_FRAME_PANEL_STRU* panel);
void _menu_page_index_sw_value_draw(uint16_t panel_id, UI_FRAME_PANEL_STRU* panel);
void _menu_page_index_rotate_switch_draw(uint16_t panel_id, UI_FRAME_PANEL_STRU *panel);
void _menu_page_index_attitude_draw(uint16_t panel_id, UI_FRAME_PANEL_STRU *panel);
void _menu_page_index_private_draw_update(uint16_t panel_id);

static uint8_t m_flight_mode[] = "DISARMED";

UI_FRAME_PANEL_STRU g_page_index_tbl[] = 
{
    /* x   y   w    h   pid                 datype                      ditype                               content*/
    {16,   6,  56,  8,  MENU_PAGE_INDEX_ID, UI_FRAME_PANEL_TYPE_PRIVATE,UI_FRAME_PANEL_DISPLAY_STATE_NORMAL, _menu_page_index_local_baterry_draw},
    {80,   6,  62,  8,  MENU_PAGE_INDEX_ID, UI_FRAME_PANEL_TYPE_PRIVATE,UI_FRAME_PANEL_DISPLAY_STATE_NORMAL, _menu_page_index_plane_baterry_draw},
    {150,  6,  46,  8,  MENU_PAGE_INDEX_ID, UI_FRAME_PANEL_TYPE_PRIVATE,UI_FRAME_PANEL_DISPLAY_STATE_NORMAL, _menu_page_index_rssi_draw},

    {35,   19, 142, 8,  MENU_PAGE_INDEX_ID, UI_FRAME_PANEL_TYPE_PRIVATE,UI_FRAME_PANEL_DISPLAY_STATE_NORMAL, _menu_page_index_sw_value_draw},

    {82,   37, 48,  8,  MENU_PAGE_INDEX_ID, UI_FRAME_PANEL_TYPE_STRING, UI_FRAME_PANEL_DISPLAY_STATE_NORMAL, m_flight_mode},

    {43,   50, 125, 8,  MENU_PAGE_INDEX_ID, UI_FRAME_PANEL_TYPE_PRIVATE, UI_FRAME_PANEL_DISPLAY_STATE_NORMAL, _menu_page_index_attitude_draw},

    {2,    3,  4,   26, MENU_PAGE_INDEX_ID, UI_FRAME_PANEL_TYPE_PRIVATE,UI_FRAME_PANEL_DISPLAY_STATE_NORMAL, _menu_page_index_rotate_switch_draw},
    {2,    35, 4,   26, MENU_PAGE_INDEX_ID, UI_FRAME_PANEL_TYPE_PRIVATE,UI_FRAME_PANEL_DISPLAY_STATE_NORMAL, _menu_page_index_rotate_switch_draw},
    {205,  3,  4,   26, MENU_PAGE_INDEX_ID, UI_FRAME_PANEL_TYPE_PRIVATE,UI_FRAME_PANEL_DISPLAY_STATE_NORMAL, _menu_page_index_rotate_switch_draw},
    {205,  35, 4,   26, MENU_PAGE_INDEX_ID, UI_FRAME_PANEL_TYPE_PRIVATE,UI_FRAME_PANEL_DISPLAY_STATE_NORMAL, _menu_page_index_rotate_switch_draw},
};

uint16_t menu_page_index_tbl_size_get(void){
    return sizeof(g_page_index_tbl) / sizeof(g_page_index_tbl[0]);
}

void menu_page_index_event_process
(
    uint32_t event,
    uint16_t panel_id
)
{
    panel_id = panel_id;
    
    switch(event){
        case UI_FRAME_EVENT_DATA_UPDATE:
            
            _menu_page_index_private_draw_update(0);
            _menu_page_index_private_draw_update(1);
            _menu_page_index_private_draw_update(2);
            _menu_page_index_private_draw_update(3);
            _menu_page_index_private_draw_update(5);
            _menu_page_index_private_draw_update(6);
            _menu_page_index_private_draw_update(7);
            _menu_page_index_private_draw_update(8);
            _menu_page_index_private_draw_update(9);
            
            break;
    }

    return;
}

void _menu_page_index_private_draw_update(uint16_t panel_id){
    ui_frame_panel_content_set(panel_id, UI_FRAME_PANEL_TYPE_PRIVATE, 
                g_page_index_tbl[panel_id].content);
}

#define MENU_PAGE_INDEX_ATTITUDE_DRAW(x, y, name, format, val_get_fn) \
    do{                                         \
        lcd_str_disp(x, y, name);               \
        startX += 6 + 3;                        \
        memset(attitude, 0, sizeof(attitude));  \
        sprintf(attitude, format, val_get_fn);  \
        lcd_str_disp(x, y, (unsigned char *)attitude);           \
        startX += 30 + 4;                       \
    }while(0)
void _menu_page_index_attitude_draw(uint16_t panel_id, UI_FRAME_PANEL_STRU* panel){
    uint8_t startX = panel->x, y = panel->y;
    char attitude[6];

    panel_id = panel_id;

    MENU_PAGE_INDEX_ATTITUDE_DRAW(startX, y, 
        (unsigned char*)"R", "%3.0f@", comm_mav_data_roll_get());
    MENU_PAGE_INDEX_ATTITUDE_DRAW(startX, y, 
        (unsigned char*)"P", "%3.0f@", comm_mav_data_pitch_get());
    MENU_PAGE_INDEX_ATTITUDE_DRAW(startX, y, 
        (unsigned char*)"H", "%d@N", comm_mav_data_heading_get());
}

uint8_t _menu_page_index_sw_tag_get(uint8_t val){
    uint8_t tag = '-';

    switch (val){
        case 1:
            tag = '{';
            break;
        case 2:
            tag = '}';
            break;
        case 0:
        default:
            break;
    }

    return tag;
}

//
// |<---35--->|<-4->|<--8-->|<-4->|<--...-->|
//           SA     0      SB     1   ...
#define MENU_PAGE_INDEX_SW_VALUE_DRAW(x, y, name, val) \
    do{\
        lcd_str_disp(x, y, name);                 \
        x += 12 + 4;                              \
        sw[0] = _menu_page_index_sw_tag_get(val); \
        lcd_str_disp(x, y, sw);                   \
        startX += 6 + 8;                          \
    }while(0)
void _menu_page_index_sw_value_draw(uint16_t panel_id, UI_FRAME_PANEL_STRU* panel){
    uint8_t startX = panel->x, y = panel->y;
    uint8_t sw[2] = {0, 0};
    panel_id = panel_id;

    ALL_STICK_INPUT_t input;
    adc_all_in_val_get(&input);

    MENU_PAGE_INDEX_SW_VALUE_DRAW(startX, y, (unsigned char*)"SA", (uint8_t)input.SW.SWS.SA);
    MENU_PAGE_INDEX_SW_VALUE_DRAW(startX, y, (unsigned char*)"SB", (uint8_t)input.SW.SWS.SB);
    MENU_PAGE_INDEX_SW_VALUE_DRAW(startX, y, (unsigned char*)"SC", (uint8_t)input.SW.SWS.SC);
    MENU_PAGE_INDEX_SW_VALUE_DRAW(startX, y, (unsigned char*)"SD", (uint8_t)input.SW.SWS.SD);
    MENU_PAGE_INDEX_SW_VALUE_DRAW(startX, y, (unsigned char*)"SE", (uint8_t)input.SW.SWS.SE);
}

void _menu_page_index_local_baterry_draw(uint16_t panel_id, UI_FRAME_PANEL_STRU* panel){
    uint8_t startX = panel->x, y = panel->y;
    panel_id = panel_id;

    lcd_str_disp(startX, y, (unsigned char*)"L");
    startX += 6 + 4;
    lcd_str_disp(startX, y, (unsigned char*)"-.-v");
    startX += 24 + 4;
    lcd_str_disp(startX, y, (unsigned char*)"--%");
}

void _menu_page_index_plane_baterry_draw(uint16_t panel_id, UI_FRAME_PANEL_STRU* panel){
    uint8_t startX = panel->x, y = panel->y;
    panel_id = panel_id;

    lcd_str_disp(startX, y, (unsigned char*)"R");
    startX += 6 + 4;
    lcd_str_disp(startX, y, (unsigned char*)"--.-v");
    startX += 30 + 4;
    lcd_str_disp(startX, y, (unsigned char*)"--%");
}

void _menu_page_index_rssi_draw(uint16_t panel_id, UI_FRAME_PANEL_STRU* panel){
    uint8_t startX = panel->x, y = panel->y;
    panel_id = panel_id;

    lcd_str_disp(startX, y, (unsigned char*)"RSSI");
    startX += 24 + 4;
    lcd_str_disp(startX, y, (unsigned char*)"--%");    
}

void _menu_page_index_rotate_switch_draw(uint16_t panel_id, UI_FRAME_PANEL_STRU *panel){
    if (panel == NULL) return;

    ALL_STICK_INPUT_t input;
    adc_all_in_val_get(&input);

    uint8_t type;

    switch (panel_id){
        case 6:
            type = ROTATE_LU;
            break;
        case 7:
            type = ROTATE_LD;
            break;
        case 8:
            type = ROTATE_RU;
            break;
        case 9:
            type = ROTATE_RD;
            break;
        default:
            return;

    }

    uint16_t val = input.adcs[type];
    
    lcd_clear_rect(panel->x, panel->y, panel->width, panel->height);
    lcd_vline_disp(panel->x + 1, panel->y, panel->height, 2);

    if (val > 2000){
        val = 2000;
    } else if (val < 1000){
        val = 1000;
    }

    val -= 1000;
    val /= 40;
    lcd_hline_disp(panel->x, panel->y + val, panel->width, 2);
}

