#include <lcom/lcf.h>
#include <lcom/lab2.h>
#include "i8254.h"
#include <stdbool.h>
#include <stdint.h>

extern int counter;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/g05/lab2/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/g05/lab2/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(timer_test_read_config)(uint8_t timer, enum timer_status_field field) {
  uint8_t st = 0;

  if (timer_get_conf(timer, &st) == 0) {
    printf("timer_get_conf esta bem!\n");
    if(timer_display_conf(timer, st, field) == 0) {
      printf("timer_display_conf esta bem\n");
      return 0;
    }
    else {
      printf("timer_display_conf NAO esta bem\n");
      return 1;
    }
  }
  else {
    printf("timer_get_conf NAO esta bem!\n");
    return 1;
  }

  return 1;
}

int(timer_test_time_base)(uint8_t timer, uint32_t freq) {
  if (timer_set_frequency(timer, freq) != 0) {
    printf("timer_set_frequency NAO esta bem\n");
    return 1;
  }
  return 0;
}

int(timer_test_int)(uint8_t time) {
  uint8_t mask = TIMER0_IRQ;
  uint32_t irq_set = BIT(mask);

  timer_subscribe_int(&mask);

  int ipc_status;
  message msg;
  double r = 0;
  int freq = 60;
  if (timer_set_frequency(0, freq)  != 0) {
    timer_unsubscribe_int();
    return 1;
  }

  while (counter < freq * time) {
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }

    if (is_ipc_notify(ipc_status)) { /* received notification */
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:                             /* hardware interrupt notification */
          if (msg.m_notify.interrupts & irq_set) { /* subscribed interrupt */
            timer_int_handler();
            if (counter % freq == 0) {
              timer_print_elapsed_time();
            }
          }
          break;
        default:
          break;
      }
    }
  }
  timer_unsubscribe_int();
  return 0;
}
