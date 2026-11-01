#pragma once

#include <pebble.h>
#include <stdbool.h>

typedef enum {
  TIME_FMT_SYSTEM = 0,
  TIME_FMT_12H,
  TIME_FMT_24H
} TimeFormat;

typedef struct {
  TimeFormat time_format;
  bool leading_zero;
  bool show_bpm;
  bool show_mood_label;
  bool show_date;
  bool demo_mode;
  int relaxed_max;
  int calm_max;
  int alert_max;
} FaceSettings;

typedef void (*SettingsChangedFn)(void);

void settings_init(void);
void settings_deinit(void);
void settings_set_changed_callback(SettingsChangedFn cb);
const FaceSettings *settings_get(void);
