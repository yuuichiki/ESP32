// SPDX-License-Identifier: MIT
// Copyright 2020 NXP

/**
 * @file custom.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <stdio.h>
#include "lvgl.h"
#include "custom.h"
int cur_scr;
/*Colors*/
#define LV_DEMO_PRINTER_WHITE           lv_color_hex(0xffffff)
#define LV_DEMO_PRINTER_LIGHT           lv_color_hex(0xf3f8fe)
#define LV_DEMO_PRINTER_GRAY            lv_color_hex(0x8a8a8a)
#define LV_DEMO_PRINTER_LIGHT_GRAY      lv_color_hex(0xc4c4c4)
#define LV_DEMO_PRINTER_BLUE            lv_color_hex(0x2f3243)
#define LV_DEMO_PRINTER_GREEN           lv_color_hex(0x4cb242)
#define LV_DEMO_PRINTER_RED             lv_color_hex(0xd51732)

#define LV_DEMO_PRINTER_ANIM_Y (LV_VER_RES / 20)
#define LV_DEMO_PRINTER_ANIM_DELAY (40)
#define LV_DEMO_PRINTER_ANIM_TIME  (150)
#define LV_DEMO_PRINTER_ANIM_TIME_BG  (300)
#define LOAD_ANIM_TIME 1000
lv_anim_t ani_en_btn_click;

static uint16_t printusbcnt = 1;
static uint16_t copynextcnt = 1;
static uint16_t hue_act;
static int16_t lightness_act;

static uint16_t save_src = 0;
char label[20] = {0};
void cnt_event_cb(lv_event_t * event){
    lv_event_code_t code = lv_event_get_code(event);
    lv_obj_t * obj = lv_event_get_target(event);
    if (code == LV_EVENT_PRESSED) {
        if(guider_ui.copynext == lv_scr_act()) {
            if (obj == guider_ui.copynext_up) {
                if (copynextcnt < 200)
                    copynextcnt++;
                sprintf(label, "%d", copynextcnt);
                lv_label_set_text(guider_ui.copynext_labelcnt, label);
            } else {
                if (copynextcnt > 1)
                    copynextcnt--;
                sprintf(label, "%d", copynextcnt);
                lv_label_set_text(guider_ui.copynext_labelcnt, label);

            }
        } else if (guider_ui.prtusb == lv_scr_act()) {
            if (obj == guider_ui.prtusb_up) {
                if (printusbcnt < 200)
                    printusbcnt++;
                sprintf(label, "%d", printusbcnt);
                lv_label_set_text(guider_ui.prtusb_labelcnt, label);
            } else {
                if (printusbcnt > 1)
                    printusbcnt--;
                sprintf(label, "%d", printusbcnt);
                lv_label_set_text(guider_ui.prtusb_labelcnt, label);
            }
        }
    }
}

void lv_demo_printer_anim_in_all(lv_obj_t * obj, uint32_t delay)
{
    lv_coord_t y;
    lv_obj_t * child;
    for(int i = 0; i < lv_obj_get_child_cnt(obj); i++) {
        child = lv_obj_get_child(obj, i);
        lv_obj_update_layout(child);
        y = lv_obj_get_y(child);
        if (child != lv_scr_act()  && child != guider_ui.saved_img1) {
            lv_anim_t a;
            lv_anim_init(&a);
            lv_anim_set_var(&a, child);
            lv_anim_set_time(&a, LV_DEMO_PRINTER_ANIM_TIME);
            lv_anim_set_delay(&a, delay);
            lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t) lv_obj_set_y);

            if(y > 150)
                lv_anim_set_values(&a, 270, y);
            else
                lv_anim_set_values(&a, 0, y);
            
            lv_anim_start(&a);

            lv_obj_fade_in(child, LV_DEMO_PRINTER_ANIM_TIME - 50, delay);
        }
    }
}

void loader_anim_cb(void * arc, uint32_t v)
{
    if(v > 100) v = 100;
    lv_arc_set_angles(arc, 270, v * 360 / 100 + 270);

    static char buf[32];
    lv_snprintf(buf, sizeof(buf), "%"LV_PRIu32 "%%", v);
    lv_label_set_text(guider_ui.loader_loadlabel, buf);
}

