#include "PreferencesManager.h"

PreferencesManager::PreferencesManager(const String& namespaceName) : ns(namespaceName) {}

PreferencesManager::~PreferencesManager() {}

bool PreferencesManager::saveInternal(const String& key, const String& value) {
    preferences.begin(ns.c_str(), false);
    bool result = preferences.putString(key.c_str(), value);
    preferences.end();
    return result;
}

bool PreferencesManager::saveInternal(const String& key, const char* value) {
    preferences.begin(ns.c_str(), false);
    bool result = preferences.putString(key.c_str(), value);
    preferences.end();
    return result;
}

bool PreferencesManager::saveInternal(const String& key, int value) {
    preferences.begin(ns.c_str(), false);
    bool result = preferences.putInt(key.c_str(), value);
    preferences.end();
    return result;
}

bool PreferencesManager::saveInternal(const String& key, float value) {
    preferences.begin(ns.c_str(), false);
    bool result = preferences.putFloat(key.c_str(), value);
    preferences.end();
    return result;
}

bool PreferencesManager::saveInternal(const String& key, bool value) {
    preferences.begin(ns.c_str(), false);
    bool result = preferences.putBool(key.c_str(), value);
    preferences.end();
    return result;
}

bool PreferencesManager::saveInternal(const String& key, uint32_t value) {
    preferences.begin(ns.c_str(), false);
    bool result = preferences.putUInt(key.c_str(), value);
    preferences.end();
    return result;
}

bool PreferencesManager::saveInternal(const String& key, int32_t value) {
    preferences.begin(ns.c_str(), false);
    bool result = preferences.putInt(key.c_str(), value);
    preferences.end();
    return result;
}

bool PreferencesManager::saveInternal(const String& key, uint64_t value) {
    preferences.begin(ns.c_str(), false);
    bool result = preferences.putULong64(key.c_str(), value);
    preferences.end();
    return result;
}

bool PreferencesManager::saveInternal(const String& key, int64_t value) {
    preferences.begin(ns.c_str(), false);
    bool result = preferences.putLong64(key.c_str(), value);
    preferences.end();
    return result;
}

// Loaders

String PreferencesManager::loadString(const String& key, const String& defaultValue) {
    preferences.begin(ns.c_str(), true);
    String result = preferences.getString(key.c_str(), defaultValue);
    preferences.end();
    return result;
}

int PreferencesManager::loadInt(const String& key, int defaultValue) {
    preferences.begin(ns.c_str(), true);
    int result = preferences.getInt(key.c_str(), defaultValue);
    preferences.end();
    return result;
}

float PreferencesManager::loadFloat(const String& key, float defaultValue) {
    preferences.begin(ns.c_str(), true);
    float result = preferences.getFloat(key.c_str(), defaultValue);
    preferences.end();
    return result;
}

bool PreferencesManager::loadBool(const String& key, bool defaultValue) {
    preferences.begin(ns.c_str(), true);
    bool result = preferences.getBool(key.c_str(), defaultValue);
    preferences.end();
    return result;
}

uint32_t PreferencesManager::loadUInt(const String& key, uint32_t defaultValue) {
    preferences.begin(ns.c_str(), true);
    uint32_t result = preferences.getUInt(key.c_str(), defaultValue);
    preferences.end();
    return result;
}

int32_t PreferencesManager::loadInt32(const String& key, int32_t defaultValue) {
    preferences.begin(ns.c_str(), true);
    int32_t result = preferences.getInt(key.c_str(), defaultValue);
    preferences.end();
    return result;
}

uint64_t PreferencesManager::loadUInt64(const String& key, uint64_t defaultValue) {
    preferences.begin(ns.c_str(), true);
    uint64_t result = preferences.getULong64(key.c_str(), defaultValue);
    preferences.end();
    return result;
}

int64_t PreferencesManager::loadInt64(const String& key, int64_t defaultValue) {
    preferences.begin(ns.c_str(), true);
    int64_t result = preferences.getLong64(key.c_str(), defaultValue);
    preferences.end();
    return result;
}

//Misc

void PreferencesManager::clearAll() {
    preferences.begin(ns.c_str(), false);
    preferences.clear();                   
    preferences.end();
}