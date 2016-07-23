#include <pebble.h>
Window *my_window;
static Layer *date_dots;
static Layer *clock_layer;
static GBitmap *texture;

typedef struct {
  int hours;
  int minutes;
} Time;    

typedef struct {
  GColor BackgroundColor;
  GColor DateDotColor;
  GColor CurrentDateDotColor;
  GColor WeekDayColor;
  GColor CurrentWeekDayColor;
  GColor AMDotColor;
  GColor PMDotColor;
  GColor MarkerColor;
  GColor RingColor;
  GColor HourHandColor;
  GColor MinuteHandColor;
  GColor MonthDotColor;
  GColor CenterKnobColor;
  GColor CenterDotColor;
} Theme;

Theme theme;

static void load_default_theme() {
  theme.BackgroundColor = GColorDarkGray;
  theme.DateDotColor = GColorBlack;
  theme.CurrentDateDotColor = GColorChromeYellow;
  theme.AMDotColor = GColorWhite;
  theme.PMDotColor = GColorBlack;
  theme.MarkerColor = GColorLightGray;
  theme.RingColor = GColorLightGray;
  theme.WeekDayColor = GColorBlack;
  theme.CurrentWeekDayColor = GColorChromeYellow;
  theme.HourHandColor = GColorChromeYellow;
  theme.MinuteHandColor = GColorWhite;
  theme.MonthDotColor = GColorChromeYellow;
  theme.CenterKnobColor = GColorWhite;
  theme.CenterDotColor = GColorLightGray;
}

// Persistent
#define SHOW_TEXTURE 222 // Use texture
#define SHOW_DAY_OF_WEEK 223 // Show Day of Week
#define THEME 224 


  
// Layout boundaries
#define HOUR_RING PBL_IF_ROUND_ELSE(50, 50) // HOUR HAND
#define MIN_RING PBL_IF_ROUND_ELSE(32, 32) // MIN HAND
#define MONTH_RING PBL_IF_ROUND_ELSE(14, 5) 
#define MONTH_DOT_RADIUS PBL_IF_ROUND_ELSE(4, 4)
#define RING_31 PBL_IF_ROUND_ELSE(27, 16) // 31st DOT
#define MON_RING PBL_IF_ROUND_ELSE(72, 68) 
#define MON_DOT_INSET PBL_IF_ROUND_ELSE(34, 30) 
#define DOW_RING PBL_IF_ROUND_ELSE(27, 27) 
#define AP_RING PBL_IF_ROUND_ELSE(52, 50) 
#define MARKER_RING PBL_IF_ROUND_ELSE(42, 43) 
#define MARKER_RING_INSET PBL_IF_ROUND_ELSE(21, 16) 
#define CONNECTOR_RADIUS PBL_IF_ROUND_ELSE(9, 8) 
#define DIAL_STROKE PBL_IF_ROUND_ELSE(2, 3) 
#define DIAL_RADIUS PBL_IF_ROUND_ELSE(24, 22)
#define HOUR_HAND_LENGTH PBL_IF_ROUND_ELSE(42, 36)
#define MIN_HAND_LENGTH PBL_IF_ROUND_ELSE(54, 50)

static Time s_last_time;

static int32_t get_angle_for_hour(int hour) {
  // Progress through 12 hours, out of 360 degrees
  return (hour * 360) / 12;
}

static int32_t get_angle_for_minute(int min) {
  // Progress through 60 minutes, out of 360 degrees
  return (min * 360) / 60;
}

static int32_t get_angle_for_day(int day) {
  return (day * 360) / 30;
}


static void update_display() {
  layer_mark_dirty(clock_layer);
  layer_mark_dirty(date_dots);
}

static void load_settings() {
  load_default_theme();
  persist_read_data(THEME, &theme, sizeof(theme));
}

static void save_settings() {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Saving settings");
  persist_write_data(THEME, &theme, sizeof(theme));
  update_display();
}