void add_loader(void (*end_cb)(lv_anim_t *))
{
    lv_anim_t a;
    lv_arc_set_angles(guider_ui.loader_loadarc, 270, 270);
    lv_anim_init(&a);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t) loader_anim_cb);
    lv_anim_set_ready_cb(&a, end_cb);
    lv_anim_set_values(&a, 0, 110);
    lv_anim_set_time(&a, LOAD_ANIM_TIME);
    lv_anim_set_var(&a, guider_ui.loader_loadarc);
    lv_anim_start(&a);
}

lv_obj_t **get_scr_by_id(int scr_id)
{
    if(scr_id == SCR_HOME)
        return &guider_ui.home;
    else if(scr_id == SCR_COPY_HOME)
        return &guider_ui.copyhome;
    else if(scr_id == SCR_COPY_NEXT)
        return &guider_ui.copynext;
    else if(scr_id == SCR_SCAN_HOME)
        return &guider_ui.scanhome;
    else if(scr_id == SCR_PRT_HOME)
        return &guider_ui.prthome;
    else if(scr_id == SCR_PRT_USB)
        return &guider_ui.prtusb;
    else if(scr_id == SCR_PRT_MB)
        return &guider_ui.prtmb;
    else if(scr_id == SCR_PRT_IT)
        return &guider_ui.printit;
    else if(scr_id == SCR_SETUP)
        return &guider_ui.setup;
    else if(scr_id == SCR_LOADER)
        return &guider_ui.loader;
    else if(scr_id == SCR_SAVED)
        return &guider_ui.saved;
    
    return NULL;
}
void guider_load_screen(int scr_id)
{
    lv_obj_t **scr = NULL;
    lv_obj_t **old_scr = NULL;

    switch(scr_id)
    {
        case SCR_HOME:
            if(!guider_ui.home) {
                scr = &guider_ui.home;
                setup_scr_home(&guider_ui);
                lv_anim_set_var(&ani_en_btn_click, guider_ui.home_imgbtncopy);
                lv_anim_start(&ani_en_btn_click);
            }
            break;
        case SCR_COPY_HOME:
            if(!guider_ui.copyhome) {
                scr = &guider_ui.copyhome;
                setup_scr_copyhome(&guider_ui);
                copy_home_event_init();
                lv_anim_set_var(&ani_en_btn_click, guider_ui.copyhome_btncopyback);
                lv_anim_start(&ani_en_btn_click);
            }
            break;
        case SCR_COPY_NEXT:
            if(!guider_ui.copynext) {
                scr = &guider_ui.copynext;
                copynextcnt = 1;
                setup_scr_copynext(&guider_ui);
                copy_next_event_init();
            }
            break;
        case SCR_SCAN_HOME:
            if(!guider_ui.scanhome) {
                scr = &guider_ui.scanhome;
                setup_scr_scanhome(&guider_ui);
                scan_home_event_init();
            }
            break;
        case SCR_PRT_HOME:
            if(!guider_ui.prthome) {
                scr = &guider_ui.prthome;
                setup_scr_prthome(&guider_ui);
                prt_home_event_init();
            }
            break;
        case SCR_PRT_USB:
            if(!guider_ui.prtusb) {
                scr = &guider_ui.prtusb;
                printusbcnt = 1;
                setup_scr_prtusb(&guider_ui);
                prt_usb_event_init();
            }
            break;
        case SCR_PRT_MB:
            if(!guider_ui.prtmb) {
                scr = &guider_ui.prtmb;
                setup_scr_prtmb(&guider_ui);
                prt_mb_event_init();
            }
            break;
        case SCR_PRT_IT:
            if(!guider_ui.printit) {
                scr = &guider_ui.printit;
                setup_scr_printit(&guider_ui);
                prtit_event_init();
            }
            break;
        case SCR_SETUP:
            if(!guider_ui.setup) {
                scr = &guider_ui.setup;
                setup_scr_setup(&guider_ui);
                setup_event_init();
            }
            break;
        case SCR_LOADER:
            if(!guider_ui.loader) {
                scr = &guider_ui.loader;
                setup_scr_loader(&guider_ui);
                loader_event_init();
            }
            break;
        case SCR_SAVED:
            if(!guider_ui.saved) {
                scr = &guider_ui.saved;
                setup_scr_saved(&guider_ui);
                saved_event_init();
            }
            break;
    }
    lv_scr_load(*scr);
    old_scr = get_scr_by_id(cur_scr);
    lv_obj_clean(*old_scr);
    lv_obj_del(*old_scr);
    
    *old_scr = NULL;
    cur_scr = scr_id;

}
void load_home(lv_anim_t *a) {
    guider_load_screen(SCR_HOME);
}
void load_print(lv_anim_t *a) {
    guider_load_screen(SCR_PRT_HOME);
    lv_demo_printer_anim_in_all(guider_ui.prthome, 200);
}
void load_copy(lv_anim_t *a) {
    guider_load_screen(SCR_COPY_HOME);
    lv_demo_printer_anim_in_all(guider_ui.copyhome, 200);
}
void load_setup(lv_anim_t *a) {
    guider_load_screen(SCR_SETUP);
    lv_demo_printer_anim_in_all(guider_ui.setup, 200);
}
void load_scan(lv_anim_t *a) {
    guider_load_screen(SCR_SCAN_HOME);
    lv_demo_printer_anim_in_all(guider_ui.scanhome, 200);
}

