#include <lvgl.h>
#include <TFT_eSPI.h>
#include <lv_demos.h>
#include <Wire.h>
#include <FT6336.h>

/*Change to your screen resolution*/
static const uint16_t screenWidth  = 480;
static const uint16_t screenHeight = 320;

#define LCD_BL_PIN (13)

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[ screenWidth * 10 ];

TFT_eSPI tft = TFT_eSPI(); /* TFT instance */

/* Display flushing */
void my_disp_flush( lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p )
{
    uint32_t w = ( area->x2 - area->x1 + 1 );
    uint32_t h = ( area->y2 - area->y1 + 1 );

    tft.startWrite();
    tft.setAddrWindow( area->x1, area->y1, w, h );
    tft.pushColors( ( uint16_t * )&color_p->full, w * h, true );
    tft.endWrite();

    lv_disp_flush_ready( disp );
}

/*Read the touchpad*/
void my_touchpad_read( lv_indev_drv_t *indev_driver, lv_indev_data_t * data) {
  uint8_t touchPoint = touch.read((uint16_t*)(&data->point.x), (uint16_t*)(&data->point.y));
  if (touchPoint > 0) {
    Serial.printf("X: %d\tY: %d\n", data->point.x, data->point.y);
  }
  data->state = touchPoint > 0 ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
}

void setup() {
    Serial.begin( 115200 ); /* prepare for possible serial debug */

    lv_init();

    tft.begin();          /* TFT init */
    tft.setRotation( 1 ); /* Landscape orientation, flipped */
    // tft.invertDisplay(false);

    // Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN, 400E3);
    Wire.begin();
    touch.init();

    lv_disp_draw_buf_init( &draw_buf, buf, NULL, screenWidth * 10 );

    /*Initialize the display*/
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init( &disp_drv );
    /*Change the following line to your display resolution*/
    disp_drv.hor_res = screenWidth;
    disp_drv.ver_res = screenHeight;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register( &disp_drv );

    /*Initialize the (dummy) input device driver*/
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = my_touchpad_read;
    lv_indev_drv_register(&indev_drv);
    
    // lv_demo_music();
    lv_demo_widgets();

    pinMode(LCD_BL_PIN, OUTPUT);
    digitalWrite(LCD_BL_PIN, LOW);
}

void loop() {
    lv_timer_handler(); /* let the GUI do its work */
    delay(5);
}
