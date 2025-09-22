#include <M5Stack.h>
#include <WiFi.h>
#include <time.h>
#include <HTTPClient.h>
#include "secrets.h"

std::vector<std::string> targets = {
    "akr1_p",
    "nyr_6r",
    "u1_8ra",
    "nyanmesi",
    "1mg_i_"
};

void connectWiFi();

void setup() {
    M5.begin();
    M5.Lcd.setTextSize(2);

    Serial.begin(9600);

    connectWiFi();
    configTime(0, 9 * 3600, "ntp.nict.jp");

    M5.Lcd.println("===");
}

void loop() {
    M5.Lcd.clear();
    M5.Lcd.setCursor(0, 0);

    struct tm timeInfo;
    getLocalTime(&timeInfo);
    M5.Lcd.printf("%d:%02d\n", timeInfo.tm_hour, timeInfo.tm_min);

    HTTPClient http;

    for (std::string target : targets) {
        std::string url = "https://apiv2.twitcasting.tv/users/";
        url += target;
        url += "/current_live";

        std::string authHeader = "Bearer ";
        authHeader += TOKEN;

        http.begin(url.c_str());

        http.addHeader("Accept", "application/json");
        http.addHeader("X-Api-Version", "2.0");
        http.addHeader("Authorization", authHeader.c_str());

        int code = http.GET();
        if (code <= 0) {
            std::string errorMessage = "Failed to fetch live; user=";
            errorMessage += target;
            errorMessage += ", err=";
            errorMessage += http.errorToString(code).c_str();
            M5.Lcd.println(errorMessage.c_str());
            Serial.println(errorMessage.c_str());
            continue;
        }

        if (code == HTTP_CODE_OK) {
            M5.Lcd.println(target.c_str());
        }
    }

    delay(60000);
}

void connectWiFi() {
    Serial.println("Connecting to Wi-Fi...");
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
    }
    Serial.print("Connected! IP address: ");
    Serial.println(WiFi.localIP());
}
