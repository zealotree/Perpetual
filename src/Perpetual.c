#include <pebble.h>
Window *my_window;
static Layer *daily_ring;
static Layer *hollow_dots;
static Layer *clock_layer;

typedef struct {
  int hours;
  int minutes;
} Time;    

static Time s_last_time;

static int32_t get_angle_for_hour(int hour) {
  // Progress through 12 hours, out of 360 degrees
  return (hour * 360) / 12;
}

static int32_t get_angle_for_minute(int min) {
  // Progress through 60 minutes, out of 360 degrees
  return (min * 360) / 60;
}


static int32_t get_angle_for_month(int month) {
  // Progress through 12 months, out of 360 degrees
  return (month * 360) / 12;
}

static int32_t get_angle_for_day(int day) {
  return (day * 360) / 30;
}


static void draw_current_date(Layer *layer, GContext *ctx) {
  // Custom drawing happens here!
  GRect bounds = layer_get_bounds(layer);
  GRect ring31 = grect_inset(bounds, GEdgeInsets(31));
  
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  
//   int hour_angle = DEG_TO_TRIGANGLE(get_angle_for_hour(tick_time->tm_hour));
  
//   APP_LOG(APP_LOG_LEVEL_DEBUG, "%d", hour_angle);
  graphics_context_set_fill_color(ctx, GColorChromeYellow);
//   graphics_fill_radial(ctx, bounds, GOvalScaleModeFitCircle, 8, DEG_TO_TRIGANGLE(0), hour_angle);
  GRect month_ring = grect_inset(bounds, GEdgeInsets(17));
  
  GRect square;
  
  if (tick_time->tm_mday == 31){
    int day_angle = DEG_TO_TRIGANGLE(get_angle_for_day(0));
    GRect square = grect_centered_from_polar(ring31, GOvalScaleModeFitCircle, day_angle, GSize(12,12));
  } else {
    int day_angle = DEG_TO_TRIGANGLE(get_angle_for_day(tick_time->tm_mday));
    GRect square = grect_centered_from_polar(month_ring, GOvalScaleModeFitCircle, day_angle, GSize(12,12));
  }

  graphics_context_set_stroke_color(ctx, GColorLightGray);
  GPoint cur_day = grect_center_point(&square);
  graphics_fill_circle(ctx, cur_day, 6);
}

static void draw_clock(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  GRect hour_ring = grect_inset(bounds, GEdgeInsets(50));  
  GRect min_ring = grect_inset(bounds, GEdgeInsets(32));  
  GRect mon_ring = grect_inset(bounds, GEdgeInsets(70));
  GRect dow_ring = grect_inset(bounds, GEdgeInsets(30));



  GRect marker_ring = grect_inset(bounds, GEdgeInsets(42));
  GRect marker_inset_ring = grect_inset(marker_ring, GEdgeInsets(21));  
  GRect inset_ring = grect_inset(marker_ring, GEdgeInsets(26));

  GPoint center = grect_center_point(&inset_ring);
  
  graphics_context_set_stroke_color(ctx, GColorLightGray);
  
  
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

    graphics_context_set_stroke_width(ctx, 3);
    graphics_draw_line(ctx, marker1a, marker1);
  }
  
  // Draw the Center Dial
  graphics_fill_circle(ctx, center, 20);
  
  // Draw Center Circle
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_circle(ctx, center, 8);


  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  
  s_last_time.hours = tick_time->tm_hour;
  s_last_time.hours -= (s_last_time.hours > 12) ? 12 : 0;
  s_last_time.minutes = tick_time->tm_min;
  
  // Draw Month Outline
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_radial(ctx, mon_ring, GOvalScaleModeFitCircle, 2, DEG_TO_TRIGANGLE(0), DEG_TO_TRIGANGLE(360));
  
  // Fill Current Month
  GRect mon_ring_r = grect_inset(mon_ring, GEdgeInsets(5));
  int month_angle = DEG_TO_TRIGANGLE(get_angle_for_month(tick_time->tm_mon + 1));
  const GPoint h3 = gpoint_from_polar(
      mon_ring_r,
      GOvalScaleModeFillCircle,
      month_angle
  );
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_context_set_fill_color(ctx, GColorChromeYellow);
  graphics_fill_circle(ctx, h3, 3);

  
  // Draw Minute Hand
  int minute_angle = DEG_TO_TRIGANGLE(get_angle_for_minute(s_last_time.minutes));
  const GPoint h2 = gpoint_from_polar(
      min_ring,
      GOvalScaleModeFillCircle,
      minute_angle
  );
  graphics_context_set_stroke_color(ctx, GColorWhite);
  graphics_context_set_stroke_width(ctx, 6);
  graphics_draw_line(ctx, center, h2);
  
  // Draw Hour Hand
  int hour_angle = DEG_TO_TRIGANGLE(get_angle_for_hour(s_last_time.hours));
  const GPoint h1 = gpoint_from_polar(
      hour_ring,
      GOvalScaleModeFillCircle,
      hour_angle
  );
  graphics_context_set_stroke_color(ctx, GColorChromeYellow);
  graphics_context_set_stroke_width(ctx, 6);
  graphics_draw_line(ctx, center, h1);
  

  
  // Draw Center Circle Dot
  graphics_context_set_fill_color(ctx, GColorLightGray);
  graphics_fill_circle(ctx, center, 3);

  int dow_angle = 120;
    
  for(int i = 0; i < 7; i++) {
    // i = 0 Sunday
    // Draw Day of Week Dots
    const GPoint d1 = gpoint_from_polar(
        dow_ring,
        GOvalScaleModeFillCircle,
        DEG_TO_TRIGANGLE(dow_angle+(20*i))
    );
    if (tick_time->tm_wday == i) {
      graphics_context_set_fill_color(ctx, GColorChromeYellow);
    } else {
      graphics_context_set_fill_color(ctx, GColorBlack);
    }
    graphics_fill_circle(ctx, d1, 3);
  }

}


