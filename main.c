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
#include <sys/stat.h>
#include <sys/ioctl.h>

#include <getopt.h>
#include <libgen.h>
#include <errno.h>

#define MIYOO_VIR_SET_MODE    _IOWR(0x100, 0, unsigned long)
#define MIYOO_VIR_SET_VER     _IOWR(0x101, 0, unsigned long)
#define MIYOO_SND_SET_VOLUME  _IOWR(0x100, 0, unsigned long)
#define MIYOO_SND_GET_VOLUME  _IOWR(0x101, 0, unsigned long)
#define MIYOO_KBD_GET_HOTKEY  _IOWR(0x100, 0, unsigned long)
#define MIYOO_KBD_SET_VER     _IOWR(0x101, 0, unsigned long)
#define MIYOO_LAYOUT_SET_VER  _IOWR(0x103, 0, unsigned long)
#define MIYOO_FB0_PUT_OSD     _IOWR(0x100, 0, unsigned long)
#define MIYOO_FB0_SET_MODE    _IOWR(0x101, 0, unsigned long)
#define MIYOO_FB0_GET_VER     _IOWR(0x102, 0, unsigned long)
#define MIYOO_FB0_SET_FPBP    _IOWR(0x104, 0, unsigned long)
#define MIYOO_FB0_GET_FPBP    _IOWR(0x105, 0, unsigned long)

#define MIYOO_FBP_FILE        "/mnt/.fpbp.conf"
#define MIYOO_LID_FILE        "/mnt/.backlight.conf"
#define MIYOO_VOL_FILE        "/mnt/.volume.conf"
#define MIYOO_BUTTON_FILE     "/mnt/.buttons.conf"
#define MIYOO_BATTERY_FILE    "/mnt/.batterylow.conf"
#define MIYOO_LID_CONF        "/sys/devices/platform/backlight/backlight/backlight/brightness"
#define MIYOO_BATTERY         "/sys/class/power_supply/miyoo-battery/voltage_now"
#define MIYOO_SND_FILE        "/dev/miyoo_snd"
#define MIYOO_FB0_FILE        "/dev/miyoo_fb0"
#define MIYOO_KBD_FILE        "/dev/miyoo_kbd"
#define MIYOO_VIR_FILE        "/dev/miyoo_vir"

#define OPTSTR                "hivV:k:l:m:M:s:f:"
#define USAGE_FMT             "%s [-h] [-i] [-v] [-V volume(0-10)]         [-m rumble_ver(1-4)] [-M rumble_mode(0-1)] [-s screen_ver(1-4)]\n         [-f fpbp_hexbyte]\n         [-k keypad_ver(1-6)]\n  [-l layout_ver(1-3)]\n"
#define DEFAULT_PROGNAME      "miyooctl"
#define ERR_DO_THE_DEED       "the main action went wrong somehow"
#define ERR_OPEN_FILE(x)      "open('"x"')"

extern int errno;
extern char *optarg;
extern int opterr, optind;

static void usage(char *progname, int opt) {
    if (opt == '?') fprintf(stderr, "Unknown option '-%c'\n", optopt);
    if (opt == ':') fprintf(stderr, "Missing argument for option '-%c'\n", optopt);
    fprintf(stderr, USAGE_FMT, (progname ? progname : DEFAULT_PROGNAME));
    exit(EXIT_FAILURE);
    /* NOTREACHED */
}

static int parse_int(const char *s, int base, int min, int max) {
    char *endptr;

    int res = (int)strtol(s, &endptr, base);
    if(endptr == s || *endptr != '\0') {
        fprintf(stderr, DEFAULT_PROGNAME ": error parsing number: got %s\n", s);
        return -1;
    } else if (errno == ERANGE || res < min || res > max) {
        fprintf(stderr, DEFAULT_PROGNAME ": number out of range %s\n", s);
        return -1;
    }
    return res;
}

typedef struct {
    int     just_want_info;
    int     verbose;
    int     screen_ver;
    int     fpbp;
    int     keypad_ver;
    int     layout_ver;
    int     rumble_ver;
    int     rumble_mode;
    int     volume;
    char    *progname;
} options_t;

int do_the_deed(options_t *opts);

int main(int argc, char** argv) {
    int opt;
    options_t options = { 0, 0, -1, -1, -1, -1, -1, -1, basename(argv[0]) };
    opterr = 0;

    while ((opt = getopt(argc, argv, OPTSTR)) != EOF) {
        switch(opt) {
            case 'i':
                options.just_want_info = 1;
                break;
            case 'v':
                options.verbose = 1;
                break;
            case 'V':
                options.volume = parse_int(optarg, 10, 0, 10);
                break;
            case 'k':
                options.keypad_ver = parse_int(optarg, 10, 1, 6);
                break;
            case 'l':
                options.layout_ver = parse_int(optarg, 10, 1, 3);
                break;
            case 'm':
                options.rumble_ver = parse_int(optarg, 10, 1, 4);
                break;
            case 'M':
                options.rumble_mode = parse_int(optarg, 10, 0, 1);
                break;
            case 's':
                options.screen_ver = parse_int(optarg, 10, 1, 4);
                break;
            case 'f':
                options.fpbp = parse_int(optarg, 16, 1, 4);
                break;
            case 'h':
            default:
                usage(options.progname, opt);
                /* NOTREACHED */
                break;
        }
    }

    if(do_the_deed(&options) != EXIT_SUCCESS) {
        perror(ERR_DO_THE_DEED);
        exit(EXIT_FAILURE);
        /* NOTREACHED */
    }

    return EXIT_SUCCESS;
}
    
