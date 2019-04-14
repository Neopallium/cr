#include <stdio.h>
#include <stdbool.h>

#include "fake_gui.h"

//
// Fake GUI callback interface
//
static cb_mouse_event_func g_cb_mouse = nullptr;
static void *g_cb_mouse_userdata = nullptr;
int fake_register_mouse_events(cb_mouse_event_func cb, void *userdata) {
    // Only support one callback.
    g_cb_mouse = cb;
    g_cb_mouse_userdata = userdata;
    return 0;
}
int fake_send_mouse_event(double x, double y, int buttons) {
    if(g_cb_mouse) {
        return g_cb_mouse(g_cb_mouse_userdata, x, y, buttons);
    } else {
        fprintf(stdout, "No mouse event callback registered\n");
    }
    return 0;
}
static cb_key_event_func g_cb_key = nullptr;
static void *g_cb_key_userdata = nullptr;
int fake_register_key_events(cb_key_event_func cb, void *userdata) {
    // Only support one callback.
    g_cb_key = cb;
    g_cb_key_userdata = userdata;
    return 0;
}
int fake_send_key_event(int key, bool is_down) {
    if(g_cb_key) {
        return g_cb_key(g_cb_key_userdata, key, is_down);
    } else {
        fprintf(stdout, "No key event callback registered\n");
    }
    return 0;
}

