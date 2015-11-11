typedef unsigned char byte;

#define CONTROL_PORT 0x0CF8
#define INFO_PORT 0x0CFC

#define BUS_SHIFT 16
#define DEVICE_SHIFT 11
#define FUNCTION_SHIFT 8
#define REGISTER_SHIFT 2
#define CONTROL_BIT (1 << 31)

#define CLASS_CODE_REGISTER 2
#define HEADER_TYPE_REGISTER 3
