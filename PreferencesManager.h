#ifndef PREFERENCES_MANAGER_H
#define PREFERENCES_MANAGER_H

#include <Preferences.h>
#include <Arduino.h>
#include <type_traits>

class PreferencesManager {
public:
    PreferencesManager(const String& namespaceName);
    ~PreferencesManager();

    template <typename T>
    bool save(const String& key, const T& value);

    template <typename T>
    T load(const String& key, const T& defaultValue);

    void clearAll();

private:
    Preferences preferences;
    String ns;

    bool saveInternal(const String& key, const String& value);
    bool saveInternal(const String& key, const char* value);
    bool saveInternal(const String& key, int value);
    bool saveInternal(const String& key, float value);
    bool saveInternal(const String& key, bool value);
    bool saveInternal(const String& key, uint32_t value);
    bool saveInternal(const String& key, int32_t value);
    bool saveInternal(const String& key, uint64_t value);
    bool saveInternal(const String& key, int64_t value);

    String loadString(const String& key, const String& defaultValue);
    int loadInt(const String& key, int defaultValue);
    float loadFloat(const String& key, float defaultValue);
    bool loadBool(const String& key, bool defaultValue);
    uint32_t loadUInt(const String& key, uint32_t defaultValue);
    int32_t loadInt32(const String& key, int32_t defaultValue);
    uint64_t loadUInt64(const String& key, uint64_t defaultValue);
    int64_t loadInt64(const String& key, int64_t defaultValue);
};

// ----------------------------
// Implementación de plantillas
// ----------------------------

template <typename T>
bool PreferencesManager::save(const String& key, const T& value) {
    if constexpr (std::is_same<T, String>::value) {
        return saveInternal(key, value);
    } else if constexpr (std::is_same<T, const char*>::value) {
        return saveInternal(key, value);
    } else if constexpr (std::is_same<T, int>::value) {
        return saveInternal(key, value);
    } else if constexpr (std::is_same<T, float>::value) {
        return saveInternal(key, value);
    } else if constexpr (std::is_same<T, bool>::value) {
        return saveInternal(key, value);
    } else if constexpr (std::is_same<T, uint32_t>::value) {
        return saveInternal(key, value);
    } else if constexpr (std::is_same<T, int32_t>::value) {
        return saveInternal(key, value);
    } else if constexpr (std::is_same<T, uint64_t>::value) {
        return saveInternal(key, value);
    } else if constexpr (std::is_same<T, int64_t>::value) {
        return saveInternal(key, value);
    } else {
        return false;
    }
}

template <typename T>
T PreferencesManager::load(const String& key, const T& defaultValue) {
    if constexpr (std::is_same<T, String>::value) {
        return loadString(key, defaultValue);
    } else if constexpr (std::is_same<T, const char*>::value) {
        return loadString(key, String(defaultValue)).c_str();
    } else if constexpr (std::is_same<T, int>::value) {
        return loadInt(key, defaultValue);
    } else if constexpr (std::is_same<T, float>::value) {
        return loadFloat(key, defaultValue);
    } else if constexpr (std::is_same<T, bool>::value) {
        return loadBool(key, defaultValue);
    } else if constexpr (std::is_same<T, uint32_t>::value) {
        return loadUInt(key, defaultValue);
    } else if constexpr (std::is_same<T, int32_t>::value) {
        return loadInt32(key, defaultValue);
    } else if constexpr (std::is_same<T, uint64_t>::value) {
        return loadUInt64(key, defaultValue);
    } else if constexpr (std::is_same<T, int64_t>::value) {
        return loadInt64(key, defaultValue);
    } else {
        return defaultValue;
    }
}

#endif // PREFERENCES_MANAGER_H
