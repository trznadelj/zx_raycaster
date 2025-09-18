#ifndef GFX_MEMMAP_H__
#define GFX_MEMMAP_H__

/* Configuration section */
#define DO_POPULATE_OTABLE
//#define DO_VLINE_PROTECTION
#define DO_PERF_TEST
#define DO_OPTIMIZED_RAYCASTING
//#define DO_LARGE_DIST
#define DO_LUT_TAN_MUL
//#define DO_LUT_CTAN_MUL
//#define DEBUG_LENGTH
//#define DO_SND
//#define DO_HIGH_CTAN_PRECISION
#define USE_STDLIB


#define FAST_CTAN_MUL_SEG        0xBD
#define FAST_CTAN_MUL_ADDR       0xBD00
#define fmul_ctan_table          ((uint8_t*) FAST_CTAN_MUL_ADDR)
#define _fmul_ctan_table         0xBD00



#define FAST_MUL_SEG        0xDF
#define FAST_MUL_ADDR       0xDF00
#define fmul_table          ((uint16_t*) FAST_MUL_ADDR)
#define _fmul_table         0xFD00

#define DTABLE_SEG          0xF0      /* F000...F7FF dtable */
#define DTABLE_ADDR         0xF000
#define d_table             ((uint8_t*) DTABLE_ADDR)
#define _d_table            DTABLE_ADDR


//uint8_t  d_table[4*128];  // depth table.

#define YTABLE_SEG          0xF8
#define YTABLE_ADDR         0xF800    /* F800...F97F ytable */
                                      /* FA80...FACF spare  */
#define YTABLE_REV_SEG      0xFA
#define YTABLE_REV_ADDR     0xFA80    /* FAD0...FAFF */
#define YTABLE_REV_OFFSET   0x80


#define ytable              ((uint8_t**) YTABLE_ADDR)
#define _ytable             YTABLE_ADDR

#define ytable_rev          ((uint8_t**) YTABLE_REV_ADDR)
#define _ytable_rev         YTABLE_REV_ADDR



#define YBUF_SEG            0xFB
#define YBUF_ADDR           0xFB00    /* FB00...FB1F Height of column visible */
#define YBUFC_ADDR          0xFB20    /* FB20...FB3F Colour of column visible */
#define YBUFI_ADDR          0xFB40    /* FB40...FB5F Address in bmap of column visible */
#define YBUFS_ADDR          0xFB60    /* Spare */
#define YBUFO_ADDR          0xFB80    /* FB80...FB9F Old height rendered for column. Used for adaptive redraw */
#define YBUFCO_ADDR         0xFBA0    /* FBA0...FBBF Old color rendered for column. Used for adaptive redraw */
#define CDIV_ADDR           0xFBC0    /* FBC0...FBDF Used for interpolation */
#define TMPBUF_ADDR         0xFBE0

/* Mapping for C code */
#define Ybuf                ((uint8_t*) YBUF_ADDR)
#define Ybuf_c              ((uint8_t*) YBUFC_ADDR)
#define Ybuf_i              ((uint8_t*) YBUFI_ADDR)
#define Ybuf_s              ((uint8_t*) YBUFS_ADDR)
#define Ybuf_o              ((uint8_t*) YBUFO_ADDR)
#define Ybuf_co             ((uint8_t*) YBUFCO_ADDR)
#define cdiv                ((uint8_t*) CDIV_ADDR)

/* Now lets be nice for assembly code */
#define _Ybuf   YBUF_ADDR
#define _Ybuf_i YBUFI_ADDR
#define _cdiv   CDIV_ADDR


#define _bufa   TMPBUF_ADDR

                                    /* 0xFBE0...0xFBFF empty     */
#define INTMAP_ADDR         0xFC00  /* 0xFC00...0xFD00 populated */
                                    /* 0xFD01...0xFDFC empty     */
#define INTMAP_ROUTINE_SEG  0xFD
#define INTMAP_ROUTINE_ADDR 0xFDFD  /* 0xFDFD...0xFDFF populated */

#define BMAP_ADDR           0xFF00  /* 0xFF00...0xFFFF populated */
#define BMAP_SEG            0xFF
#define bmap                ((uint8_t*) BMAP_ADDR)

#endif /* GFX_MEMMAP_H__ */
