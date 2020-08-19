#include "esp_log.h"
#include "lwip/ip4_addr.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "sdkconfig.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "app_settings.h"

static const char *TAG     = "settings";
static const char *NVS_KEY = "settings";

static void log_settings()
{
  ESP_LOGI(TAG, " hostname=%s", settings.hostname);
  ESP_LOGI(TAG, " wifi_ssid=%s", settings.wifi_ssid);
  ESP_LOGI(TAG, " wifi_password=%s", settings.wifi_password);
  ESP_LOGI(TAG, " mdns_instance=%s", settings.mdns_instance);
  ESP_LOGI(TAG, " ntp_server=%s", settings.ntp_server);
  ESP_LOGI(TAG, " timezone=%s", settings.timezone);
  ESP_LOGI(TAG, " ota_url=%s", settings.ota_url);
  ESP_LOGI(TAG, " dhcp=%u", settings.dhcp);
  ESP_LOGI(TAG, " ip=%s", ip4addr_ntoa(&settings.ip));
  ESP_LOGI(TAG, " netmask=%s", ip4addr_ntoa(&settings.netmask));
  ESP_LOGI(TAG, " gateway=%s", ip4addr_ntoa(&settings.gateway));
  ESP_LOGI(TAG, " dns1=%s", ip4addr_ntoa(&settings.dns1));
  ESP_LOGI(TAG, " dns2=%s", ip4addr_ntoa(&settings.dns2));
}

void app_settings_reset()
{
  nvs_handle_t handle;

  ESP_LOGI(TAG, "Erasing all settings from NVS");
  if (nvs_open(NVS_KEY, NVS_READWRITE, &handle) == ESP_OK)
  {
    nvs_erase_all(handle);
    nvs_close(handle);
  }

  ESP_LOGI(TAG, "Restoring default settings");
  memset(&settings, 0, sizeof(settings));
  strncpy(settings.wifi_ssid, CONFIG_ESP_WIFI_SSID, LEN_WIFI_SSID);
  strncpy(settings.wifi_password, CONFIG_ESP_WIFI_PASSWORD, LEN_WIFI_PASSWORD);
  strncpy(settings.hostname, CONFIG_LWIP_LOCAL_HOSTNAME, LEN_HOSTNAME);
  strncpy(settings.mdns_instance, CONFIG_MDNS_INSTANCE, LEN_MDNS_INSTANCE);
  strncpy(settings.ntp_server, CONFIG_NTP_SERVER, LEN_NTP_SERVER);
  strncpy(settings.timezone, CONFIG_TIMEZONE, LEN_TIMEZONE);
  strncpy(settings.ota_url,CONFIG_FIRMWARE_UPGRADE_URL, LEN_OTA_URL);
  settings.dhcp = true;
}

void app_settings_save()
{
  //Get a handle and error object to work with
  nvs_handle_t handle;
  esp_err_t    ret;

  //Open nvs in RW mode
  ret = nvs_open(NVS_KEY, NVS_READWRITE, &handle);

  if (ret == ESP_OK)
  {
    //Opened ok
    settings.size = sizeof(settings);

    ret = nvs_set_blob(handle, "settings", &settings, sizeof(settings));

    if (ret == ESP_OK)
    {
      //Life is good, our settings are saved
      nvs_commit(handle);
      ESP_LOGI(TAG, "Saved settings to NVS");
      log_settings();
    }
    else
    {
      //Couldn't save the settings, spit out the log
      ESP_LOGE(TAG, "Error saving settings to NVS: %s", esp_err_to_name(ret));
    }

    //Time to close the NVS
    nvs_close(handle);
  }
  else
  {
    //NVS couldn't be opened, spit out the log
    ESP_LOGE(TAG, "Error opening settings: %s", esp_err_to_name(ret));
  }
}

void app_settings_startup()
{
  //Create handle for NVS storage
  nvs_handle_t handle;

  //Initialize the default partition labeled "nvs"
  esp_err_t ret = nvs_flash_init();

  //Check if things were good, otherwise wipe and start clean
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
      ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
  {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }

  ESP_ERROR_CHECK(ret);

  ESP_LOGI(TAG, "NVS Flash Init");

  //Open the nvs storage in RO mode
  ret = nvs_open(NVS_KEY, NVS_READONLY, &handle);

  //If it opened ok, then let's get the data into our settings struct
  if (ret == ESP_OK)
  {
    size_t size = sizeof(settings);

    ret = nvs_get_blob(handle, "settings", &settings, &size);

    if (ret == ESP_OK)
    {
      //Check to make sure our sizes match
      if (settings.size == sizeof(settings))
      {
        //If they do, then log it
        ESP_LOGI(TAG, "Settings loaded from NVS");
        log_settings();
      }
      else
      {
        //Sizes didn't match, so something may have changed so report it and load defaults
        ESP_LOGE(TAG, "Settings size did not match, nothing loaded");
        app_settings_reset();
        app_settings_save();
      }
    }
    else
    {
      //Couldn't get the blob, so log the error and factory reset the nvs
      ESP_LOGE(TAG, "NVS blob retrieval error: %s", esp_err_to_name(ret));
      app_settings_reset();
      app_settings_save();
    }

    //All done, so close NVS
    nvs_close(handle);
  }
  else
  {
    //NVS couldn't be opened, factory reset time
    ESP_LOGE(TAG, "Error opening NVS: %s", esp_err_to_name(ret));
    app_settings_reset();
    app_settings_save();
  }
}

void app_settings_shutdown()
{
  app_settings_save();
  nvs_flash_deinit();
}