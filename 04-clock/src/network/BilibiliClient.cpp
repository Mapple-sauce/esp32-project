#include "BilibiliClient.h"

BilibiliClient::BilibiliClient()
    : _lastFetchMs(0)
{
    memset(&_data, 0, sizeof(_data));
    strncpy(_uid, BILIBILI_UID, sizeof(_uid) - 1);
}

BilibiliClient::~BilibiliClient() {}

void BilibiliClient::setUID(const char* uid) {
    strncpy(_uid, uid, sizeof(_uid) - 1);
}

bool BilibiliClient::fetch() {
    if (!_fetchUserInfo()) return false;
    if (!_fetchUpStats()) {
        // Up stats failure isn't fatal — user still has basic info
        Serial.println("[Bilibili] Up stats fetch failed (continuing)");
    }
    _data.lastUpdate = millis();
    _data.valid = true;
    _lastFetchMs = millis();
    return true;
}

bool BilibiliClient::isDataFresh() const {
    if (!_data.valid) return false;
    if (millis() - _data.lastUpdate > BILIBILI_UPDATE_INTERVAL_MS + 60000) return false;
    return true;
}

bool BilibiliClient::_fetchUserInfo() {
    HTTPClient http;
    String url = String("https://api.bilibili.com/x/space/acc/info?mid=") + _uid;

    http.begin(url);  // HTTP (Bilibili API supports HTTP)
    http.setConnectTimeout(5000);
    http.setTimeout(5000);
    http.addHeader("User-Agent", "Mozilla/5.0");
    http.addHeader("Referer", "https://space.bilibili.com/");

    int httpCode = http.GET();
    if (httpCode <= 0) {
        Serial.printf("[Bilibili] HTTP error: %s\n", http.errorToString(httpCode).c_str());
        http.end();
        return false;
    }

    if (httpCode != 200) {
        Serial.printf("[Bilibili] HTTP %d\n", httpCode);
        http.end();
        return false;
    }

    String payload = http.getString();
    http.end();

    // Parse JSON
    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, payload);
    if (err) {
        Serial.printf("[Bilibili] JSON parse error: %s\n", err.c_str());
        return false;
    }

    int code = doc["code"] | -1;
    if (code != 0) {
        Serial.printf("[Bilibili] API error: code=%d, msg=%s\n",
                      code, (const char*)(doc["message"] | "unknown"));
        return false;
    }

    JsonObject data = doc["data"];
    _data.followerCount = data["follower"] | 0;
    _data.followingCount = data["following"] | 0;
    _data.videoCount = data["video_count"] | 0;
    strncpy(_data.userName, data["name"] | "Unknown", sizeof(_data.userName) - 1);

    Serial.printf("[Bilibili] User: %s, Followers: %ld, Videos: %ld\n",
                  _data.userName, _data.followerCount, _data.videoCount);
    return true;
}

bool BilibiliClient::_fetchUpStats() {
    HTTPClient http;
    String url = String("https://api.bilibili.com/x/space/upstat?mid=") + _uid;

    http.begin(url);
    http.setConnectTimeout(5000);
    http.setTimeout(5000);
    http.addHeader("User-Agent", "Mozilla/5.0");
    http.addHeader("Referer", "https://space.bilibili.com/");

    int httpCode = http.GET();
    if (httpCode <= 0) {
        Serial.printf("[Bilibili-Stat] HTTP error: %s\n", http.errorToString(httpCode).c_str());
        http.end();
        return false;
    }

    if (httpCode != 200) {
        http.end();
        return false;
    }

    String payload = http.getString();
    http.end();

    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, payload);
    if (err) return false;

    int code = doc["code"] | -1;
    if (code != 0) return false;

    JsonObject data = doc["data"];
    _data.viewCount = data["archive"]["view"] | 0;
    _data.likeCount = data["archive"]["like"] | 0;

    return true;
}