int do_the_deed(options_t *opts) {
    options_t current = { 0, 0, -1, -1, -1, -1, -1, -1, NULL };
    int f;

    if(opts->volume != -1 || opts->just_want_info) {
        if(!(f = open(MIYOO_SND_FILE, O_RDWR))) {
            perror(ERR_OPEN_FILE(MIYOO_SND_FILE));
            return EXIT_FAILURE;
            /* NOTREACHED */
        }
        ioctl(f, MIYOO_SND_GET_VOLUME, &(current.volume));
        if(opts->volume != -1) {
            if(opts->verbose) {
                fprintf(stdout, "%s: setting volume to %d\n", opts->progname, opts->volume);
            }
            ioctl(f, MIYOO_SND_SET_VOLUME, opts->volume);
        }
        close(f);
    }

    if(opts->screen_ver != -1 || opts->fpbp != -1 || opts->just_want_info) {
        if(!(f = open(MIYOO_FB0_FILE, O_RDWR))) {
            perror(ERR_OPEN_FILE(MIYOO_FB0_FILE));
            return EXIT_FAILURE;
            /* NOTREACHED */
        }
        ioctl(f, MIYOO_FB0_GET_VER, &(current.screen_ver));
        ioctl(f, MIYOO_FB0_GET_FPBP, &(current.fpbp));
        if(opts->screen_ver != -1) {
            if(opts->verbose) {
                fprintf(stdout, "%s: setting screen version to %d\n", opts->progname, opts->screen_ver);
            }
            ioctl(f, MIYOO_FB0_SET_MODE, opts->screen_ver);
        }
        if(opts->fpbp != -1) {
            if(opts->verbose) {
                fprintf(stdout, "%s: setting fp to 0x%x and bp to 0x%x\n", opts->progname, ((uint8_t)opts->fpbp&0xF0)>>4, (uint8_t)opts->fpbp&0xF);
            }
            ioctl(f, MIYOO_FB0_SET_FPBP, (uint8_t)opts->fpbp);
        }
        close(f);
    }

    if(opts->keypad_ver != -1) {
        if(!(f = open(MIYOO_KBD_FILE, O_RDWR))) {
            perror(ERR_OPEN_FILE(MIYOO_KBD_FILE));
            return EXIT_FAILURE;
            /* NOTREACHED */
        }
        if(opts->verbose) {
            fprintf(stdout, "%s: setting keypad version to %d\n", opts->progname, opts->keypad_ver);
        }
        ioctl(f, MIYOO_KBD_SET_VER, opts->keypad_ver);
        close(f);
    }

    if(opts->layout_ver != -1) {
        if(!(f = open(MIYOO_KBD_FILE, O_RDWR))) {
            perror(ERR_OPEN_FILE(MIYOO_KBD_FILE));
            return EXIT_FAILURE;
            /* NOTREACHED */
        }
        if(opts->verbose) {
            fprintf(stdout, "%s: setting keypad layout version to %d\n", opts->progname, opts->layout_ver);
        }
        ioctl(f, MIYOO_LAYOUT_SET_VER, opts->layout_ver);
        close(f);
    }

    if(opts->rumble_ver != -1 || opts->rumble_mode != -1) {
        if(!(f = open(MIYOO_VIR_FILE, O_RDWR))) {
            perror(ERR_OPEN_FILE(MIYOO_VIR_FILE));
            return EXIT_FAILURE;
            /* NOTREACHED */
        }
        if(opts->rumble_ver != -1) {
            if(opts->verbose) {
                fprintf(stdout, "%s: setting rumble motor version to %d\n", opts->progname, opts->rumble_ver);
            }
            ioctl(f, MIYOO_VIR_SET_VER, opts->rumble_ver);
        }
        if(opts->rumble_mode != -1) {
            if(opts->verbose) {
                fprintf(stdout, "%s: setting rumble motor mode to %d\n", opts->progname, opts->rumble_mode);
            }
            ioctl(f, MIYOO_VIR_SET_MODE, opts->rumble_mode);
        }
        close(f);
    }

    if(opts->just_want_info) {
        fprintf(stdout, "%s: current screen version: %d\n", opts->progname, current.screen_ver);
        fprintf(stdout, "%s: def_fb 0x%x; def_bp 0x%x; cur_fb 0x%x; cur_bp 0x%x\n", opts->progname,
                ((uint16_t)current.fpbp&0xF000)>>12,
                ((uint16_t)current.fpbp&0x0F00)>>8,
                ((uint16_t)current.fpbp&0x00F0)>>4,
                ((uint16_t)current.fpbp&0x000F));
        fprintf(stdout, "%s: current volume: %d\n", opts->progname, current.volume);
    }

    return EXIT_SUCCESS;
}
