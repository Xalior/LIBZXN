//
// Created by D. Rimron-Soutter on 13/05/2022.
//

#ifndef LIBZXN_ZXN_H
#define LIBZXN_ZXN_H

#include <arch/zxn.h>
#include <stdint.h>
#include <input.h>

/*
 * SYSVARs
 */

extern unsigned char _SYSVAR_LASTK;
extern unsigned char _SYSVAR_FLAGS;
//extern unsigned char _SYSVAR_FRAMES;

#define _SYSVAR_FRAMES          (*(uint16_t *)(0x5C78))
#define _SYSVAR_ATTR_P          (*(int8_t *)(0x5C8D))
#define _SYSVAR_RAMTOP          (*(uint16_t *)(0x5CB2))

/*
 * MACROs
 */

#define printInk(k)                 printf("\x10%c", '0'+k)
#define printPaper(k)               printf("\x11%c", '0'+k)
#define printFlash(k)               printf("\x12%c",30+k)
#define printFlashOn()              printf("\x12\x31")
#define printFlashOff()             printf("\x12\x30")
#define printBright(k)              printf("\x13%d", k)
#define printBrightOn()             printf("\x13""1")
#define printBrightOff()            printf("\x13""0")
#define printAt(row, col)           printf("\x16%c%c", col, row)
#define printAtStr(row, col, ...)   printf("\x16%c%c%s", col, row, __VA_ARGS__)

#define MACRO__WAIT_FOR_SCANLINE(line)  while (ZXN_READ_REG(REG_ACTIVE_VIDEO_LINE_L) == line); \
                                            while (ZXN_READ_REG(REG_ACTIVE_VIDEO_LINE_L) != line);

#define MACRO__WAIT_KEY                 while(in_inkey()==0){}; \
                                            while(in_inkey()!=0){};

#define MACRO__RESET_SOFT               ZXN_NEXTREG(2,1);

/*
 * Hardware and Registers
 */
#define LAYER_PRIORITIES_S_U_L 0x2
#define LAYER_PRIORITIES_UT_S_TU_BL 0x6
#define LAYER_PRIORITIES_UT_S_TU_BL5 0x7

#define LAYER_PRIORITIES_MASK  0x07
#define LAYER_PRIORITIES_SHIFT 2

/* (27) => Clip Window Tilemap          (R/W)
 * bits 7:0 = Clip window coordinate (inclusive)
 * 1st write = X1 position (soft reset = 0)
 * 2nd write = X2 position (soft reset = 159)
 * 3rd write = Y1 position (soft reset = 0)
 * 4rd write = Y2 position (soft reset = 255)
 * Reads do not advance the clip position
 * The X coordinates are internally doubled.
 */
#define REG_CLIP_WINDOW_TILEMAP         0x1B



#define REG_LAYER2_XSCROLL_LSB          0x16


/*
 * (47) => Tilemap X Scroll MSB         (R/W)
 * bits 7:2 = Reserved, must be 0
 * bits 1:0 = MSB X Offset (soft reset = 0)
 * Meaningful Range is 0-319 in 40 char mode, 0-639 in 80 char mode
 */
#define REG_TILEMAP_XSCROLL_MSB         0x2F

/*
 * (48) => Tilemap X Scroll LSB         (R/W)
 * bits 7:0 = LSB X Offset (soft reset = 0)
 * Meaningful range is 0-319 in 40 char mode, 0-639 in 80 char mode
 */
#define REG_TILEMAP_XSCROLL_LSB         0x30

/*
 * 0x4C (76) => Tilemap Transparency Index      (R/W)
 * bits 7-4 = Reserved, must be 0
 * bits 3-0 = Tilemap colour index treated as transparent (soft reset = 0xf)
 */
#define REG_TILEMAP_TRANSPARENCY_INDEX  0x4C

/*
 * (67) => Palette Control              (R/W)
 * bit 7 = Disable palette write auto-increment (soft reset = 0)
 * bits 6-4 = Select palette for reading or writing (soft reset = 000)
 *   000 = ULA first palette
 *   100 = ULA second palette
 *   001 = Layer 2 first palette
 *   101 = Layer 2 second palette
 *   010 = Sprites first palette
 *   110 = Sprites second palette
 *   011 = Tilemap first palette
 *   111 = Tilemap second palette
 * bit 3 = Select Sprites palette (0 = first palette, 1 = second palette) (soft reset = 0)
 * bit 2 = Select Layer 2 palette (0 = first palette, 1 = second palette) (soft reset = 0)
 * bit 1 = Select ULA palette (0 = first palette, 1 = second palette) (soft reset = 0)
 * bit 0 = Enabe ULANext mode (soft reset = 0)
 */
