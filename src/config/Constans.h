#ifndef CONFIG_CONSTANTS_H
#define CONFIG_CONSTANTS_H

// General definitions
constexpr int MAX_TOKENS = 3;
constexpr int HTTP_PORT = 80;
constexpr int WS_PORT = 8080;

// Sentinel values
constexpr float BAD_FLOAT = NAN;
constexpr long BAD_LONG = -1;

// Paquetes
constexpr int BUFFER_SIZE = 256; //Tamaño del buffer por mensaje

// WebSocket
constexpr int QUEUE_SIZE = 256; //Tamaño de la cola 
constexpr int PAYLOAD_MAX_SIZE = BUFFER_SIZE; //Tamaño maximo por mensaje de la cola

// String constants
constexpr const char* ERR = "ERR";
constexpr const char* NR = "NR";

#endif // CONFIG_CONSTANTS_H
