
#ifndef __CB_HOST_H__
#define __CB_HOST_H__

// only include cr.h for 'CR_EXPORT'
#include "../cr.h"

//
// Host-side trampolines to safely call plugin's callbacks.
// These trampoline functions MUST NOT be reloadable.  They need fixed addresses.
// Either define them in the host or in a shared library that isn't reloadable.
//
CR_EXPORT int tr_mouse_event_func(void *userdata, int x, int y, int buttons);
CR_EXPORT int tr_key_event_func(void *userdata, int key, bool is_down);

#endif

