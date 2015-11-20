#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_text_layer;
static PropertyAnimation *s_prop_animation;

static bool s_toggle;

static void animation_started(Animation *animation, void *data) {
  text_layer_set_text(s_text_layer, "Started");
}

static void animation_stopped(Animation *animation, bool finished, void *data) {
  text_layer_set_text(s_text_layer, "Stopped");
}

static void click_handler(ClickRecognizerRef recognizer, void *context) {
  Layer *layer = text_layer_get_layer(s_text_layer);
  GRect to_rect = (s_toggle) ? GRect(4, 4, 120, 60) : GRect(84, 92, 60, 60);

  s_toggle = !s_toggle;

  s_prop_animation = property_animation_create_layer_frame(layer, NULL, &to_rect);
  Animation *animation = property_animation_get_animation(s_prop_animation);
  switch (click_recognizer_get_button_id(recognizer)) {
    case BUTTON_ID_UP:
      animation_set_curve(animation, AnimationCurveEaseOut);
      break;

    case BUTTON_ID_DOWN:
      animation_set_curve(animation, AnimationCurveEaseIn);
      break;

    default:
    case BUTTON_ID_SELECT:
      animation_set_curve(animation, AnimationCurveEaseInOut);
      break;
  }

  animation_set_duration(animation, 1000);
  animation_set_handlers(animation, (AnimationHandlers) {
    .started = (AnimationStartedHandler) animation_started,
    .stopped = (AnimationStoppedHandler) animation_stopped,
  }, NULL);
  animation_schedule(animation);
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_UP, click_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, click_handler);
}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(s_main_window);
  GRect bounds = layer_get_frame(window_layer);

  s_text_layer = text_layer_create(GRect(0, 0, 60, 60));
  text_layer_set_text(s_text_layer, "Press a button!");
  layer_add_child(window_layer, text_layer_get_layer(s_text_layer));
}

static void main_window_unload(Window *window) {
  text_layer_destroy(s_text_layer);
}

static void init() {
  s_main_window = window_create();
  window_set_click_config_provider(s_main_window, click_config_provider);
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });
  window_stack_push(s_main_window, true);
}

static void deinit(void) {
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