void load_save(lv_anim_t *a) {
    guider_load_screen(SCR_SAVED);
    if (save_src == 1) {
        lv_obj_set_x(guider_ui.saved_label2, 157);
        lv_label_set_text(guider_ui.saved_label2, "File saved");
    } else if (save_src == 2) {
        lv_obj_set_x(guider_ui.saved_label2, 157);
        lv_label_set_text(guider_ui.saved_label2, "Printing finished");
    } else {
        lv_obj_set_x(guider_ui.saved_label2, 157);
        lv_label_set_text(guider_ui.saved_label2, "File saved");
    }
    
    lv_demo_printer_anim_in_all(guider_ui.saved, 200);
}

void en_click_anim_cb(void * btn, uint32_t v)
{
    if(v >= LV_DEMO_PRINTER_ANIM_TIME) {
        switch (cur_scr) {
            case SCR_HOME:
                lv_obj_clear_flag(guider_ui.home_imgbtncopy, true);
                lv_obj_clear_flag(guider_ui.home_imgbtnprt, true);
                lv_obj_clear_flag(guider_ui.home_imgbtnscan, true);
                lv_obj_clear_flag(guider_ui.home_imgbtnset, true);
                break;
            case SCR_COPY_HOME:
                lv_obj_clear_flag(guider_ui.copyhome_btncopyback, true);
                break;
            case SCR_COPY_NEXT:
                lv_obj_clear_flag(guider_ui.copynext_print, true);
                break;
            case SCR_SCAN_HOME:
                break;
            case SCR_PRT_HOME:
                break;
            case SCR_PRT_USB:
                break;
            case SCR_PRT_MB:
                break;
            case SCR_PRT_IT:
                break;
            case SCR_SETUP:
                break;
            case SCR_LOADER:
                break;
            case SCR_SAVED:
                break;
        }
    }
}

void load_home_cb(lv_event_t * event){
    lv_event_code_t code = lv_event_get_code(event);
    if (code == LV_EVENT_PRESSED) {
        guider_load_screen(SCR_HOME);
        lv_demo_printer_anim_in_all(guider_ui.home, 200);
    }
}

void load_copy_next_cb(lv_event_t * event){
    lv_event_code_t code = lv_event_get_code(event);
    if (code == LV_EVENT_PRESSED) {
        guider_load_screen(SCR_COPY_NEXT);
        lv_demo_printer_anim_in_all(guider_ui.copynext, 200);

        lv_anim_set_var(&ani_en_btn_click, guider_ui.copynext_print);
        lv_anim_start(&ani_en_btn_click);
    }
}

void load_print_finish_cb(lv_event_t * event){
    lv_event_code_t code = lv_event_get_code(event);
    if (code == LV_EVENT_PRESSED) {
        save_src = 2;
        guider_load_screen(SCR_LOADER);
        add_loader(load_save);
    }
}

void load_disbtn_home_cb(lv_event_t * event){
    lv_event_code_t code = lv_event_get_code(event);
    if (code == LV_EVENT_PRESSED) {
        guider_load_screen(SCR_HOME);
    }
}


void load_save_cb(lv_event_t * event){
    lv_event_code_t code = lv_event_get_code(event);
    if (code == LV_EVENT_PRESSED) {
        guider_load_screen(SCR_LOADER);
        save_src = 1;
        add_loader(load_save);
    }
}

void load_print_usb_cb(lv_event_t * event){
    lv_event_code_t code = lv_event_get_code(event);
    if (code == LV_EVENT_PRESSED) {
        guider_load_screen(SCR_PRT_USB);
        lv_demo_printer_anim_in_all(guider_ui.prtusb, 200);
    }
}

