/* i2c - Example
   Tis is  a test of the SSD1306 OLED display driver running in a task 
   For other examples please check:
   https://github.com/espressif/esp-idf/tree/master/examples

   See README.md file to get detailed usage of this example.
updated by Ole Schultz 18 okt. 2020
*/
#include <stdio.h>
#include "esp_log.h"
#include "driver/i2c.h"
#include "sdkconfig.h"
#include "ssd1306.h"

#define SDA_PIN GPIO_NUM_21
#define SCL_PIN GPIO_NUM_22

static const char *TAG = "i2c-example";

SemaphoreHandle_t print_mux = NULL;
#define SECOND (1000 / portTICK_PERIOD_MS)

char tasklistbuf[20]={0};    

void i2c_example_master_init()
{
   
i2c_config_t conf;

conf.mode = I2C_MODE_MASTER;

conf.sda_io_num = SDA_PIN;  //14

conf.scl_io_num = SCL_PIN;   //2

conf.sda_pullup_en = GPIO_PULLUP_ENABLE;

conf.scl_pullup_en = GPIO_PULLUP_ENABLE;

conf.master.clk_speed = 100000;
//conf.clk_stretch_tick = 500;




// install the driver

ESP_ERROR_CHECK(i2c_driver_install(I2C_NUM_0, conf.mode,0,0,0));

printf("- i2c driver installed\r\n\r\n");

ESP_ERROR_CHECK(i2c_param_config(I2C_NUM_0, &conf));

printf("- i2c controller configured\r\n");

printf("scanning the bus...\r\n\r\n");

int devices_found = 0;



for(int address = 1; address < 127; address++) {



	// create and execute the command link

	i2c_cmd_handle_t cmd = i2c_cmd_link_create();

	i2c_master_start(cmd);

	i2c_master_write_byte(cmd, (address << 1) | I2C_MASTER_WRITE, true);

	i2c_master_stop(cmd);

	if(i2c_master_cmd_begin(I2C_NUM_0, cmd, 100 / portTICK_RATE_MS) == ESP_OK) {
        // dev.i2c_addr=address;
		printf("-> found device with address 0x%02x\r\n", address);

		devices_found++;

	}

	i2c_cmd_link_delete(cmd);

}

if(devices_found == 0) printf("\r\n-> no devices found\r\n");

printf("\r\n...scan completed!\r\n");

    //return ESP_OK;
}


static void ssd1306_task(void *pvParameters)
{

  

 vTaskDelay(1000/portTICK_PERIOD_MS);
 InitializeDisplay();
reset_display();
setXY(0,0);
sendStr("test");
//printBigTime("102");
 sendStrXY("hello",6,0);
sendCharXY('a',4,4);

    bool fwd = false;

    printf("%s: Started user interface task\n", __FUNCTION__);
    vTaskDelay(SECOND);

vTaskDelete(NULL);
 
}

void app_main(void)
{
    print_mux = xSemaphoreCreateMutex();
    i2c_example_master_init();
    

    printf("done\n");
InitializeDisplay();

    printf("done init display\n");
    
 
    xTaskCreate( ssd1306_task, " ssd1306_task", 2048, (void *)0, 10, NULL);
   
}
