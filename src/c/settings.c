#include "settings.h"
#include "characters.h"

enum {
  PERSIST_TIME_FORMAT = 1,
  PERSIST_LEADING_ZERO = 2,
  PERSIST_SHOW_BPM = 3,
  PERSIST_SHOW_MOOD = 4,
  PERSIST_SHOW_DATE = 5,
  PERSIST_DEMO_MODE = 6,
  PERSIST_RELAXED_MAX = 7,
  PERSIST_CALM_MAX = 8,
  PERSIST_ALERT_MAX = 9,
  PERSIST_CHARACTER = 10,
};

static FaceSettings s_settings;
static SettingsChangedFn s_changed;
static bool s_inbox_open;

static int clamp_int(int v, int lo, int hi) {
  if (v < lo) return lo;
  if (v > hi) return hi;
  return v;
}

static void normalize_thresholds(void) {
  s_settings.relaxed_max = clamp_int(s_settings.relaxed_max, 40, 120);
  s_settings.calm_max = clamp_int(s_settings.calm_max, s_settings.relaxed_max + 1, 160);
  s_settings.alert_max = clamp_int(s_settings.alert_max, s_settings.calm_max + 1, 200);
  s_settings.character = clamp_int(s_settings.character, 0, CHARACTER_COUNT - 1);
}

static void settings_set_defaults(void) {
  s_settings.time_format = TIME_FMT_SYSTEM;
  s_settings.leading_zero = true;
  s_settings.show_bpm = true;
  s_settings.show_mood_label = true;
  s_settings.show_date = false;
  s_settings.demo_mode = false;
  s_settings.character = CHAR_MIA;
  s_settings.relaxed_max = 65;
  s_settings.calm_max = 85;
  s_settings.alert_max = 105;
}

static void settings_load(void) {
  settings_set_defaults();
  if (persist_exists(PERSIST_TIME_FORMAT)) {
    s_settings.time_format = (TimeFormat)clamp_int(
        persist_read_int(PERSIST_TIME_FORMAT), TIME_FMT_SYSTEM, TIME_FMT_24H);
  }
  if (persist_exists(PERSIST_LEADING_ZERO)) {
    s_settings.leading_zero = persist_read_bool(PERSIST_LEADING_ZERO);
  }
  if (persist_exists(PERSIST_SHOW_BPM)) {
    s_settings.show_bpm = persist_read_bool(PERSIST_SHOW_BPM);
  }
  if (persist_exists(PERSIST_SHOW_MOOD)) {
    s_settings.show_mood_label = persist_read_bool(PERSIST_SHOW_MOOD);
  }
  if (persist_exists(PERSIST_SHOW_DATE)) {
    s_settings.show_date = persist_read_bool(PERSIST_SHOW_DATE);
  }
  if (persist_exists(PERSIST_DEMO_MODE)) {
    s_settings.demo_mode = persist_read_bool(PERSIST_DEMO_MODE);
  }
  if (persist_exists(PERSIST_CHARACTER)) {
    s_settings.character = persist_read_int(PERSIST_CHARACTER);
  }
  if (persist_exists(PERSIST_RELAXED_MAX)) {
    s_settings.relaxed_max = persist_read_int(PERSIST_RELAXED_MAX);
  }
  if (persist_exists(PERSIST_CALM_MAX)) {
    s_settings.calm_max = persist_read_int(PERSIST_CALM_MAX);
  }
  if (persist_exists(PERSIST_ALERT_MAX)) {
    s_settings.alert_max = persist_read_int(PERSIST_ALERT_MAX);
  }
  normalize_thresholds();
}