void load_print_mobile_cb(lv_event_t * event){
    lv_event_code_t code = lv_event_get_code(event);
    if (code == LV_EVENT_PRESSED) {
        guider_load_screen(SCR_PRT_MB);
        lv_demo_printer_anim_in_all(guider_ui.prtmb, 200);
    }
}

void load_print_it_cb(lv_event_t * event){
    lv_event_code_t code = lv_event_get_code(event);
    if (code == LV_EVENT_PRESSED) {
        guider_load_screen(SCR_PRT_IT);
        lv_demo_printer_anim_in_all(guider_ui.printit, 200);
    }
}
lv_color_t c;
void scan_img_color_refr(void)
{
    int16_t light = lightness_act - 80;

    if(guider_ui.scanhome == lv_scr_act()) {
        uint8_t s = light > 0 ? 100 - light : 100;
        uint8_t v = light < 0 ? 100 + light : 100;
        lv_color_t c = lv_color_hsv_to_rgb(hue_act, s, v);

        lv_obj_set_style_img_recolor_opa(guider_ui.scanhome_img3, v, 0);
        lv_obj_set_style_img_recolor(guider_ui.scanhome_img3, c, 0);

    } else if (guider_ui.copyhome == lv_scr_act()) {
        uint8_t s = light > 0 ? 100 - light : 100;
        uint8_t v = light < 0 ? 100 + light : 100;
        c = lv_color_hsv_to_rgb(hue_act, s, v);
        lv_obj_set_style_img_recolor_opa(guider_ui.copyhome_img3, v, 0);
        lv_obj_set_style_img_recolor(guider_ui.copyhome_img3, c, 0);
    }
}

void hue_slider_event_cb(lv_event_t * event){
    lv_event_code_t code = lv_event_get_code(event);
    lv_obj_t * obj = lv_event_get_target(event);
    if (code == LV_EVENT_VALUE_CHANGED) {
        hue_act = lv_slider_get_value(obj);
        scan_img_color_refr();
    }
}

void lightness_slider_event_cb(lv_event_t * event){
    lv_event_code_t code = lv_event_get_code(event);
    lv_obj_t * obj = lv_event_get_target(event);
    if (code == LV_EVENT_VALUE_CHANGED) {
        lightness_act = lv_slider_get_value(obj);
        scan_img_color_refr();
    }
}

static lv_style_t style_backbtn;
static lv_style_t style_upbtn;
static lv_style_t style_downbtn;
static lv_style_t arc_style;

