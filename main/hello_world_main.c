/* This script creates a representative dataset of audio from the ESP32-S3
   
    TO RUN: >> idf.py set-target esp32s3
           >> idf.py -p COM3 -b 480600 flash monitor

    MAKE SURE SPI FLASH SIZE IS 8MB:
            >> idf.py menuconfig
            >> Serial Flasher Config >> Flash Size (8 MB)
            
*/
#include <stdio.h>
#include <driver/adc.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "esp_adc_cal.h"

void app_main(void)
{   
    printf("Hello world!\n");

  
    /* ADC Configuration and Callibration Documentation: |
    ------------------------------------------------------
        VRef of the ESP32-S3 is 1100 mV                  |
                                                         
        Right Channel: GPIO 4 ADC1 Channel 3             |
        Left Channel: GPIO 11 ADC2 Channel 0             |             

        ADC Attenuation Options:                         |
        ADC_ATTEN_DB_0   : 0 - 950 mV                    |
        ADC_ATTEN_DB_2_5 : 0 - 1250 mV                   |
        ADC_ATTEN_DB_6   : 0 - 1750 mV                   |
        ADC_ATTEN_DB_11  : 0 - 3100 mV                   |

        ADC Accuracy Options:                            |
        ADC_WIDTH_9Bit                                   |
        ADC_WIDTH_10Bit                                  |
        ADC_WIDTH_11Bit                                  |
        ADC_WIDTH_12Bit                                  |
        ADC_WIDTH_BIT_DEFAULT (Max Bit Width)            |
    -----------------------------------------------------*/

    // Configure desired precision and attenuation for ADC pins
    adc1_config_width(ADC_WIDTH_BIT_DEFAULT);
    adc1_config_channel_atten(ADC1_CHANNEL_3,ADC_ATTEN_DB_0);
    adc2_config_channel_atten(ADC2_CHANNEL_0,ADC_ATTEN_DB_0);

    // Create ADC channel characteristics structs for use in calibration functions
    
    esp_adc_cal_characteristics_t adc1_chars;
    esp_adc_cal_characteristics_t adc2_chars;
    esp_adc_cal_characterize(ADC_UNIT_1,ADC_ATTEN_DB_0,ADC_WIDTH_BIT_DEFAULT,1100,&adc1_chars);
    esp_adc_cal_characterize(ADC_UNIT_2,ADC_ATTEN_DB_0,ADC_WIDTH_BIT_DEFAULT,1100,&adc2_chars);

    
    int val2 = 0;
    int* pval2 = &val2;
    while(true){
        int val1 = adc1_get_raw(ADC1_CHANNEL_3);
        adc2_get_raw(ADC2_CHANNEL_0,ADC_WIDTH_BIT_DEFAULT,pval2);
        
        // int val1_prev = val1;
        // int val2_prev = val2;
        
        // if(val1 != val1_prev || val2 != val2_prev){
        //     printf("%d, %d\n",val1,val2);
        // }

        int adc1_voltage = esp_adc_cal_raw_to_voltage(val1,&adc1_chars);
        int adc2_voltage = esp_adc_cal_raw_to_voltage(val2,&adc2_chars);
        printf("%d, %d\n",adc1_voltage,adc2_voltage);
        vTaskDelay(10);
    }


    /* Print chip information */
  /*  esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    printf("This is %s chip with %d CPU core(s), WiFi%s%s, ",
            CONFIG_IDF_TARGET,
            chip_info.cores,
            (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
            (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

    printf("silicon revision %d, ", chip_info.revision);

    printf("%dMB %s flash\n", spi_flash_get_chip_size() / (1024 * 1024),
            (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    printf("Minimum free heap size: %d bytes\n", esp_get_minimum_free_heap_size());

    for (int i = 10; i >= 0; i--) {
        printf("Restarting in %d seconds...\n", i);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    printf("Restarting now.\n");
    fflush(stdout);
    esp_restart();
    */
}
