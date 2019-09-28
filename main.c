/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option)any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <syslog.h>
#include <linux/ioctl.h>
#include <sys/fcntl.h> 
#include <sys/stat.h>
#include <sys/ioctl.h>

#define MIYOO_VIR_SET_MODE    _IOWR(0x100, 0, unsigned long)
#define MIYOO_VIR_SET_VER     _IOWR(0x101, 0, unsigned long)
#define MIYOO_SND_SET_VOLUME  _IOWR(0x100, 0, unsigned long)
#define MIYOO_SND_GET_VOLUME  _IOWR(0x101, 0, unsigned long)
#define MIYOO_KBD_GET_HOTKEY  _IOWR(0x100, 0, unsigned long)
#define MIYOO_KBD_SET_VER     _IOWR(0x101, 0, unsigned long)
#define MIYOO_FB0_PUT_OSD     _IOWR(0x100, 0, unsigned long)
#define MIYOO_FB0_SET_MODE    _IOWR(0x101, 0, unsigned long)
#define MIYOO_FB0_GET_VER     _IOWR(0x102, 0, unsigned long)
#define MIYOO_FB0_SET_FPBP    _IOWR(0x104, 0, unsigned long)
#define MIYOO_FB0_GET_FPBP    _IOWR(0x105, 0, unsigned long)

#define MIYOO_FBP_FILE        "/mnt/.fpbp.conf"
#define MIYOO_LID_FILE        "/mnt/.backlight.conf"
#define MIYOO_VOL_FILE        "/mnt/.volume.conf"
#define MIYOO_LID_CONF        "/sys/devices/platform/backlight/backlight/backlight/brightness"
#define MIYOO_BUTTON_FILE     "/mnt/.buttons.conf"
#define MIYOO_BATTERY    "/sys/class/power_supply/miyoo-battery/voltage_now"
#define MIYOO_BATTERY_FILE    "/mnt/.batterylow.conf"

#define BUTTON_COUNT	10

void printhelp(void){
  printf("miyooctl <COMMAND> <VALUE>: \n");
}

int main(int argc, char** argv)
{
  if (argc<2) {
    printhelp();
    return EXIT_SUCCESS;
  }

  int lid=0, vol=0, fbp=0;
  char buf[255]={0};
  unsigned long ret;
  int fb0, kbd, snd, vir;
  int battery_low=3550;
  FILE *battery_file;
  char wstr[100];
  int battery_level; 
  if (strcmp(argv[1], "volume") == 0) {
      snd = open("/dev/miyoo_snd", O_RDWR);
      if(argc==2){
        ioctl(snd, MIYOO_SND_GET_VOLUME, &vol);
        printf("Volume: %lu\n",vol);
      } else if(argc==3) {
        int new_vol = atoi(argv[2]);
        if(new_vol >= 0 && new_vol <= 10) {
          ioctl(snd, MIYOO_SND_SET_VOLUME, new_vol);
        } else {
          printhelp();
        }
      } else {
        printhelp();
      }
      close(snd);
  } else if (strcmp(argv[1], "setversion") == 0) {
    if(argc==3){
      ret=atol(argv[2]);
    }else{
      fb0 = open("/dev/miyoo_fb0", O_RDWR);
      ioctl(fb0, MIYOO_FB0_GET_VER, &ret);
    }
    kbd = open("/dev/miyoo_kbd", O_RDWR);
    ioctl(kbd, MIYOO_KBD_SET_VER, ret);
    vir = open("/dev/miyoo_vir", O_RDWR);
    ioctl(vir, MIYOO_VIR_SET_VER, ret);
    close(vir);
    if(argc!=3){
      close(fb0);
    }
    close(kbd);
  } else if (strcmp(argv[1], "version") == 0 ) {
    fb0 = open("/dev/miyoo_fb0", O_RDWR);
    ioctl(fb0, MIYOO_FB0_GET_VER, &ret);
    printf("Version: %lu\n", ret);
    close(fb0);
  } else {
    printhelp();
  }
  //setvbuf (stdout, NULL, _IONBF, 0);


  //ioctl(snd, MIYOO_SND_GET_VOLUME, &vol);
  //printf("%lu\n",vol);
 
  return EXIT_SUCCESS;
}


