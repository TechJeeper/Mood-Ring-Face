#include "face.h"
#include "mood.h"
#include "settings.h"
#include "characters.h"

static Window *s_window;
static Layer *s_layer;

static GBitmap *s_faces[MOOD_COUNT];
static int s_loaded_character = -1;

static int s_hour;
static int s_minute;
static int s_second;
static int s_wday;
static int s_mday;
static int s_mon;
static int s_pulse;  // 0..7 for subtle bounce

static void unload_faces(void) {
  for (int i = 0; i < MOOD_COUNT; i++) {
    if (s_faces[i]) {
      gbitmap_destroy(s_faces[i]);
      s_faces[i] = NULL;
    }
  }
  s_loaded_character = -1;
}

static void load_faces(CharacterId id) {
  if (s_loaded_character == (int)id) {
    return;
  }
  unload_faces();
  for (int i = 0; i < MOOD_COUNT; i++) {
    s_faces[i] = gbitmap_create_with_resource(character_resource(id, (MoodLevel)i));
  }
  s_loaded_character = (int)id;
}

static bool use_24h(void) {
  const FaceSettings *s = settings_get();
  if (s->time_format == TIME_FMT_24H) return true;
  if (s->time_format == TIME_FMT_12H) return false;
  return clock_is_24h_style();
}

static void format_time(char *buf, size_t n) {
  int hour = s_hour;
  const bool is24 = use_24h();
  if (!is24) {
    hour = hour % 12;
    if (hour == 0) hour = 12;
  }
  if (settings_get()->leading_zero) {
    snprintf(buf, n, "%02d:%02d", hour, s_minute);
  } else {
    snprintf(buf, n, "%d:%02d", hour, s_minute);
  }
}

static void format_date(char *buf, size_t n) {
  static const char *const DOW[] = {
    "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
  };
  static const char *const MON[] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
  };
  snprintf(buf, n, "%s %d %s", DOW[s_wday % 7], s_mday, MON[s_mon % 12]);
}

static void update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  const MoodState *mood = mood_get();
  const FaceSettings *cfg = settings_get();
  GColor bg = mood_bg_color(mood->level);
  GColor fg = mood_fg_color(mood->level);

  graphics_context_set_fill_color(ctx, bg);
  graphics_fill_rect(ctx, bounds, 0, GCornerNone);

  // Caption band at the bottom (mood + BPM).
  const int caption_h = 42;
  const int time_bottom = cfg->show_date ? 64 : 48;

  // Vertically center the face in the band between time and captions.
  GBitmap *face = s_faces[mood->level];
  if (face) {
    GSize sz = gbitmap_get_bounds(face).size;
    int bounce = 0;
    if (mood->has_hr || cfg->demo_mode) {
      int amp = 1 + (int)mood->level;
      bounce = ((s_pulse + (int)mood->level) % 8 < 4) ? amp : 0;
    }

    const int band_top = time_bottom;
    const int band_bot = bounds.size.h - caption_h;
    int face_y = band_top + (band_bot - band_top - sz.h) / 2 - bounce;
    if (face_y < band_top) {
      face_y = band_top;
    }
    if (face_y + sz.h > band_bot) {
      face_y = band_bot - sz.h;
    }

    GRect dest = GRect((bounds.size.w - sz.w) / 2, face_y, sz.w, sz.h);
#if defined(PBL_COLOR)
    graphics_context_set_compositing_mode(ctx, GCompOpSet);
#else
    graphics_context_set_compositing_mode(ctx, GCompOpAssign);
#endif
    graphics_draw_bitmap_in_rect(ctx, face, dest);

#if defined(PBL_COLOR)
    {
      int cx = bounds.size.w / 2;
      int cy = face_y + sz.h / 2;
      int r = (sz.w / 2) + 4 + ((s_pulse % 4 == 0) ? 1 : 0);
      // Keep ring from eating into the caption band.
      int max_r = (bounds.size.h - caption_h) - cy - 2;
      if (r > max_r && max_r > 8) {
        r = max_r;
      }
      graphics_context_set_stroke_color(ctx, fg);
      graphics_context_set_stroke_width(ctx, 2);
      graphics_draw_circle(ctx, GPoint(cx, cy), r);
      graphics_context_set_stroke_width(ctx, 1);
      graphics_draw_circle(ctx, GPoint(cx, cy), r + 3);
    }
#endif
  }

  // Draw time/date last so they always sit above the sprite.
  char time_buf[16];
  format_time(time_buf, sizeof(time_buf));
  graphics_context_set_text_color(ctx, fg);
  graphics_draw_text(ctx, time_buf,
                     fonts_get_system_font(FONT_KEY_LECO_42_NUMBERS),
                     GRect(0, 2, bounds.size.w, 44),
                     GTextOverflowModeTrailingEllipsis,
                     GTextAlignmentCenter, NULL);

  if (cfg->show_date) {
    char date_buf[24];
    format_date(date_buf, sizeof(date_buf));
    graphics_draw_text(ctx, date_buf,
                       fonts_get_system_font(FONT_KEY_GOTHIC_14),
                       GRect(0, 44, bounds.size.w, 18),
                       GTextOverflowModeTrailingEllipsis,
                       GTextAlignmentCenter, NULL);
  }

  if (cfg->show_mood_label) {
    graphics_context_set_text_color(ctx, fg);
    graphics_draw_text(ctx, mood_label(mood->level),
                       fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD),
                       GRect(0, bounds.size.h - 40, bounds.size.w, 22),
                       GTextOverflowModeTrailingEllipsis,
                       GTextAlignmentCenter, NULL);
  }

  if (cfg->show_bpm) {
    char bpm_buf[24];
    if (mood->has_hr) {
      snprintf(bpm_buf, sizeof(bpm_buf), "%d BPM", mood->bpm);
    } else if (cfg->demo_mode) {
      snprintf(bpm_buf, sizeof(bpm_buf), "%d BPM · demo", mood->bpm);
    } else {
      snprintf(bpm_buf, sizeof(bpm_buf), "No HR");
    }
    graphics_context_set_text_color(ctx, fg);
    graphics_draw_text(ctx, bpm_buf,
                       fonts_get_system_font(FONT_KEY_GOTHIC_14),
                       GRect(0, bounds.size.h - 20, bounds.size.w, 18),
                       GTextOverflowModeTrailingEllipsis,
                       GTextAlignmentCenter, NULL);
  }

  (void)s_second;
}

static void mark_dirty(void) {
  if (s_layer) {
    layer_mark_dirty(s_layer);
  }
  if (s_window) {
    window_set_background_color(s_window, mood_bg_color(mood_get()->level));
  }
}

void face_init(Window *window) {
  s_window = window;
  load_faces((CharacterId)settings_get()->character);

  Layer *root = window_get_root_layer(window);
  s_layer = layer_create(layer_get_bounds(root));
  layer_set_update_proc(s_layer, update_proc);
  layer_add_child(root, s_layer);

  face_tick();
  mark_dirty();
}

void face_deinit(void) {
  if (s_layer) {
    layer_destroy(s_layer);
    s_layer = NULL;
  }
  unload_faces();
  s_window = NULL;
}

void face_tick(void) {
  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  s_hour = t->tm_hour;
  s_minute = t->tm_min;
  s_second = t->tm_sec;
  s_wday = t->tm_wday;
  s_mday = t->tm_mday;
  s_mon = t->tm_mon;
  s_pulse = (s_pulse + 1) % 8;
  mark_dirty();
}

void face_apply_settings(void) {
  load_faces((CharacterId)settings_get()->character);
  mark_dirty();
}

void face_on_mood_changed(void) {
  mark_dirty();
}
