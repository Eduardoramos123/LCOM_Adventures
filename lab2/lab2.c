#include <lcom/lcf.h>
#include <lcom/lab2.h>
#include "i8254.h"
#include <stdbool.h>
#include <stdint.h>


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
  /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}
