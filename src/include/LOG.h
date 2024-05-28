// log.h

#define LOG(format, ...) printf(format "\n", __VA_ARGS__)
#define ERR(format, ...) LOG("Error: " format, __VA_ARGS__)
