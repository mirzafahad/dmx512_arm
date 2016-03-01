#ifndef __SERIAL_H__
 #define __SERIAL_H__


//Functions
void serial_init(int baud_rate);
void serial_puts(char str[]);
char serial_getc();

#endif
