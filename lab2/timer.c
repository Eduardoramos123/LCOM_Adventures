#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>
#include <utils.h>
#include "i8254.h"

int (timer_set_frequency)(uint8_t timer, uint32_t freq) {
  uint8_t st = 0;
  if(timer_get_conf(timer, &st) != 0) {
    printf("timer_get_conf NAO esta bem\n");
  }

  uint8_t keepers = 0x0F & st;

  uint8_t control_word = 0;
  
  if (timer == 0) {
    control_word |= TIMER_SEL0 | TIMER_LSB_MSB | keepers;
  }
  else if (timer == 1) {
    control_word |= TIMER_SEL1 | TIMER_LSB_MSB | keepers;
  }
  else if (timer == 2) {
    control_word |= TIMER_SEL2 | TIMER_LSB_MSB | keepers;
  }
  else {
    return 1;
  }

  uint16_t final_freq = TIMER_FREQ / freq;

  sys_outb(TIMER_CTRL, control_word);

  uint8_t lsb;
  uint8_t msb;
  util_get_LSB(final_freq, &lsb);
  util_get_MSB(final_freq, &msb);

  if (timer == 0) {
    sys_outb(TIMER_0, lsb);
    sys_outb(TIMER_0, msb);
    return 0;
  }
  else if (timer == 1) {
    sys_outb(TIMER_1, lsb);
    sys_outb(TIMER_1, msb);
    return 0;
  }
  else if (timer == 2) {
    sys_outb(TIMER_2, lsb);
    sys_outb(TIMER_2, msb);
    return 0;
  }
  else {
    return 1;
  }  

  return 1;
}

int (timer_subscribe_int)(uint8_t *bit_no) {
  /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}

int (timer_unsubscribe_int)() {
  /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}

void (timer_int_handler)() {
  /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);
}

int (timer_get_conf)(uint8_t timer, uint8_t *st) {
  uint8_t temp = 0;

  if (timer == 0) {
    temp |= TIMER_RB_CMD | TIMER_RB_COUNT_ |TIMER_RB_SEL(0);

    if (sys_outb(TIMER_CTRL, temp) != 0) {
      return 1;
    }

    if (util_sys_inb(TIMER_0, st) == 0) {
      return 0;
    }
    else {
      return 1;
    }
  }

  if (timer == 1) {
    temp |= TIMER_RB_CMD | TIMER_RB_COUNT_ | TIMER_RB_SEL(1);

    if (sys_outb(TIMER_CTRL, temp) != 0) {
      return 1;
    }

    if (util_sys_inb(TIMER_1, st) == 0) {
      return 0;
    }
    else {
      return 1;
    }
  }

  if (timer == 2) {
    temp |= TIMER_RB_CMD | TIMER_RB_COUNT_ | TIMER_RB_SEL(2);

    if (sys_outb(TIMER_CTRL, temp) != 0) {
      return 1;
    }

    if (util_sys_inb(TIMER_2, st) == 0) {
      return 0;
    }
    else {
      return 1;
    }
  }
  return 1;
}

int (timer_display_conf)(uint8_t timer, uint8_t st, enum timer_status_field field) {
  union timer_status_field_val conf;

  uint8_t mask_initial = 0;
  uint8_t mask_counting_mode = 0;
  uint8_t mask_bcd = 0;

  mask_initial |= BIT(5) | BIT(4);
  mask_counting_mode |= BIT(3) | BIT(2) | BIT(1);
  mask_bcd |= BIT(0);

  mask_initial &= st;
  mask_counting_mode &= st;
  mask_bcd &= st;

  mask_initial = mask_initial >> 4;
  mask_counting_mode = mask_counting_mode >> 1;

  switch (field) {
    case tsf_all:
      conf.byte = st;
      break;
    
    case tsf_initial:
      if (mask_initial == 1) {
        conf.in_mode = LSB_only;
      }
      else if (mask_initial == 2) {
        conf.in_mode = MSB_only;
      }
      else if (mask_initial == 3) {
        conf.in_mode = MSB_after_LSB;
      }
      else {
        conf.in_mode = INVAL_val;
      }
      break;
    
    case tsf_mode:
      conf.count_mode = mask_counting_mode;
      break;
    
    case tsf_base:
      if (mask_bcd == 0) {
        conf.bcd = false;
      }
      else {
        conf.bcd = true;
      }
      break;

  }

  if (timer_print_config(timer, field, conf) != 0) {
    return 1;
  }

  return 0;
}
