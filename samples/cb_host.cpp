#include <cstdio>
#include <chrono>
#include <thread>

#define CR_HOST CR_UNSAFE // try to best manage static states
#define CR_DEBUG
#include "../cr.h"

#include "fake_gui.h"
#include "cb_host.h"

//
// Trampolines: Defined in the Host so the function address doesn't change.
//
int tr_mouse_event_func(void *userdata, int x, int y, int buttons) {
    CR_CLOSURE_CALL_START(cb_mouse_event_func, userdata, true)
    if (cb_func) {
        return cb_func(closure->userdata, x, y, buttons);
    } else {
        fprintf(stdout, "No Plugin callback: MOUSE: x=%d, y=%d, buttons=0x%x\n", x, y, buttons);
    }
    CR_CLOSURE_CALL_END

    return 0;
}

int tr_key_event_func(void *userdata, int key, bool is_down) {
    CR_CLOSURE_CALL_START(cb_key_event_func, userdata, true)
    if (cb_func) {
        return cb_func(closure->userdata, key, is_down);
    } else {
        fprintf(stdout, "No Plugin callback: KEY: key=%d, is_down=%d\n", key, is_down);
    }
    CR_CLOSURE_CALL_END

    return 0;
}

const char *plugin = CR_DEPLOY_PATH "/" CR_PLUGIN("cb_guest");

int main(int argc, char *argv[]) {
    cr_plugin ctx;

    // the host application should initalize a plugin with a context, a plugin
    // filename without extension and the full path to the plugin
    cr_plugin_load(ctx, plugin);

    // call the plugin update function with the plugin context to execute it
    // at any frequency matters to you
    while (true) {
        cr_plugin_update(ctx);
        fflush(stdout);
        fflush(stderr);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        // fake events
        fake_send_key_event(10, true);
        fake_send_mouse_event(1, 1, 0x01);
    }

    // at the end do not forget to cleanup the plugin context, as it needs to
    // allocate some memory to track internal and plugin states
    cr_plugin_close(ctx);
    return 0;
}