static void draw_clock(Layer *layer, GContext *ctx) {
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  // Set Layout Boundaries
  GRect bounds = layer_get_bounds(layer);
  GRect mon_ring = grect_inset(bounds, GEdgeInsets(MON_RING));
  GRect dow_ring = grect_inset(bounds, GEdgeInsets(DOW_RING));
  GRect ap_ring = grect_inset(bounds, GEdgeInsets(AP_RING));
  GRect marker_ring = grect_inset(bounds, GEdgeInsets(MARKER_RING));
  GRect marker_inset_ring = grect_inset(marker_ring, GEdgeInsets(MARKER_RING_INSET));  
  GPoint center = grect_center_point(&marker_inset_ring);

  // Draw the AM/PM indicator
  if (tick_time->tm_hour > 11) { // 00:00 - 11:59
    graphics_context_set_fill_color(ctx, theme.PMDotColor);
  } else {
    graphics_context_set_fill_color(ctx, theme.AMDotColor);
  }
  const GPoint ap_dot = gpoint_from_polar(
    ap_ring,
    GOvalScaleModeFillCircle,
    DEG_TO_TRIGANGLE(30)
  );
  graphics_fill_circle(ctx, ap_dot, 3);
  
  if (persist_exists(SHOW_TEXTURE) && persist_read_bool(SHOW_TEXTURE)) {
    graphics_context_set_stroke_color(ctx, theme.MarkerColor);  
  } else {
    graphics_context_set_stroke_color(ctx, theme.MarkerColor);  
  }
  // Draw the Markers
  for(int i = 0; i < 7; i++) {
    
    int angle = 60;
  
    const GPoint marker1 = gpoint_from_polar(
      marker_inset_ring,
      GOvalScaleModeFillCircle,
      DEG_TO_TRIGANGLE(angle*i)
    );

    const GPoint marker1a = gpoint_from_polar(
      marker_ring,
      GOvalScaleModeFillCircle,
      DEG_TO_TRIGANGLE(angle*i)
    );

    graphics_context_set_stroke_width(ctx, PBL_IF_ROUND_ELSE(4, 3));
    graphics_draw_line(ctx, marker1a, marker1);



    if (persist_exists(SHOW_DAY_OF_WEEK) && persist_read_bool(SHOW_DAY_OF_WEEK)) {
      // Day of the Week
      // i = 0 Sunday
      int dow_angle = 120;
      const GPoint d1 = gpoint_from_polar(
          dow_ring,
          GOvalScaleModeFillCircle,
          DEG_TO_TRIGANGLE(dow_angle+(20*i))
      );
      if (tick_time->tm_wday == i) {
        graphics_context_set_fill_color(ctx, theme.CurrentWeekDayColor);
      } else {
        graphics_context_set_fill_color(ctx, theme.DateDotColor);
      }
      graphics_fill_circle(ctx, d1, 3);
    }
  }
  
  // Draw the Center Dial
  graphics_context_set_stroke_width(ctx, DIAL_STROKE);
  graphics_context_set_stroke_color(ctx, theme.RingColor);
  graphics_draw_circle(ctx, center, DIAL_RADIUS);
  
  // Draw Center Circle
  graphics_context_set_fill_color(ctx, theme.CenterKnobColor);
  graphics_fill_circle(ctx, center, CONNECTOR_RADIUS);


  
  s_last_time.hours = tick_time->tm_hour;
  s_last_time.hours -= (s_last_time.hours > 12) ? 12 : 0;
  s_last_time.minutes = tick_time->tm_min;
  
  
  // Fill Current Month
  int month_angle = DEG_TO_TRIGANGLE(get_angle_for_hour(tick_time->tm_mon + 1));
  const GPoint h3 = gpoint_from_polar(
      mon_ring,
      GOvalScaleModeFillCircle,
      month_angle
  );
  graphics_context_set_fill_color(ctx, theme.MonthDotColor);
  graphics_fill_circle(ctx, h3, 3);

  
  // Draw Minute Hand
  int minute_angle = TRIG_MAX_ANGLE * s_last_time.minutes / 60;
  
  // Draw Hour Hand
  int hour_angle = TRIG_MAX_ANGLE * (tick_time->tm_hour * 5 + tick_time->tm_min / 12) / 60;
//   hour_angle += (minute_angle / TRIG_MAX_ANGLE) * (TRIG_MAX_ANGLE / 12);


  GPoint hour_hand = {
    .x = (int16_t)(sin_lookup(hour_angle) * (int32_t) HOUR_HAND_LENGTH / TRIG_MAX_RATIO) + center.x,
    .y = (int16_t)(-cos_lookup(hour_angle) * (int32_t) HOUR_HAND_LENGTH / TRIG_MAX_RATIO) + center.y,
  };

  GPoint minute_hand = {
    .x = (int16_t)(sin_lookup(minute_angle) * (int32_t) MIN_HAND_LENGTH / TRIG_MAX_RATIO) + center.x,
    .y = (int16_t)(-cos_lookup(minute_angle) * (int32_t) MIN_HAND_LENGTH / TRIG_MAX_RATIO) + center.y,
  };
  
  graphics_context_set_stroke_width(ctx, 6);
  graphics_context_set_stroke_color(ctx, theme.MinuteHandColor);
  graphics_draw_line(ctx, center, minute_hand);

  graphics_context_set_stroke_color(ctx, theme.HourHandColor);
  graphics_draw_line(ctx, center, hour_hand);

  
  // Draw Center Circle Dot
  graphics_context_set_fill_color(ctx, theme.CenterDotColor);
  graphics_fill_circle(ctx, center, 4);


}