static void settings_save(void) {
  normalize_thresholds();
  persist_write_int(PERSIST_TIME_FORMAT, (int)s_settings.time_format);
  persist_write_bool(PERSIST_LEADING_ZERO, s_settings.leading_zero);
  persist_write_bool(PERSIST_SHOW_BPM, s_settings.show_bpm);
  persist_write_bool(PERSIST_SHOW_MOOD, s_settings.show_mood_label);
  persist_write_bool(PERSIST_SHOW_DATE, s_settings.show_date);
  persist_write_bool(PERSIST_DEMO_MODE, s_settings.demo_mode);
  persist_write_int(PERSIST_CHARACTER, s_settings.character);
  persist_write_int(PERSIST_RELAXED_MAX, s_settings.relaxed_max);
  persist_write_int(PERSIST_CALM_MAX, s_settings.calm_max);
  persist_write_int(PERSIST_ALERT_MAX, s_settings.alert_max);
}

// Clay may deliver ints or numeric strings depending on control type.
static int tuple_as_int(Tuple *t, int fallback) {
  if (!t) return fallback;
  if (t->type == TUPLE_INT) return (int)t->value->int32;
  if (t->type == TUPLE_CSTRING) return atoi(t->value->cstring);
  return fallback;
}

static bool tuple_as_bool(Tuple *t, bool fallback) {
  if (!t) return fallback;
  if (t->type == TUPLE_INT) return t->value->int32 != 0;
  if (t->type == TUPLE_CSTRING) {
    return strcmp(t->value->cstring, "true") == 0 ||
           strcmp(t->value->cstring, "1") == 0;
  }
  return fallback;
}

static void inbox_received(DictionaryIterator *iter, void *context) {
  (void)context;
  Tuple *t;

  t = dict_find(iter, MESSAGE_KEY_TimeFormat);
  if (t) {
    s_settings.time_format = (TimeFormat)clamp_int(
        tuple_as_int(t, (int)s_settings.time_format), TIME_FMT_SYSTEM, TIME_FMT_24H);
  }
  t = dict_find(iter, MESSAGE_KEY_LeadingZero);
  if (t) {
    s_settings.leading_zero = tuple_as_bool(t, s_settings.leading_zero);
  }
  t = dict_find(iter, MESSAGE_KEY_ShowBpm);
  if (t) {
    s_settings.show_bpm = tuple_as_bool(t, s_settings.show_bpm);
  }
  t = dict_find(iter, MESSAGE_KEY_ShowMoodLabel);
  if (t) {
    s_settings.show_mood_label = tuple_as_bool(t, s_settings.show_mood_label);
  }
  t = dict_find(iter, MESSAGE_KEY_ShowDate);
  if (t) {
    s_settings.show_date = tuple_as_bool(t, s_settings.show_date);
  }
  t = dict_find(iter, MESSAGE_KEY_DemoMode);
  if (t) {
    s_settings.demo_mode = tuple_as_bool(t, s_settings.demo_mode);
  }
  t = dict_find(iter, MESSAGE_KEY_Character);
  if (t) {
    s_settings.character = tuple_as_int(t, s_settings.character);
  }
  t = dict_find(iter, MESSAGE_KEY_RelaxedMax);
  if (t) {
    s_settings.relaxed_max = tuple_as_int(t, s_settings.relaxed_max);
  }
  t = dict_find(iter, MESSAGE_KEY_CalmMax);
  if (t) {
    s_settings.calm_max = tuple_as_int(t, s_settings.calm_max);
  }
  t = dict_find(iter, MESSAGE_KEY_AlertMax);
  if (t) {
    s_settings.alert_max = tuple_as_int(t, s_settings.alert_max);
  }

  settings_save();
  if (s_changed) {
    s_changed();
  }
}

void settings_init(void) {
  settings_load();
  if (!s_inbox_open) {
    app_message_register_inbox_received(inbox_received);
    // Clay sends the full settings dict at once — use max buffers.
    app_message_open(app_message_inbox_size_maximum(),
                     app_message_outbox_size_maximum());
    s_inbox_open = true;
  }
}

void settings_deinit(void) {
  settings_save();
  if (s_inbox_open) {
    app_message_deregister_callbacks();
    s_inbox_open = false;
  }
}

void settings_set_changed_callback(SettingsChangedFn cb) {
  s_changed = cb;
}

const FaceSettings *settings_get(void) {
  return &s_settings;
}
