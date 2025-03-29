/*
 * Copyright (c) 2020 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#define DT_DRV_COMPAT zmk_behavior_move_window

#include <stdbool.h>

#include <zephyr/device.h>
#include <drivers/behavior.h>
#include <zephyr/logging/log.h>

#include <zmk/event_manager.h>
#include <zmk/events/keycode_state_changed.h>
#include <zmk/behavior.h>

#include <dt-bindings/zmk/move_window.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

static bool is_enabled = false;

static int on_keymap_binding_pressed(struct zmk_behavior_binding *binding,
                                     struct zmk_behavior_binding_event event) {
    switch (binding->param1) {
        case MWND_ENABLE: {
            LOG_DBG("move_window ENABLE pressed");
            is_enabled = true;
        } break;

        case MWND_UP:
        case MWND_DOWN:
        case MWND_LEFT:
        case MWND_RIGHT: {
            LOG_DBG("move_window direction keycode 0x%02X pressed", binding->param1);
            if (is_enabled) {
                LOG_DBG("move_window moving window...");
                raise_zmk_keycode_state_changed_from_encoded(LGUI, true, event.timestamp);
                raise_zmk_keycode_state_changed_from_encoded(binding->param1, true, event.timestamp);
            }
        } break;
    }

    return ZMK_BEHAVIOR_OPAQUE;
}

static int on_keymap_binding_released(struct zmk_behavior_binding *binding,
                                      struct zmk_behavior_binding_event event) {
    switch (binding->param1) {
        case MWND_ENABLE: {
            LOG_DBG("move_window ENABLE released");
            raise_zmk_keycode_state_changed_from_encoded(LGUI, false, event.timestamp);
            is_enabled = false;
        } break;

        case MWND_UP:
        case MWND_DOWN:
        case MWND_LEFT:
        case MWND_RIGHT: {
            LOG_DBG("move_window direction keycode 0x%02X released", binding->param1);
            raise_zmk_keycode_state_changed_from_encoded(binding->param1, false, event.timestamp);
        } break;
    }

    return ZMK_BEHAVIOR_OPAQUE;
}

static const struct behavior_driver_api behavior_move_window_driver_api = {
    .binding_pressed = on_keymap_binding_pressed,
    .binding_released = on_keymap_binding_released,
};

BEHAVIOR_DT_INST_DEFINE(0, NULL, NULL, NULL, NULL, POST_KERNEL,
                        CONFIG_KERNEL_INIT_PRIORITY_DEFAULT, &behavior_move_window_driver_api);
