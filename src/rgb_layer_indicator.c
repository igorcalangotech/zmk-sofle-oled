/*
 * SPDX-License-Identifier: MIT
 *
 * Underglow colours by keymap layer index (Eyelash Sofle), inspired by Voyager
 * per-layer RGB. Solid HSV on NAV/UTIL/MOUSE/FN5; restores Kconfig defaults
 * for BASE/SYM/BT layers.
 */

#include <zephyr/kernel.h>
#include <zephyr/init.h>

#include <zmk/event_manager.h>
#include <zmk/events/layer_state_changed.h>
#include <zmk/keymap.h>
#include <zmk/rgb_underglow.h>

/* Must match rgb_underglow.c */
#define UG_EFFECT_SOLID 0

enum { AMBIENT_MODE, LAYER_SOLID_MODE };

static uint8_t indicator_mode = AMBIENT_MODE;

static struct k_work_delayable apply_work;

static struct zmk_led_hsb hsb_for_layer(zmk_keymap_layer_index_t idx)
{
    switch (idx) {
    case 2:
        return (struct zmk_led_hsb){.h = 195, .s = 100, .b = 100};
    case 3:
        return (struct zmk_led_hsb){.h = 265, .s = 100, .b = 100};
    case 4:
        return (struct zmk_led_hsb){.h = 169, .s = 100, .b = 100};
    case 5:
        return (struct zmk_led_hsb){.h = 74, .s = 100, .b = 100};
    default:
        return (struct zmk_led_hsb){.h = CONFIG_ZMK_RGB_UNDERGLOW_HUE_START,
                                    .s = CONFIG_ZMK_RGB_UNDERGLOW_SAT_START,
                                    .b = CONFIG_ZMK_RGB_UNDERGLOW_BRT_START};
    }
}

static bool layer_index_is_special(zmk_keymap_layer_index_t idx)
{
    return idx == 2 || idx == 3 || idx == 4 || idx == 5;
}

static void apply_work_handler(struct k_work *work)
{
    ARG_UNUSED(work);

    bool rgb_on;
    if (zmk_rgb_underglow_get_state(&rgb_on) != 0 || !rgb_on) {
        return;
    }

    const zmk_keymap_layer_index_t idx = zmk_keymap_highest_layer_active();
    const bool special = layer_index_is_special(idx);
    const struct zmk_led_hsb color = hsb_for_layer(idx);

    if (special) {
        if (indicator_mode != LAYER_SOLID_MODE) {
            zmk_rgb_underglow_select_effect(UG_EFFECT_SOLID);
            indicator_mode = LAYER_SOLID_MODE;
        }
        zmk_rgb_underglow_set_hsb(color);
    } else {
        if (indicator_mode != AMBIENT_MODE) {
            zmk_rgb_underglow_select_effect(CONFIG_ZMK_RGB_UNDERGLOW_EFF_START);
            indicator_mode = AMBIENT_MODE;
        }
        zmk_rgb_underglow_set_hsb(color);
    }
}

static int layer_rgb_listener(const zmk_event_t *eh)
{
    if (as_zmk_layer_state_changed(eh) == NULL) {
        return ZMK_EV_EVENT_BUBBLE;
    }

    (void)k_work_reschedule(&apply_work, K_MSEC(5));
    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(rgb_layer_indicator, layer_rgb_listener);
ZMK_SUBSCRIPTION(rgb_layer_indicator, zmk_layer_state_changed);

static int rgb_layer_indicator_init(void)
{
    k_work_init_delayable(&apply_work, apply_work_handler);
    (void)k_work_reschedule(&apply_work, K_MSEC(80));
    return 0;
}

SYS_INIT(rgb_layer_indicator_init, APPLICATION, 91);
