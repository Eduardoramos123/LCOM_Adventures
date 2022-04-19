#include <lcom/lcf.h>

#include <stdint.h>

int cnt = 0;

int (util_get_LSB)(uint16_t val, uint8_t *lsb) {
  uint16_t mask = 0x00FF;
  uint16_t res = val & mask;
  *lsb = (uint8_t) res;
  return 0;
}

int (util_get_MSB)(uint16_t val, uint8_t *msb) {
  uint16_t mask = 0xFF00;
  uint16_t res = val & mask;
  res = val >> 8;
  *msb = (uint8_t) res;
  return 0;
}

int (util_sys_inb)(int port, uint8_t *value) {
  uint32_t mask = 0;
  int flag = sys_inb(port, &mask);

#ifdef LAB3
  cnt++;
#endif
  
  if (flag == 0) {
    *value = (uint8_t) mask;
    return 0;
  }
  else {
    return 1;
  }
  return 1;
}
