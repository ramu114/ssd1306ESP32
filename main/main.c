#include <stdio.h>
#include "esp_log.h"
#include "driver/i2c.h"
#include "sdkconfig.h"
#include "ssd1306.h"
#include <dht.h>

#define SDA_PIN GPIO_NUM_21
#define SCL_PIN GPIO_NUM_22
//#define tag "SSD1306"

static const dht_sensor_type_t sensor_type = DHT_TYPE_AM2301;
static const gpio_num_t dht_gpio = 17;

void i2c_example_master_init()
{
	i2c_config_t conf;
	conf.mode = I2C_MODE_MASTER;
	conf.sda_io_num = SDA_PIN;	//14
	conf.scl_io_num = SCL_PIN;	//2
	conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
	conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
	conf.master.clk_speed = 100000;
	//conf.clk_stretch_tick = 500;

	// install the driver
	ESP_ERROR_CHECK(i2c_driver_install(I2C_NUM_0, conf.mode, 0, 0, 0));
	printf("- i2c driver installed\r\n\r\n");
	ESP_ERROR_CHECK(i2c_param_config(I2C_NUM_0, &conf));
	printf("- i2c controller configured\r\n");
	printf("scanning the bus...\r\n\r\n");
	int devices_found = 0;

	for (int address = 1; address < 127; address++)
	{
		// create and execute the command link
		i2c_cmd_handle_t cmd = i2c_cmd_link_create();
		i2c_master_start(cmd);
		i2c_master_write_byte(cmd, (address << 1) | I2C_MASTER_WRITE, true);
		i2c_master_stop(cmd);

		if (i2c_master_cmd_begin(I2C_NUM_0, cmd, 100 / portTICK_RATE_MS) == ESP_OK)
		{
			// dev.i2c_addr=address;
			printf("-> found device with address 0x%02x\r\n", address);
			devices_found++;
		}

		i2c_cmd_link_delete(cmd);

	}

	if (devices_found == 0) printf("\r\n-> no devices found\r\n");
	printf("\r\n...scan completed!\r\n");
	//return ESP_OK;
}

void dht(void *pvParameters)
{
    int16_t temperature = 0;
    int16_t humidity = 0;
	InitializeDisplay();
	reset_display();
	vTaskDelay(1000 / portTICK_PERIOD_MS);

	char buffer [50];

    while (1)
    {
        if (dht_read_data(sensor_type, dht_gpio, &humidity, &temperature) == ESP_OK){
            sprintf(buffer,"Humidity: %d%% \n Temp: %dC", humidity / 10, temperature / 10);
			sendStrXY(buffer,0,0);
			printf("Humidity: %d%% Temp: %dC\n", humidity / 10, temperature / 10);
        }else{W
            printf("Could not read data from sensor\n");
		}
        // If you read the sensor data too often, it will heat up
        // http://www.kandrsmith.org/RJS/Misc/Hygrometers/dht_sht_how_fast.html
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}


void app_main(void)
{
	i2c_example_master_init();

	printf("done\n");
	InitializeDisplay();

	printf("done init display\n");

	xTaskCreate(dht, "dht_test", configMINIMAL_STACK_SIZE * 3, NULL, 5, NULL);
}