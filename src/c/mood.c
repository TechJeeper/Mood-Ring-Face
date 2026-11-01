#include "mood.h"
#include "settings.h"

static MoodState s_state;
static AppTimer *s_demo_timer;
static uint8_t s_demo_idx;
static MoodChangedFn s_changed;

static void notify_changed(void) {
  if (s_changed) {
    s_changed();
  }
}

static MoodLevel level_from_bpm(int bpm) {
  const FaceSettings *s = settings_get();
  if (bpm <= s->relaxed_max) {
    return MOOD_RELAXED;
  }
  if (bpm <= s->calm_max) {
    return MOOD_CALM;
  }
  if (bpm <= s->alert_max) {
    return MOOD_ALERT;
  }
  return MOOD_ANXIOUS;
}

static void apply_bpm(int bpm, bool has_hr) {
  MoodLevel prev = s_state.level;
  int prev_bpm = s_state.bpm;
  bool prev_hr = s_state.has_hr;

  s_state.has_hr = has_hr;
  s_state.bpm = has_hr ? bpm : 0;
  if (has_hr) {
    s_state.level = level_from_bpm(bpm);
  } else if (!settings_get()->demo_mode) {
    s_state.level = MOOD_CALM;
  }

  if (prev != s_state.level || prev_bpm != s_state.bpm || prev_hr != s_state.has_hr) {
    notify_changed();
  }
}

#if defined(PBL_HEALTH)
static bool read_live_hr(int *out_bpm) {
  HealthServiceAccessibilityMask mask =
      health_service_metric_accessible(HealthMetricHeartRateBPM, time(NULL), time(NULL));
  if (!(mask & HealthServiceAccessibilityMaskAvailable)) {
    return false;
  }
  HealthValue value = health_service_peek_current_value(HealthMetricHeartRateBPM);
  if (value <= 0) {
    return false;
  }
  *out_bpm = (int)value;
  return true;
}

static void health_handler(HealthEventType event, void *context) {
  (void)context;
  if (event == HealthEventHeartRateUpdate ||
      event == HealthEventSignificantUpdate) {
    mood_refresh();
  }
}
#endif

static void demo_tick(void *ctx) {
  (void)ctx;
  s_demo_timer = NULL;
  if (!settings_get()->demo_mode) {
    return;
  }
  if (s_state.has_hr) {
    // Live HR wins over demo cycling.
    s_demo_timer = app_timer_register(4000, demo_tick, NULL);
    return;
  }
  s_demo_idx = (uint8_t)((s_demo_idx + 1) % MOOD_COUNT);
  s_state.level = (MoodLevel)s_demo_idx;
  static const int demo_bpm[MOOD_COUNT] = {58, 72, 96, 118};
  s_state.bpm = demo_bpm[s_demo_idx];
  notify_changed();
  s_demo_timer = app_timer_register(4000, demo_tick, NULL);
}

void mood_set_changed_callback(MoodChangedFn cb) {
  s_changed = cb;
}

void mood_on_settings_changed(void) {
  if (s_demo_timer) {
    app_timer_cancel(s_demo_timer);
    s_demo_timer = NULL;
  }
  mood_refresh();
  if (settings_get()->demo_mode) {
    s_demo_timer = app_timer_register(4000, demo_tick, NULL);
  }
  notify_changed();
}

void mood_init(void) {
  s_state.level = MOOD_CALM;
  s_state.bpm = 0;
  s_state.has_hr = false;
  s_demo_idx = 0;

#if defined(PBL_HEALTH)
  health_service_events_subscribe(health_handler, NULL);
#endif
  mood_refresh();

  if (settings_get()->demo_mode) {
    s_demo_timer = app_timer_register(4000, demo_tick, NULL);
  }
}

void mood_deinit(void) {
  if (s_demo_timer) {
    app_timer_cancel(s_demo_timer);
    s_demo_timer = NULL;
  }
#if defined(PBL_HEALTH)
  health_service_events_unsubscribe();
#endif
}

void mood_refresh(void) {
  int bpm = 0;
  bool has_hr = false;

#if defined(PBL_HEALTH)
  has_hr = read_live_hr(&bpm);
#endif

  if (has_hr) {
    apply_bpm(bpm, true);
  } else if (settings_get()->demo_mode) {
    // Keep cycling level; ensure bpm label has a value.
    if (s_state.bpm <= 0) {
      static const int demo_bpm[MOOD_COUNT] = {58, 72, 96, 118};
      s_state.bpm = demo_bpm[s_state.level];
    }
    s_state.has_hr = false;
  } else {
    apply_bpm(0, false);
  }
}

const MoodState *mood_get(void) {
  return &s_state;
}

const char *mood_label(MoodLevel level) {
  switch (level) {
    case MOOD_RELAXED: return "RELAXED";
    case MOOD_CALM:    return "CALM";
    case MOOD_ALERT:   return "ALERT";
    case MOOD_ANXIOUS: return "ANXIOUS";
    default:           return "CALM";
  }
}

GColor mood_bg_color(MoodLevel level) {
#if defined(PBL_COLOR)
  switch (level) {
    case MOOD_RELAXED: return GColorDukeBlue;
    case MOOD_CALM:    return GColorVividCerulean;
    case MOOD_ALERT:   return GColorChromeYellow;
    case MOOD_ANXIOUS: return GColorDarkCandyAppleRed;
    default:           return GColorVividCerulean;
  }
#else
  (void)level;
  return GColorBlack;
#endif
}

GColor mood_fg_color(MoodLevel level) {
#if defined(PBL_COLOR)
  // Yellow alert needs dark ink; everything else is light.
  if (level == MOOD_ALERT) {
    return GColorBlack;
  }
  return GColorWhite;
#else
  (void)level;
  return GColorWhite;
#endif
}
