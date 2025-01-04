#ifndef STATUS_H
#define STATUS_H

#define ZHIOS_ALL_OK 0
#define EINARG 1
#define EMEM 2
#define EIO 3

#define ISERR(value) value < 0
#define ERROR_I(value) (int)value

#define MAX_PROCESS_REACHED -301

#endif