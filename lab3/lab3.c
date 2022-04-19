#include <lcom/lcf.h>

#include <lcom/lab3.h>
#include "i8042.h"
#include "keyboard.h"
#include <stdbool.h>
#include <stdint.h>

extern int cnt;
extern int counter;
extern uint8_t scancode;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/g05/lab3/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/g05/lab3/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(kbd_test_scan)() {
    uint8_t mask = KEYBOARD_IRQ1;
    uint32_t irq_set = BIT(mask);

    kbc_subscribe();

    int ipc_status;
    message msg;
    double r = 0;

    while (scancode != ESC) { 
        if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
            printf("driver_receive failed with: %d", r);
            continue;
        }

        if (is_ipc_notify(ipc_status)) { 
            switch (_ENDPOINT_P(msg.m_source)) {
                case HARDWARE:                             
                    if (msg.m_notify.interrupts & irq_set) { 
                        kbc_ih(); 
                        kbc_print_scancode(); 
                    }
                    break;
                default: break;
            }
        }
    }

    kbc_unsubscribe();
    kbd_print_no_sysinb(cnt);
    return 0;
}

int(kbd_test_poll)() {
    while (scancode != ESC) {
        if (kbc_read() != 0) {
            printf("Erro no READ!!!\n");
            return 1;
        }
        kbc_print_scancode();
    }

    if (sys_outb(INPUT_BUFFER_COMMANDS, READ_COMMAND) != 0) {
        printf("Erro no outb!!!!\n");
        return 1;
    }

    kbc_read();

    if (sys_outb(INPUT_BUFFER_COMMANDS, WRITE_COMMAND) != 0) {
        printf("Erro no outb!!!!\n");
        return 1;
    }

    if (sys_outb(INPUT_BUFFER_ARGUMENTS, scancode | BIT(0)) != 0) {
        printf("Erro no outb!!!!\n");
        return 1;
    }

    kbd_print_no_sysinb(cnt);
    return 0;
}

int(kbd_test_timed_scan)(uint8_t n) {
  uint8_t mask1 = KEYBOARD_IRQ1;
  uint8_t mask2 = TIMER_IRQ0;
  uint32_t irq_set_kbd = BIT(mask1);
  uint32_t irq_set_timer = BIT(mask2);

  kbc_subscribe();
  timer_subscribe_int(&mask2);

  int ipc_status;
  message msg;
  double r = 0;

  while (scancode != ESC) { 
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { 
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:                             
          if (msg.m_notify.interrupts & irq_set_kbd) { 
            kbc_ih(); 
            kbc_print_scancode();
            counter = 0;  
          }
          if (msg.m_notify.interrupts & irq_set_timer) { 
            timer_int_handler();
            printf("time = %d ms\n",counter);
            if(counter >= n * 60){ 
              scancode = ESC; 
            }
          }

          break;
        default: break;
      }
    }
  }

  kbc_unsubscribe();
  timer_unsubscribe_int();

  kbd_print_no_sysinb(cnt);
  return 0;
}
