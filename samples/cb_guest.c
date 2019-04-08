#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include "../cr.h"

#include "fake_gui.h"
#include "cb_host.h"

// To save states automatically from previous instance to a new loaded one, use CR_STATE flag on statics/globals.
// This will create a new data section in the binary for transferable states between instances that will be copied
// and kept around to overwrite the new instance initial states. That means that things that were initialized in
// a previous loaded states will continue from where they were.
static unsigned int CR_STATE version = 1;
// But unfortunately new added states in new instances (post the fact states) do not work because they will be
// overwritten by previous states (in this case, inexisting ones being zeroed), so uncommenting the following line
// will have a value of zero.
//static int32_t CR_STATE sad_state = 2;
// For this reason, cr is better for runtime modifing existing stuff than adding/removing new stuff.
// At the sime time, if we remove a state variable, the reload will safely fail with the error CR_STATE_INVALIDATED.
// A rollback will be effectued and this error can be dealt client side, for exemple, by poping a dialog box asking
// to force reload cleaning up states (restarting the client from scratch with the new version).

void hello() {
    // this demonstrate how to transfer an state between instances by using CR_STATE tag.
    // in this case, we track a flag to indicate if hello was print or not, so each new reload
    // after the initial one will not print the hello world message.
    static bool CR_STATE said_hello = false;
    if (!said_hello) {
        said_hello = true;
        fprintf(stdout, "hello world! ");
    }
    static int skip = 0;
    if (++skip%5 == 0)
	    fprintf(stdout, "--- Hello: test5\n");
}

void test_crash() {
    int *addr = NULL; (void)addr; // warning
    // to test crash protection, uncomment the following line
    //int i = *addr;
}

//
// Fake GUI callbacks
//
static struct cr_symbol * CR_STATE g_plugin_mouse_cb = NULL;
static struct cr_symbol * CR_STATE g_plugin_key_cb = NULL;
// Normally these `cr_closure` objects would be embedded in the callback's `userdata` object
static struct cr_closure * CR_STATE g_plugin_mouse_closure = NULL;
static struct cr_closure * CR_STATE g_plugin_key_closure = NULL;
static void registery_fake_gui_callbacks(struct cr_plugin *ctx) {
    if (!g_plugin_mouse_cb) {
        // Only need one instance of `cr_symbol` for each callback function.
        g_plugin_mouse_cb = cr_plugin_get_symbol(ctx, "plugin_mouse_events");
        // create one instance of `cr_closure` each time the callback function is registered.
        // this holds the callback's userdata value.
        g_plugin_mouse_closure = cr_symbol_new_closure(g_plugin_mouse_cb, ctx);
        // Use the trampoline from the Host to unwrap the `cr_closure` and all the callback function.
        fake_register_mouse_events(tr_mouse_event_func, g_plugin_mouse_closure);
    }
    if (!g_plugin_key_cb) {
        // Only need one instance of `cr_symbol` for each callback function.
        g_plugin_key_cb = cr_plugin_get_symbol(ctx, "plugin_key_events");
        // create one instance of `cr_closure` each time the callback function is registered.
        // this holds the callback's userdata value.
        g_plugin_key_closure = cr_symbol_new_closure(g_plugin_key_cb, ctx);
        // Use the trampoline from the Host to unwrap the `cr_closure` and all the callback function.
        fake_register_key_events(tr_key_event_func, g_plugin_key_closure);
    }
}

void test_callback_crash() {
    int *addr = NULL; (void)addr; // warning
    // to test crash protection, uncomment the following line
    int i = *addr;
}

//
// Need to export our plugin's callbacks (use any name)
//
CR_EXPORT int plugin_mouse_events(void *userdata, int x, int y, int buttons) {
    struct cr_plugin *ctx = (struct cr_plugin *)userdata;
    fprintf(stdout, "cb_guest: mouse event callback. (x=%d,y=%d,buttons=0x%x): ver=%d\n",
            x, y, buttons, ctx->version);
    //test_callback_crash();
    return 0;
}
CR_EXPORT int plugin_key_events(void *userdata, int key, bool is_down) {
    struct cr_plugin *ctx = (struct cr_plugin *)userdata;
    fprintf(stdout, "cb_guest: key event callback. (key=%d,is_down=%d): ver=%d\n",
            key, is_down, ctx->version);
    return 0;
}

CR_EXPORT int cr_main(struct cr_plugin *ctx, enum cr_op operation) {
    assert(ctx);
    if (operation != CR_STEP) {
        fprintf(stdout, "OP: %s(%d)\n", operation == CR_LOAD ? "LOAD" : "UNLOAD", ctx->version);
        if (operation == CR_LOAD) {
            registery_fake_gui_callbacks(ctx);
        }
        int *addr = NULL; (void)addr; // warning
        // to test crash protection during load
        //int i = *addr;
        return 0;
    }

    // crash protection may cause the version to decrement. So we can test current version against one
    // tracked between instances with CR_STATE to signal that we're not running the most recent instance.
    if (ctx->version < version) {
        // a failure code is acessible in the `failure` variable from the `cr_plugin` context.
        // on windows this is the structured exception error code, for more info:
        //      https://msdn.microsoft.com/en-us/library/windows/desktop/ms679356(v=vs.85).aspx
        fprintf(stdout, "A rollback happened due to failure: %x!\n", ctx->failure);
    }
    version = ctx->version;

    // Not this does not carry state between instances (no CR_STATE), this means each time we load an instance
    // this value will be reset to its initial state (true), and then we can print the loaded instance version
    // one time only by instance version.
    static bool print_version = true;
    if (print_version) {
        fprintf(stdout, "loaded version: %d\n", ctx->version);

        // disable further printing for this instance only
        print_version = false;
    }
    hello();
    test_crash();
    //std::this_thread::sleep_for(std::chrono::milliseconds(500));
    return 0;
}
