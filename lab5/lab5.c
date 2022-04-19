// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <lcom/lab5.h>

#include "video_gr.h"
#include <stdint.h>
#include <stdio.h>

// Any header files included below this line should have been created by you
#include "i8042.h"
#include "keyboard.h"
#include "timer.h"

extern int counter;
extern uint8_t scancode;
extern vbe_mode_info_t vmi_p;

// Any header files included below this line should have been created by you

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/g05/lab5/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/g05/lab5/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(video_test_init)(uint16_t mode, uint8_t delay) {
  if (vg_init(mode) == NULL) {
    return 1;
  }

  sleep(delay);
  vg_exit();

  return 0;
}

int(video_test_rectangle)(uint16_t mode, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {
  uint8_t mask = KEYBOARD_IRQ1;
  uint32_t irq_set = BIT(mask);
  kbc_subscribe();
  int ipc_status;
  message msg;
  double r = 0;
  vg_init(mode);
  vg_draw_rectangle(x, y, width, height, color); 
  double_buffer();

  while (scancode != ESC) { //critério ESC
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { 
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:                             
          if (msg.m_notify.interrupts & irq_set) { 
            kbc_ih();
          }
          break;
        default: break;
      }
    }
  }
  kbc_unsubscribe();
  vg_exit();
  return 0;
}

int(video_test_pattern)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step) {
  vg_init(mode);
  int ipc_status;
  message msg;
  double r = 0;
  int color;

  uint32_t R_first;
  uint32_t G_first;
  uint32_t B_first;

  switch (mode) {
    case 0x105:
      break;

    case 0x110:
      R_first = (first & 0x00007C00) >> vmi_p.RedFieldPosition;
      G_first = (first & 0x000003E0) >> vmi_p.GreenFieldPosition;
      B_first = (first & 0x0000001F) >> vmi_p.BlueFieldPosition;
      break;

    case 0x115:
      R_first = (first & 0x00FF0000) >> vmi_p.RedFieldPosition;
      G_first = (first & 0x0000FF00) >> vmi_p.GreenFieldPosition;
      B_first = (first & 0x000000FF) >> vmi_p.BlueFieldPosition;
      break;

    case 0x11A:
      R_first = (first & 0x0000F800) >> vmi_p.RedFieldPosition;
      G_first = (first & 0x000007E0) >> vmi_p.GreenFieldPosition;
      B_first = (first & 0x0000001F) >> vmi_p.BlueFieldPosition;
      break;

    case 0x14C:
      R_first = (first & 0x00FF0000) >> vmi_p.RedFieldPosition;
      G_first = (first & 0x0000FF00) >> vmi_p.GreenFieldPosition;
      B_first = (first & 0x000000FF) >> vmi_p.BlueFieldPosition;
      break;
  }

  int R, G, B;
  int x_rectangles = vmi_p.XResolution / no_rectangles;
  int y_rectangles = vmi_p.YResolution / no_rectangles;

  uint8_t mask = KEYBOARD_IRQ1;
  uint32_t irq_set = BIT(mask);
  kbc_subscribe();

  for(int i = 0; i < no_rectangles; i++){ 
    for(int j = 0; j < no_rectangles; j++){ 
      switch (mode) {
        case 0x105:
          color = (first + (i * no_rectangles + j) * step) % (1 << vmi_p.BitsPerPixel);
          break;
        default:
          R = (R_first + j * step) % (1 << vmi_p.RedMaskSize);
          G = (G_first + i * step) % (1 << vmi_p.GreenMaskSize);
          B = (B_first + (j + i) * step) % (1 << vmi_p.BlueMaskSize);

          color = 0 | (R << vmi_p.RedFieldPosition) | (G << vmi_p.GreenFieldPosition) | (B << vmi_p.BlueFieldPosition);
          break;
    }

    vg_draw_rectangle(j * x_rectangles, i * y_rectangles, x_rectangles, y_rectangles, color);
    }
     if (i * y_rectangles > vmi_p.YResolution) {
      break;
    }
  }

  double_buffer();
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
          }
          break;
        default: break;
      }
    }
  }
  kbc_unsubscribe();

  vg_exit();

  return 0;
}

int(video_test_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y) {
  /* To be completed */
  printf("%s(%8p, %u, %u): under construction\n", __func__, xpm, x, y);

  return 1;
}

int(video_test_move)(xpm_map_t xpm, uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf,
                     int16_t speed, uint8_t fr_rate) {
  /* To be completed */
  printf("%s(%8p, %u, %u, %u, %u, %d, %u): under construction\n",
         __func__, xpm, xi, yi, xf, yf, speed, fr_rate);

  return 1;
}

int(video_test_controller)() {
  /* To be completed */
  printf("%s(): under construction\n", __func__);

  return 1;
}