void home_event_init(void)
{
    lv_demo_printer_anim_in_all(guider_ui.home, 200);
}
void copy_home_event_init(void)
{
    lv_obj_add_event_cb(guider_ui.copyhome_btncopyback, load_disbtn_home_cb, LV_EVENT_ALL, NULL);
    lv_obj_add_style(guider_ui.copyhome_btncopyback, &style_backbtn, LV_STATE_DEFAULT);
    lv_obj_add_event_cb(guider_ui.copyhome_btncopynext, load_copy_next_cb, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(guider_ui.copyhome_sliderhue, hue_slider_event_cb, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(guider_ui.copyhome_sliderbright, lightness_slider_event_cb, LV_EVENT_ALL, NULL);
    lv_obj_set_style_radius(guider_ui.copyhome_img3, 8, LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(guider_ui.copyhome_img3, true, LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_recolor_opa(guider_ui.copyhome_img3, 80, LV_STATE_DEFAULT);

}
void copy_next_event_init(void)
{
    lv_obj_add_style(guider_ui.copynext_btncopyback, &style_backbtn, LV_STATE_DEFAULT);
    lv_obj_clear_flag(guider_ui.copynext_print, false);
    lv_obj_add_event_cb(guider_ui.copynext_up, cnt_event_cb, LV_EVENT_ALL, NULL);
    lv_obj_add_style(guider_ui.copynext_up, &style_upbtn, LV_STATE_DEFAULT);
    lv_obj_add_event_cb(guider_ui.copynext_down, cnt_event_cb, LV_EVENT_ALL, NULL);
    lv_obj_add_style(guider_ui.copynext_down, &style_downbtn, LV_STATE_DEFAULT);
    lv_obj_add_event_cb(guider_ui.copynext_print, load_print_finish_cb, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(guider_ui.copynext_btncopyback, load_disbtn_home_cb, LV_EVENT_ALL, NULL);
    lv_obj_set_style_radius(guider_ui.copynext_img3, 5, LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(guider_ui.copynext_img3, true, LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_recolor_opa(guider_ui.copynext_img3, 80, LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_recolor(guider_ui.copynext_img3, c, LV_STATE_DEFAULT);
}
void scan_home_event_init(void)
{
    lv_obj_add_event_cb(guider_ui.scanhome_btnscanback, load_disbtn_home_cb, LV_EVENT_ALL, NULL);
    lv_obj_add_style(guider_ui.scanhome_btnscanback, &style_backbtn, LV_STATE_DEFAULT);
    lv_obj_add_event_cb(guider_ui.scanhome_btnscansave, load_save_cb, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(guider_ui.scanhome_sliderhue, hue_slider_event_cb, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(guider_ui.scanhome_sliderbright, lightness_slider_event_cb, LV_EVENT_ALL, NULL);
    lv_obj_set_style_radius(guider_ui.scanhome_img3, 8, LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(guider_ui.scanhome_img3, true, LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_recolor_opa(guider_ui.scanhome_img3, 80, LV_STATE_DEFAULT);

}
void prt_home_event_init(void)
{
    lv_obj_add_event_cb(guider_ui.prthome_imgbtnusb, load_print_usb_cb, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(guider_ui.prthome_imgbtnmobile, load_print_mobile_cb, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(guider_ui.prthome_imgbtnit, load_print_it_cb, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(guider_ui.prthome_btnprintback, load_disbtn_home_cb, LV_EVENT_ALL, NULL);
    lv_obj_add_style(guider_ui.prthome_btnprintback, &style_backbtn, LV_STATE_DEFAULT);
}
void prt_usb_event_init(void)
{
    lv_obj_add_style(guider_ui.prtusb_back, &style_backbtn, LV_STATE_DEFAULT);
    lv_obj_add_event_cb(guider_ui.prtusb_back, load_disbtn_home_cb, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(guider_ui.prtusb_btnprint, load_print_finish_cb, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(guider_ui.prtusb_up, cnt_event_cb, LV_EVENT_ALL, NULL);
    lv_obj_add_style(guider_ui.prtusb_up, &style_upbtn, LV_STATE_DEFAULT);
    lv_obj_add_event_cb(guider_ui.prtusb_down, cnt_event_cb, LV_EVENT_ALL, NULL);
    lv_obj_add_style(guider_ui.prtusb_down, &style_downbtn, LV_STATE_DEFAULT);

}
void prt_mb_event_init(void)
{
    lv_obj_add_event_cb(guider_ui.prtmb_btnback, load_disbtn_home_cb, LV_EVENT_ALL, NULL);
    
}
void prtit_event_init(void)
{
    lv_obj_add_event_cb(guider_ui.printit_btnprtitback, load_disbtn_home_cb, LV_EVENT_ALL, NULL);
    
}
void setup_event_init(void)
{
    lv_obj_add_event_cb(guider_ui.setup_btnsetback, load_disbtn_home_cb, LV_EVENT_ALL, NULL);
}
void loader_event_init(void)
{
    lv_obj_add_style(guider_ui.loader_loadarc, &arc_style, LV_STATE_DEFAULT);

}
void saved_event_init(void)
{
    lv_obj_add_event_cb(guider_ui.saved_btnsavecontinue, load_disbtn_home_cb, LV_EVENT_ALL, NULL);
}

void event_cb(void)
{
    lightness_act = 0;
    hue_act = 180;
    
    lv_anim_init(&ani_en_btn_click);
    lv_anim_set_exec_cb(&ani_en_btn_click, (lv_anim_exec_xcb_t) en_click_anim_cb);
    lv_anim_set_values(&ani_en_btn_click, 0, LV_DEMO_PRINTER_ANIM_TIME);
    lv_anim_set_time(&ani_en_btn_click, LV_DEMO_PRINTER_ANIM_TIME);
}

/**
 * Create a demo application
 */

void custom_init(lv_ui *ui)
{
    setup_scr_home(ui);
    event_cb();
    home_event_init();
    lv_anim_set_var(&ani_en_btn_click, guider_ui.home_imgbtncopy);
    lv_anim_start(&ani_en_btn_click);
    cur_scr = SCR_HOME;
    ui->copyhome = NULL;
    ui->copynext = NULL;
    ui->scanhome = NULL;
    ui->prthome = NULL;
    ui->prtusb = NULL;
    ui->prtmb = NULL;
    ui->printit = NULL;
    ui->setup = NULL;
    ui->loader = NULL;
    ui->saved = NULL;

    lv_scr_load(ui->home);
}

