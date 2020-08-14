set(COMPONENT_SRCS
    "app_main.c"
    "app_settings.c"
    "app_sntp.c"
    "app_wifi.c"
)

set(COMPONENT_ADD_INCLUDEDIRS
    "include"
    "fonts"
)

set(COMPONENT_REQUIRES
    esp32-camera
    mdns
    nvs_flash
    esp_http_server
)

set(COMPONENT_EMBED_FILES
    "www/index.html.gz"
    "www/style.css.gz"
    "www/script.js.gz"
)

register_component()
