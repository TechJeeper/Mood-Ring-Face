#include <pebble.h>
#include "face.h"
#include "mood.h"
#include "settings.h"

static Window *s_window;
static AppTimer *s_pulse_timer;

static void on_tick(struct tm *tick_time, TimeUnits units_changed) {
  (void)tick_time;
  (void)units_changed;
  face_tick();
}

static void pulse_cb(void *ctx) {
  (void)ctx;
  s_pulse_timer = app_timer_register(250, pulse_cb, NULL);
  face_tick();
}

static void on_settings_changed(void) {
  mood_on_settings_changed();
  face_apply_settings();
}

static void on_mood_changed(void) {
  face_on_mood_changed();
}

static void window_load(Window *window) {
  face_init(window);
}

static void window_unload(Window *window) {
  (void)window;
  face_deinit();
}

static void init(void) {
  settings_init();
  settings_set_changed_callback(on_settings_changed);

  mood_set_changed_callback(on_mood_changed);
  mood_init();

  s_window = window_create();
  window_set_background_color(s_window, mood_bg_color(mood_get()->level));
  window_set_window_handlers(s_window, (WindowHandlers){
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(s_window, true);

  tick_timer_service_subscribe(MINUTE_UNIT, on_tick);
  s_pulse_timer = app_timer_register(250, pulse_cb, NULL);
}

static void deinit(void) {
  if (s_pulse_timer) {
    app_timer_cancel(s_pulse_timer);
    s_pulse_timer = NULL;
  }
  tick_timer_service_unsubscribe();
  mood_deinit();
  settings_deinit();
  window_destroy(s_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
