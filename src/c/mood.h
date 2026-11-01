#pragma once

#include <pebble.h>

typedef enum {
  MOOD_RELAXED = 0,
  MOOD_CALM,
  MOOD_ALERT,
  MOOD_ANXIOUS,
  MOOD_COUNT
} MoodLevel;

typedef struct {
  MoodLevel level;
  int bpm;
  bool has_hr;
} MoodState;

typedef void (*MoodChangedFn)(void);

void mood_init(void);
void mood_deinit(void);
void mood_refresh(void);
void mood_on_settings_changed(void);
void mood_set_changed_callback(MoodChangedFn cb);
const MoodState *mood_get(void);
const char *mood_label(MoodLevel level);
GColor mood_bg_color(MoodLevel level);
GColor mood_fg_color(MoodLevel level);
