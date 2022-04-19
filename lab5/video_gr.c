#include <lcom/lcf.h>
#include <lcom/lab5.h>
#include "i8042.h"
#include "i8254.h"
#include "video_gr.h"

vbe_mode_info_t vmi_p;
char* buff;
static void *video_mem;

void *(vg_init)(uint16_t mode){
  struct minix_mem_range mr;
  unsigned int vram_base;
  unsigned int vram_size;
  int r;
  
  vbe_get_mode_info_custom(mode, &vmi_p);

  vram_base = vmi_p.PhysBasePtr;
  if (vmi_p.BitsPerPixel == 15) {
    vram_size = 2 * vmi_p.XResolution * vmi_p.YResolution;
  }
  else {
    vram_size = ((vmi_p.BitsPerPixel) / 8) * vmi_p.XResolution * vmi_p.YResolution;
  }

  buff = (char *) malloc(vram_size);
  mr.mr_base = (phys_bytes) vram_base;
  mr.mr_limit = mr.mr_base + vram_size;

  if ((r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)) != 0) {
    panic("sys_privctl error\n");
  }

  video_mem = vm_map_phys(SELF, (void *)mr.mr_base, vram_size);

  reg86_t reg86;
  memset(&reg86, 0, sizeof(reg86));
  reg86.intno = 0x10;
  reg86.ax = 0x4F02;
  reg86.bx = 1 << 14 | mode;
  if(sys_int86(&reg86) != 0) {
    printf("vg_exit() falhou\n");
    return NULL;
  }
  if(video_mem == MAP_FAILED){
    panic("video_mem error\n");
    return NULL;
  }

  return video_mem;

}

int (vg_draw_pixel)(uint16_t x, uint16_t y, uint32_t color){
  if (vmi_p.XResolution < x || vmi_p.YResolution < y) {
    return 1;
  }

  if (vmi_p.BitsPerPixel == 15) {
    memcpy(buff + vmi_p.XResolution * y * 2 + x * 2, &color,2);
  }
  else {
    memcpy(buff + vmi_p.XResolution * y * (vmi_p.BitsPerPixel/8) + x * (vmi_p.BitsPerPixel/8), &color, (vmi_p.BitsPerPixel/8));
  }
  return 0;
}


int (vg_draw_hline)	(uint16_t x, uint16_t y, uint16_t len, uint32_t color){
  for (int i = 0; i < len; i++) {
    vg_draw_pixel(x + i, y, color);
  }
  return 0;
}	

void (double_buffer)(){
  if (vmi_p.BitsPerPixel == 15) {
    memcpy(video_mem, buff, 2 * vmi_p.XResolution * vmi_p.YResolution);
  }
  else {
    memcpy(video_mem, buff, (vmi_p.BitsPerPixel/8) * vmi_p.XResolution * vmi_p.YResolution);
  }
}


int (vg_draw_rectangle)	(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color){
  for (int i = 0; i < height; i++) {
    vg_draw_hline(x, y + i, width, color);
  }
  return 0;
}

void(draw_sprite)(xpm_image_t img,uint8_t *sprite,int x, int y){
  /* To be completed */
  printf("%s(): under construction\n", __func__);
  return;
}

int (vbe_get_mode_info_custom)(uint16_t mode, vbe_mode_info_t *vmi_p){
  mmap_t map;
  if(lm_alloc(sizeof(vbe_mode_info_t),&map) == NULL){
    return 1;
  }

  reg86_t reg86;
  memset(&reg86, 0, sizeof(reg86));
  reg86.intno = 0x10;
  reg86.ax = 0x4F01; 
  reg86.es = PB2BASE(map.phys);
  reg86.di = PB2OFF(map.phys);
  reg86.cx = mode;
  if( sys_int86(&reg86) != OK ) {
    printf("vg_exit() erro\n");
    return 1;
  }    
  *vmi_p = *((vbe_mode_info_t*) map.virt);
 
  if(!lm_free(&map)){
    return 1;
  }
  return 0;
}
