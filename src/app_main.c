#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_event.h"
#include "esp_system.h"
#include "esp_log.h"
#include "tcpip_adapter.h"
#include "nvs_flash.h"
#include "sdkconfig.h"
#include "esp_flash_partitions.h"
#include "esp_partition.h"
#include "esp_ota_ops.h"
#ifdef CONFIG_MDNS_ENABLED
#include "mdns.h"
#endif
#ifdef CONFIG_SNTP_ENABLED
//#include "app_sntp.h"
#endif

#define LOG_LOCAL_LEVEL ESP_LOG_INFO
#include "esp_log.h"

#define HASH_LEN 32 /* SHA-256 digest length */
static const char *TAG = "application_main";

void app_main()
{

}