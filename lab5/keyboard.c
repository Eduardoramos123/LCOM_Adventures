#include <lcom/lcf.h>

#include "i8042.h"
#include "keyboard.h"
#include <stdint.h>

uint8_t scancode = 0;
int hook_id_kbd = 0;

int(kbc_subscribe)() {
    hook_id_kbd = KEYBOARD_IRQ1;
    int policy = IRQ_REENABLE | IRQ_EXCLUSIVE;
    int irq_line = KEYBOARD_IRQ1;

    if (sys_irqsetpolicy(irq_line, policy, &hook_id_kbd) != 0) {
        printf("Irq set policy deu MAL!!!\n");
        return 1;
    }
    return 0;
}

int(kbc_unsubscribe)() {
    if (sys_irqrmpolicy(&hook_id_kbd) != 0) {
        printf("Irq remove policy deu MAL!!!\n");
        return 1;
    }
    return 0;
}

void(kbc_ih)() {
    uint8_t stat = 0;
    for (int i = 0; i < 2; i++) {
        if (util_sys_inb(STATUS_REGISTER, &stat) != 0) {
            printf("Erro no sys inb do IH!!!!\n");
            return;
        }

        if ((stat & OUT_BUFF_BIT) != 0) {
            if (util_sys_inb(OUTPUT_BUFFER, &scancode) != 0) {
                printf("Erro ao aceder a port do BUFFER!!!!\n");
                return;
            }

            if ((stat & (KBC_PAR_ERROR | KBC_TO_ERROR)) == 0) {
                return;
            }
            else {
                printf("Erros na PARIDADE ou TIME OUT!!!!\n");
                return;
            }
        }
        tickdelay(micros_to_ticks(WAIT_KBC));
    }
}

void (kbc_print_scancode)() {
    uint8_t scancodes[2];
    bool make = false;
    uint8_t size = 1;
    if (scancode == SCAN_CONTINUE) {
        size = 2;
        scancodes[1] = scancode;
        return;
    }

    if ((scancode & BREAK_KBD) == 0) {
        make = true;
    }
    scancodes[0] = scancode;
    kbd_print_scancode(make, size, scancodes); 
    size = 1;
    make = false;
    return;

}

int (kbc_read)(){
    uint8_t stat = 0;
    while ( 1 ) {
        if (util_sys_inb(STATUS_REGISTER, &stat) != 0) {
            printf("Erro no sys inb do read!!!!\n");
            return 1;
        }

        if ((stat & OUT_BUFF_BIT) != 0) {
            if (util_sys_inb(OUTPUT_BUFFER, &scancode) != 0) {
                printf("Erro ao aceder a port do BUFFER!!!!\n");
                return 1;
            }

            if ((stat & (KBC_PAR_ERROR | KBC_TO_ERROR)) == 0) {
                return 0;
            }
            else {
                printf("Erros na PARIDADE ou TIME OUT!!!!\n");
                return 1;
            }
        }

    }
    return 1;
}