//#define REG_PALETTE_CONTROL             0x43
//#define  RPC_SELECT_ULA_PALETTE_0 	        0x00
#define  RPC_SELECT_LAYER2_PALETTE_0 	    0x10
#define  RPC_SELECT_SPRITE_PALETTE_0 	    0x20
#define  RPC_SELECT_TILEMAP_PALETTE_0 	    0x30
#define  RPC_SELECT_LAYER2_PALETTE_1 	    0x50
#define  RPC_SELECT_SPRITE_PALETTE_1 	    0x60
#define  RPC_SELECT_TILEMAP_PALETTE_1	    0x70
//#define  RPC_SELECT_ULA_PALETTE_1 	        0x80

/*
 * 0x68 (104) => ULA Control
 * (R/W)
 *   bit 7 = Disable ULA output (soft reset = 0)
 *   bits 6:5 = Blending in SLU modes 6 & 7 (soft reset = 0)
 *            = 00 for ula as blend colour
 *            = 10 for ula/tilemap mix result as blend colour
 *            = 11 for tilemap as blend colour
 *            = 01 for no blending
 *   bit 4 = Cancel entries in 8x5 matrix for extended keys
 *   bit 3 = ULA+ enable (soft reset = 0)
 *   bit 2 = ULA half pixel scroll (may change) (soft reset = 0)
 *   bit 1 = Reserved, must be 0
 *   bit 0 = Enable stencil mode when both the ULA and tilemap are enabled (soft reset = 0)
 *   (if either are transparent the result is transparent otherwise the result is a logical
 *      AND of both colours)
 */
#define REG_ULA_CONTROL                 0x68
#define  REG_ULA_PLUS_ENABLE                0x08

/* (108) => Default Tilemap Attribute   (R/W)
 * Active if nextreg 0x6B bit 5 is set
 * bits 7:4 = Palette offset (soft reset = 0)
 * bit 3 = X mirror (soft reset = 0)
 * bit 2 = Y mirror (soft reset = 0)
 * bit 1 = Rotate (soft reset = 0)
 * bit 0 = ULA over tilemap (soft reset = 0)
 *         (or bit 8 of the tile number if 512 tile mode is enabled)
 */
#define REG_TILEMAP_DEFAULT_ATTRIBUTE   0x6C
#define  RPC_TILEMAP_ATTRIBUTE_ULA_OVER  0x01
#define  RPC_TILEMAP_ATTRIBUTE_ROTATE    0x02
#define  RPC_TILEMAP_ATTRIBUTE_MIRROR_Y  0x04
#define  RPC_TILEMAP_ATTRIBUTE_MIRROR_X  0x08
#define TILEMAP_PALETTE_OFFSET_SHIFT    4

#define RPC_SELECT_TILEMAP_PALETTE_0 	0x30
#define RPC_SELECT_TILEMAP_PALETTE_1	0x70

#define SPRITE_SLOT_MASK_LSB 0x3F

#define REG_USER                        0x7f


#define LAYER_PRIORITIES_S_L_U 0x0
#define LAYER_PRIORITIES_L_S_U 0x1
#define LAYER_PRIORITIES_S_U_L 0x2
#define LAYER_PRIORITIES_L_U_S 0x3
#define LAYER_PRIORITIES_U_S_L 0x4
#define LAYER_PRIORITIES_U_L_S 0x5

#define LAYER_PRIORITIES_MASK  0x07
#define LAYER_PRIORITIES_SHIFT 2

#define LAYER2_SCREEN_TOP 0
#define LAYER2_SCREEN_MIDDLE 1
#define LAYER2_SCREEN_BOTTOM 2

#define LAYER2_FULLSCREEN_MASK 0x10
#define LAYER2_SHADOW_SCREEN_MASK 0x08
#define LAYER2_READ_ENABLE_MASK 0x04
#define LAYER2_VISIBLE_MASK 0x02
#define LAYER2_WRITE_ENABLE_MASK 0x01

#define LAYER2_SCREEN_SECTION_MASK 0x03
#define LAYER2_SCREEN_SECTION_SHIFT 6


#define SPRITE_PALETTE_OFFSET_SHIFT 4
#define SPRITE_SLOT_MASK_LSB 0x3F
#define SPRITE_EXTENDED_MASK 0x40
#define SPRITE_VISIBLE_MASK  0x80

#define SPRITE_ROTATE_MASK              0x02
#define SPRITE_MIRROR_Y_MASK            0x04
#define SPRITE_MIRROR_X_MASK            0x08

#define SPRITE_X_LSB_MASK               0x00FF
#define SPRITE_X_MSB_MASK               0x0100
#define SPRITE_X_MSB_SHIFT              8
#define SPRITE_X_LSB(x)       (uint8_t) ((x) & SPRITE_X_LSB_MASK)
#define SPRITE_X_MSB(x)       (uint8_t) (((x) & SPRITE_X_MSB_MASK) >> SPRITE_X_MSB_SHIFT)


#endif //LIBZXN_ZXN_H
