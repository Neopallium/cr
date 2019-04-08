
#ifndef __FAKE_GUI_H__
#define __FAKE_GUI_H__

// only include cr.h for 'CR_EXPORT'
#include "../cr.h"

//
// Fake GUI callback interface
// This interface doesn't need to know anything about reloadable plugins.
//
typedef int (*cb_mouse_event_func)(void *userdata, int x, int y, int buttons);
typedef int (*cb_key_event_func)(void *userdata, int key, bool is_down);

CR_EXPORT int fake_register_mouse_events(cb_mouse_event_func cb, void *userdata);
CR_EXPORT int fake_send_mouse_event(int x, int y, int buttons);
CR_EXPORT int fake_register_key_events(cb_key_event_func cb, void *userdata);
CR_EXPORT int fake_send_key_event(int key, bool is_down);

#endif