static void draw_date_dots(Layer *layer, GContext *ctx) {



  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  GRect bounds = layer_get_bounds(layer);
  GRect month_ring = grect_inset(bounds, GEdgeInsets(MONTH_RING));
  GRect ring31 = grect_inset(bounds, GEdgeInsets(RING_31));  

  // Background Color
  graphics_context_set_fill_color(ctx, theme.BackgroundColor);
  graphics_fill_rect(ctx, bounds, 0, GCornersAll);

  // Draw the Texture
  if (persist_exists(SHOW_TEXTURE) && persist_read_bool(SHOW_TEXTURE)) {
    graphics_context_set_compositing_mode(ctx, GCompOpSet);
    graphics_draw_bitmap_in_rect(ctx, texture, bounds);
  }

  
  for(int i = 0; i < 32; i++) {
    int day_angle = DEG_TO_TRIGANGLE(get_angle_for_day(i));
    
    if (i == 31) {
      int day_angle = DEG_TO_TRIGANGLE(get_angle_for_day(0));
      GRect fill_square = grect_centered_from_polar(ring31, GOvalScaleModeFitCircle, day_angle, GSize(10,10));


      GPoint fill_in = grect_center_point(&fill_square);
      if (tick_time->tm_mday == 31) {
        graphics_context_set_fill_color(ctx, theme.CurrentDateDotColor);
      } else {
        graphics_context_set_fill_color(ctx, theme.DateDotColor);
      }
      graphics_fill_circle(ctx, fill_in, MONTH_DOT_RADIUS);

    } else {
      GRect fill_square = grect_centered_from_polar(month_ring, GOvalScaleModeFitCircle, day_angle, GSize(10,10));

      GPoint fill_in = grect_center_point(&fill_square);
      if (tick_time->tm_mday == i) {
        graphics_context_set_fill_color(ctx, theme.CurrentDateDotColor);
      } else {
        graphics_context_set_fill_color(ctx, theme.DateDotColor);
      }
      graphics_fill_circle(ctx, fill_in, MONTH_DOT_RADIUS);
    }
    
  }
  

}


