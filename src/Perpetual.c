#include <pebble.h>
Window *my_window;
static Layer *date_dots;
static Layer *clock_layer;

typedef struct {
  int hours;
  int minutes;
} Time;    

// Layout boundaries
#define HOUR_RING PBL_IF_ROUND_ELSE(50, 50) // HOUR HAND
#define MIN_RING PBL_IF_ROUND_ELSE(32, 32) // MIN HAND
#define MONTH_RING PBL_IF_ROUND_ELSE(14, 5) 
#define MONTH_DOT_RADIUS PBL_IF_ROUND_ELSE(4, 4)
#define RING_31 PBL_IF_ROUND_ELSE(27, 16) // 31st DOT
#define MON_RING PBL_IF_ROUND_ELSE(68, 68) 
#define MON_DOT_INSET PBL_IF_ROUND_ELSE(34, 30) 
#define DOW_RING PBL_IF_ROUND_ELSE(27, 27) 
#define AP_RING PBL_IF_ROUND_ELSE(52, 50) 
#define MARKER_RING PBL_IF_ROUND_ELSE(37, 37) 
#define MARKER_RING_INSET PBL_IF_ROUND_ELSE(21, 16) 
#define CONNECTOR_RADIUS PBL_IF_ROUND_ELSE(9, 8) 
#define DIAL_STROKE PBL_IF_ROUND_ELSE(2, 3) 
#define DIAL_RADIUS PBL_IF_ROUND_ELSE(24, 22) 

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


static void draw_clock(Layer *layer, GContext *ctx) {
  //time_t temp = time(NULL);
  time_t temp = 1461738634;
  struct tm *tick_time = localtime(&temp);

  // Set Layout Boundaries
  GRect bounds = layer_get_bounds(layer);
  GRect hour_ring = grect_inset(bounds, GEdgeInsets(HOUR_RING));
  GRect min_ring = grect_inset(bounds, GEdgeInsets(MIN_RING));  
  GRect mon_ring = grect_inset(bounds, GEdgeInsets(MON_RING));
  GRect dow_ring = grect_inset(bounds, GEdgeInsets(DOW_RING));
  GRect ap_ring = grect_inset(bounds, GEdgeInsets(AP_RING));
  GRect marker_ring = grect_inset(bounds, GEdgeInsets(MARKER_RING));
  GRect marker_inset_ring = grect_inset(marker_ring, GEdgeInsets(MARKER_RING_INSET));  
  GRect inset_ring = grect_inset(marker_ring, GEdgeInsets(26));
  GRect mon_ring_r = grect_inset(marker_ring, GEdgeInsets(MON_DOT_INSET));
  GPoint center = grect_center_point(&marker_inset_ring);

  // Draw the AM/PM indicator
  if (tick_time->tm_hour > 12) {
    graphics_context_set_fill_color(ctx, GColorBlack);
  } else {
    graphics_context_set_fill_color(ctx, GColorWhite);
  }
  const GPoint ap_dot = gpoint_from_polar(
    ap_ring,
    GOvalScaleModeFillCircle,
    DEG_TO_TRIGANGLE(30)
  );
  graphics_fill_circle(ctx, ap_dot, 3);
  
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

    graphics_context_set_stroke_width(ctx, 4);
    graphics_draw_line(ctx, marker1a, marker1);


    // Day of the Week
    // i = 0 Sunday
    int dow_angle = 120;
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
  
  // Draw the Center Dial
  graphics_context_set_stroke_width(ctx, DIAL_STROKE);
  graphics_context_set_fill_color(ctx, GColorBrass);
  graphics_draw_circle(ctx, center, DIAL_RADIUS);
  
  // Draw Center Circle
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_circle(ctx, center, CONNECTOR_RADIUS);


  
  s_last_time.hours = tick_time->tm_hour;
  s_last_time.hours -= (s_last_time.hours > 12) ? 12 : 0;
  s_last_time.minutes = tick_time->tm_min;
  
  
  // Fill Current Month
  int month_angle = DEG_TO_TRIGANGLE(get_angle_for_hour(tick_time->tm_mon + 1));
  const GPoint h3 = gpoint_from_polar(
      mon_ring_r,
      GOvalScaleModeFillCircle,
      month_angle
  );
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
  graphics_fill_circle(ctx, center, 4);


}


static void draw_date_dots(Layer *layer, GContext *ctx) {
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  GRect bounds = layer_get_bounds(layer);
  GRect month_ring = grect_inset(bounds, GEdgeInsets(MONTH_RING));
  GRect ring31 = grect_inset(bounds, GEdgeInsets(RING_31));  
  
  for(int i = 0; i < 32; i++) {
    int day_angle = DEG_TO_TRIGANGLE(get_angle_for_day(i));
     GRect square;
   GRect fill_square;
    
    if (i == 31) {
      int day_angle = DEG_TO_TRIGANGLE(get_angle_for_day(0));
      GRect square = grect_centered_from_polar(ring31, GOvalScaleModeFitCircle, day_angle, GSize(12,12));
      GRect fill_square = grect_centered_from_polar(ring31, GOvalScaleModeFitCircle, day_angle, GSize(10,10));


      GPoint fill_in = grect_center_point(&fill_square);
      if (tick_time->tm_mday == 31) {
        graphics_context_set_fill_color(ctx, GColorChromeYellow);
      } else {
        graphics_context_set_fill_color(ctx, GColorBlack);
      }
      graphics_fill_circle(ctx, fill_in, MONTH_DOT_RADIUS);

    } else {
      GRect square = grect_centered_from_polar(month_ring, GOvalScaleModeFitCircle, day_angle, GSize(12,12));
      GRect fill_square = grect_centered_from_polar(month_ring, GOvalScaleModeFitCircle, day_angle, GSize(10,10));

      GPoint fill_in = grect_center_point(&fill_square);
      if (tick_time->tm_mday == i) {
        graphics_context_set_fill_color(ctx, GColorChromeYellow);
      } else {
        graphics_context_set_fill_color(ctx, GColorBlack);
      }
      graphics_fill_circle(ctx, fill_in, MONTH_DOT_RADIUS);
    }
    
  }
  

}

static void main_window_load() {
  window_set_background_color(my_window, GColorDarkGray);
  Layer *window_layer = window_get_root_layer(my_window);
  GRect bounds = layer_get_bounds(window_layer);
  date_dots = layer_create(bounds);
  clock_layer = layer_create(bounds);
  
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
}

static void tick_handler(struct tm *tick_time, TimeUnits changed) {
  layer_mark_dirty(date_dots);
  layer_mark_dirty(clock_layer);
}

static void app_connection_handler(bool connected) {
  if (! connected) {
      static const uint32_t const segments[] = { 1000, 900, 250 };
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
