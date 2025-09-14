#ifndef CONFIG_CONSTANTS_H
#define CONFIG_CONSTANTS_H

// General definitions
constexpr int MAX_TOKENS = 3;
constexpr int BUFFER_SIZE = 512;
constexpr int HTTP_PORT = 80;
constexpr int WS_PORT = 8080;

// Sentinel values
constexpr float BAD_FLOAT = NAN;
constexpr long BAD_LONG = -1;

// WebSocket
constexpr int QUEUE_SIZE = 10;
constexpr int PAYLOAD_MAX_SIZE = 512;
constexpr int DEFAULT_SAMPLE_RATE_HZ = 1000;

// String constants
constexpr const char* ERR = "ERR";
constexpr const char* NR = "NR";

#endif // CONFIG_CONSTANTS_H