static void inbox_received_handler(DictionaryIterator *iter, void *context) {


  // Texture
  Tuple *st_t = dict_find(iter, MESSAGE_KEY_Show_Texture);
  if (st_t) {
    persist_write_int(SHOW_TEXTURE, st_t->value->int32 == 1);
  }

  // Day of Week
  Tuple *sd_t = dict_find(iter, MESSAGE_KEY_Show_DoW);
  if (sd_t) {
    persist_write_int(SHOW_DAY_OF_WEEK, sd_t->value->int32 == 1);
  }

  // Background Color
  Tuple *bg_color_t = dict_find(iter, MESSAGE_KEY_BackgroundColor);
  if (bg_color_t) {
    theme.BackgroundColor = GColorFromHEX(bg_color_t->value->int32);
  }

  // Date Dot
  Tuple *dd_color_t = dict_find(iter, MESSAGE_KEY_DateDotColor);
  if (dd_color_t) {
    theme.DateDotColor = GColorFromHEX(dd_color_t->value->int32);
  }

  // Current Date Dot
  Tuple *cdd_color_t = dict_find(iter, MESSAGE_KEY_CurrentDateDotColor);
  if (cdd_color_t) {
    theme.CurrentDateDotColor = GColorFromHEX(cdd_color_t->value->int32);
  }

  // AM Dot
  Tuple *add_color_t = dict_find(iter, MESSAGE_KEY_AMDotColor);
  if (add_color_t) {
    theme.AMDotColor = GColorFromHEX(add_color_t->value->int32);
  }

  // PM Dot
  Tuple *pdd_color_t = dict_find(iter, MESSAGE_KEY_PMDotColor);
  if (pdd_color_t) {
    theme.PMDotColor = GColorFromHEX(pdd_color_t->value->int32);
  }

  // Marker Color
  Tuple *m_color_t = dict_find(iter, MESSAGE_KEY_MarkerColor);
  if (m_color_t) {
    theme.MarkerColor = GColorFromHEX(m_color_t->value->int32);
  }

  // Ring Color
  Tuple *r_color_t = dict_find(iter, MESSAGE_KEY_RingColor);
  if (r_color_t) {
    theme.RingColor = GColorFromHEX(r_color_t->value->int32);
  }

  // Current Week Day Color
  Tuple *cwd_color_t = dict_find(iter, MESSAGE_KEY_CurrentWeekDayColor);
  if (cwd_color_t) {
    theme.CurrentWeekDayColor = GColorFromHEX(cwd_color_t->value->int32);
  }

  // Week Day Color
  Tuple *wd_color_t = dict_find(iter, MESSAGE_KEY_WeekDayColor);
  if (wd_color_t) {
    theme.WeekDayColor = GColorFromHEX(wd_color_t->value->int32);
  }

  // Hour Hand Color
  Tuple *hh_color_t = dict_find(iter, MESSAGE_KEY_HourHandColor);
  if (hh_color_t) {
    theme.HourHandColor = GColorFromHEX(hh_color_t->value->int32);
  }


  // Minute Hand Color
  Tuple *mh_color_t = dict_find(iter, MESSAGE_KEY_MinuteHandColor);
  if (wd_color_t) {
    theme.MinuteHandColor = GColorFromHEX(mh_color_t->value->int32);
  }

  // Month DOt Color
  Tuple *md_color_t = dict_find(iter, MESSAGE_KEY_MonthDotColor);
  if (md_color_t) {
    theme.MonthDotColor = GColorFromHEX(md_color_t->value->int32);
  }

  // Center Dot
  Tuple *cd_color_t = dict_find(iter, MESSAGE_KEY_CenterDotColor);
  if (cd_color_t) {
    theme.CenterDotColor = GColorFromHEX(cd_color_t->value->int32);
  }

  // Center Knob Color
  Tuple *ck_color_t = dict_find(iter, MESSAGE_KEY_CenterKnobColor);
  if (ck_color_t) {
    theme.CenterKnobColor = GColorFromHEX(ck_color_t->value->int32);
  }



  save_settings();

}

static void main_window_load() {
  window_set_background_color(my_window, theme.BackgroundColor);
  app_message_register_inbox_received(inbox_received_handler);
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());

  Layer *window_layer = window_get_root_layer(my_window);
  GRect bounds = layer_get_bounds(window_layer);
  date_dots = layer_create(bounds);
  clock_layer = layer_create(bounds);
  texture = gbitmap_create_with_resource(RESOURCE_ID_TEXTURE);
  
  layer_set_update_proc(date_dots, draw_date_dots);
  layer_set_update_proc(clock_layer, draw_clock);

  layer_add_child(window_get_root_layer(my_window), date_dots);
  layer_add_child(window_get_root_layer(my_window), clock_layer);
  layer_mark_dirty(date_dots);
  layer_mark_dirty(clock_layer);
}

static void main_window_unload() {
  layer_destroy(date_dots);
  layer_destroy(clock_layer);
  gbitmap_destroy(texture);
}

static void tick_handler(struct tm *tick_time, TimeUnits changed) {
  layer_mark_dirty(date_dots);
  layer_mark_dirty(clock_layer);
}

static void app_connection_handler(bool connected) {
  if (! connected) {
      static const uint32_t const segments[] = { 1000, 100, 900, 100, 250 };
      VibePattern pat = {
        .durations = segments,
        .num_segments = ARRAY_LENGTH(segments),
      };
      vibes_enqueue_custom_pattern(pat);
  }
}

void handle_init(void) {
  load_settings();
  my_window = window_create();
  window_set_window_handlers(my_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  connection_service_subscribe((ConnectionHandlers) {
    .pebble_app_connection_handler = app_connection_handler,
  });
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  window_stack_push(my_window, true);
}

void handle_deinit(void) {
  window_destroy(my_window);
  app_message_deregister_callbacks();
  connection_service_unsubscribe();
  tick_timer_service_unsubscribe();
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}
