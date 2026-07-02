#ifndef BILIBILI_CLIENT_H
#define BILIBILI_CLIENT_H

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "config.h"

class BilibiliClient {
public:
    BilibiliClient();
    ~BilibiliClient();

    // Set the UID to track
    void setUID(const char* uid);

    // Fetch follower count and stats from Bilibili API
    bool fetch();

    // Get latest data
    BilibiliData getData() const { return _data; }

    // Check if data is fresh
    bool isDataFresh() const;

private:
    BilibiliData _data;
    char _uid[16];
    unsigned long _lastFetchMs;

    // Fetch user info: name, followers, following, video count
    bool _fetchUserInfo();

    // Fetch up stats: total views, likes
    bool _fetchUpStats();
};

#endif // BILIBILI_CLIENT_H
