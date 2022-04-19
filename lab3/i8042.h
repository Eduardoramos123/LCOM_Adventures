#ifndef _LCOM_I8042_H_
#define _LCOM_I8042_H_

#include <lcom/lcf.h>

/** @defgroup i8042 i8042
 * @{
 *
 * Constants for programming the i8042 Keyboard.
 */

/*waiting for KBC*/
#define WAIT_KBC 20000 
#define SCAN_CONTINUE 0xE0
#define BREAK_KBD 0x80 


#define KEYBOARD_IRQ1 1
#define TIMER_IRQ0 0

/*i8042 ports*/

#define STATUS_REGISTER 0x64 

#define INPUT_BUFFER_COMMANDS 0x64 

#define INPUT_BUFFER_ARGUMENTS 0x60 

#define OUTPUT_BUFFER 0x60 

/*status bits*/

#define OUT_BUFF_BIT BIT(0)

#define IN_BUFF BIT(1)

#define SYS_FLAG BIT(2)

#define DATA_CMD BIT(3)

#define KBC_PAR_ERROR BIT(7)

#define KBC_TO_ERROR BIT(6)

/*Commands*/

#define READ_COMMAND 0x20

#define WRITE_COMMAND 0x60

/*KEYS*/

#define ESC 0x81 

void (double_buffer)();




#endif