static void draw_hollow_dots(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  GRect month_ring = grect_inset(bounds, GEdgeInsets(17));
  GRect ring31 = grect_inset(bounds, GEdgeInsets(31));  
  
  for(int i = 0; i < 31; i++) {
    int day_angle = DEG_TO_TRIGANGLE(get_angle_for_day(i));
     GRect square;
   GRect fill_square;
    
    if (i == 30) {
      int day_angle = DEG_TO_TRIGANGLE(get_angle_for_day(0));
      GRect square = grect_centered_from_polar(ring31, GOvalScaleModeFitCircle, day_angle, GSize(12,12));
      GRect fill_square = grect_centered_from_polar(ring31, GOvalScaleModeFitCircle, day_angle, GSize(10,10));
    } else {
      GRect square = grect_centered_from_polar(month_ring, GOvalScaleModeFitCircle, day_angle, GSize(12,12));
      GRect fill_square = grect_centered_from_polar(month_ring, GOvalScaleModeFitCircle, day_angle, GSize(10,10));
    }
    GPoint fill_in = grect_center_point(&fill_square);
    graphics_context_set_fill_color(ctx, GColorLightGray);
    graphics_fill_circle(ctx, fill_in, 5);

    
    graphics_context_set_fill_color(ctx, GColorLightGray);
    graphics_fill_radial(ctx, square, GOvalScaleModeFitCircle, 2, DEG_TO_TRIGANGLE(0), DEG_TO_TRIGANGLE(360));
  }
  

}

static void main_window_load() {
  window_set_background_color(my_window, GColorDarkGray);
  Layer *window_layer = window_get_root_layer(my_window);
  GRect bounds = layer_get_bounds(window_layer);
  daily_ring = layer_create(bounds);
  hollow_dots = layer_create(bounds);
  clock_layer = layer_create(bounds);
  
  layer_set_update_proc(daily_ring, draw_current_date);
  layer_set_update_proc(hollow_dots, draw_hollow_dots);
  layer_set_update_proc(clock_layer, draw_clock);


  layer_add_child(window_get_root_layer(my_window), hollow_dots);
  layer_add_child(window_get_root_layer(my_window), daily_ring);
  layer_add_child(window_get_root_layer(my_window), clock_layer);
  layer_mark_dirty(daily_ring);
  layer_mark_dirty(hollow_dots);
  layer_mark_dirty(clock_layer);
}

static void main_window_unload() {
  layer_destroy(daily_ring);
  layer_destroy(hollow_dots);
  layer_destroy(clock_layer);
}

static void tick_handler(struct tm *tick_time, TimeUnits changed) {
  layer_mark_dirty(daily_ring);
  layer_mark_dirty(hollow_dots);
  layer_mark_dirty(clock_layer);
}

static void app_connection_handler(bool connected) {
  if (! connected) {
      static const uint32_t const segments[] = { 1000, 500, 400, 300, 200 };
      VibePattern pat = {
        .durations = segments,
        .num_segments = ARRAY_LENGTH(segments),
      };
      vibes_enqueue_custom_pattern(pat);
  }
}

void handle_init(void) {
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
  layer_mark_dirty(daily_ring);
}

void handle_deinit(void) {
  window_destroy(my_window);
  connection_service_unsubscribe();
  tick_timer_service_unsubscribe();
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}