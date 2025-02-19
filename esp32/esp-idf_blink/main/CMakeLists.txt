idf_component_register(SRCS "blink_example_main.c"
                       INCLUDE_DIRS "."
        REQUIRES esp_wifi
        REQUIRES wpa_supplicant
        REQUIRES nvs_flash
        REQUIRES esp_http_server
)
set(EXTRA_COMPONENT_DIRS $ENV{IDF_PATH}/components/wifi_provisioning)
