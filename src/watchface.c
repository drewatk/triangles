#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_time_layer;
static GFont s_time_font;
static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap;

static void update_time() {
    // get a tm structure
    time_t temp = time(NULL);
    struct tm *tick_time = localtime(&temp);

    // Write the current hours and minutes into a buffer
    static char s_time_buffer[8];
    strftime(s_time_buffer, sizeof(s_time_buffer), clock_is_24h_style() ? "%H:%M" : "%l:%M", tick_time);

    // set text on the TextLayers
    text_layer_set_text(s_time_layer, s_time_buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
    update_time(); 
}

static void main_window_load(Window *window) {
    // Get information about the Window
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(window_layer);

    // Create GBitmap
    s_background_bitmap = gbitmap_create_with_resource(PBL_IF_RECT_ELSE(RESOURCE_ID_IMAGE_BACKGROUND_RECT, RESOURCE_ID_IMAGE_BACKGROUND_ROUND));

    // Create Bitmap Layer to display the GBitmap
    s_background_layer = bitmap_layer_create(bounds);

    // set the bitmap onto the layer and add to the window
    bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
    layer_add_child(window_layer, bitmap_layer_get_layer(s_background_layer));


    // create the TextLayer with specific bounds
    s_time_layer = text_layer_create(GRect(0, (bounds.size.h/2.0)-(33), bounds.size.w, 70));

    // Improve the layout
    text_layer_set_background_color(s_time_layer, GColorClear);
    text_layer_set_text_color(s_time_layer, GColorWhite);
    //text_layer_set_text(s_time_layer, "0:00");
    text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

    // Creat Time GFont and apply to time layer
    s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTECH_44));
    text_layer_set_font(s_time_layer, s_time_font);

    // Add the time layer to the Window's root layer
    layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
}

static void main_window_unload(Window *window) {
    // destroy TextLayers
    text_layer_destroy(s_time_layer);

    // destroy GBitmap
    gbitmap_destroy(s_background_bitmap);

    // Destroy BitmapLayer
    bitmap_layer_destroy(s_background_layer);

    // Destroy fonts
    fonts_unload_custom_font(s_time_font);
}

static void init() {
    // Create main Window element and assign to static pointer
    s_main_window = window_create();

    // set handlers to manage the elements inside the Window
    window_set_window_handlers(s_main_window, (WindowHandlers) {
        .load = main_window_load,
        .unload = main_window_unload
    });

    // Show the window on the watch, with animated=true
    window_stack_push(s_main_window, true);

    // Register with TickTimerService
    tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void deinit() {
    // Destroy window
    window_destroy(s_main_window);
}

int main(void) {
    init();
    app_event_loop();
    deinit();

}

