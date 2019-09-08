#include "station.h"
#include "Arduino.h"
#include "WiFi.h"
#include <HTTPClient.h>

extern "C" void wifi_init_sta();

static void activate_wifi()
{
  //Initialize NVS
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
  wifi_init_sta();
}

static void http_get_task(void *pvParameters)
{
    while(1) {
      // xEventGroupWaitBits(s_wifi_event_group, WIFI_CONNECTED_BIT,
      //                     false, true, portMAX_DELAY);
      //
      // ESP_LOGI(TAG, "Connected to AP");
      if(WiFi.status() == WL_CONNECTED){   //Check WiFi connection status

          HTTPClient http;

          http.begin("http://jsonplaceholder.typicode.com/posts");  //Specify destination for HTTP request
          http.addHeader("Content-Type", "text/plain");             //Specify content-type header

          int httpResponseCode = http.POST("POSTING from ESP32");   //Send the actual POST request

          if(httpResponseCode>0){

           String response = http.getString();                       //Get the response to the request

           Serial.println(httpResponseCode);   //Print return code
           Serial.println(response);           //Print request answer

          }else{

           Serial.print("Error on sending POST: ");
           Serial.println(httpResponseCode);

          }

          http.end();  //Free resources

        }else{
           Serial.println("Error in WiFi connection");
        }
    }
}

void serialSetup() {
    Serial.begin(115200);
    Serial.println("Serial.begin(115200) DONE");
    delay(10);
    Serial.println("serialSetup done");
}

extern "C" void app_main()
{
    initArduino();
    serialSetup();
    activate_wifi();
    xTaskCreate(&http_get_task, "http_get_task", 44096, NULL, 5, NULL);
}
