
/*
 * To build:
 * 	zcc +zx -lm -lndos -create-app gfx.c
 */

#include <graphics.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gfx_memmap.h"
#include "gfx_maps.c"
#include "gfx_io.c"
#include "div.c"
#include "gfx_raycast.c"
#include "gfx_math.c"



uint8_t  itable[8*64];    // interpolation table
uint16_t px=8*256,py=2*256-30;
uint8_t  pa;

#define highbyte(n) (((uint8_t*)&n)[1])
#define YADDR(y) ((((uint16_t) (( (y<<3)&56 ) | ( (y>>3)&7) | (y&192)))<<5)|16384)
#define YMIX(y)  (( (y<<3)&56 ) | ( (y>>3)&7) | (y&192))
//#define MIDDLE_LINE 91
#define MIDDLE_LINE 91
#define NUM_LEVELS 6
uint16_t ltimes[NUM_LEVELS]= {65535,65535,65535,65535,65535,65535}, btime;

#include "gfx_interp.c"
#include "gfx_intro.c"
#include "gfx_render.c"



void mplot( uint8_t x, uint8_t y)
{
    uint8_t *a = ytable[y] + (x>>3);
    *a |= (1<<((7^x)&7));
}

#ifdef USE_C_CALC_CDIV
void calc_cdiv()
{
    for( uint8_t x=0; x<31; x++)
    {
        uint8_t a=Ybuf[x]-Ybuf[x+1];
        if ((a<2)||(a>0xFE)) { /*printf("%c%c%c  ", 22, 32+23, 32+2*x); */cdiv[x]=2; continue; }

        a=Ybuf_i[x];
        uint8_t b=Ybuf_i[x+1];
        a=(((a&0Xf)-(b&0xF))&0xF) | (((a&0Xf0)-(b&0xF0))&0xF0);

        switch(a)
        {

            case 0x0 : b=0;   /*  printf("%c%c%c0 ", 22, 32+23, 32+2*x);  */ break;
            case 0x1 : b=0;   /*  printf("%c%c%c1 ", 22, 32+23, 32+2*x);  */ break;
            case 0xF : b=0;   /*  printf("%c%c%c1 ", 22, 32+23, 32+2*x);  */ break;
            case 0x10: b=0;   /*  printf("%c%c%c1 ", 22, 32+23, 32+2*x);  */ break;
            case 0xF0: b=0;   /*  printf("%c%c%c1 ", 22, 32+23, 32+2*x);  */ break;
            default: 
              if ((!(a&0xF))||(!(a&0xF0)))
              {
                     a=Ybuf_i[x];
                     b=Ybuf_i[x+1];
                     uint8_t c=((a&0Xf)+(b&0xF))>>1;  // calculate mean position between block.
                     c|=(((a>>4)+(b>>4))<<3)&0xF0;
                     if (!bmap[c]) b=2; else b=0;
              }
              else  b=2;  // not same line
        }

        cdiv[x] = b;
    }
    cdiv[31]=0;
}

#else
void calc_cdiv()
{
#asm
    ld ixl, 31
    ld hl, _Ybuf
    exx
    ld hl, _Ybuf_i
    exx

calc_cdiv_loop_start:
    ld a, (hl)  // b= Ybuf, c=Ybuf[+1]
    ld b, a
    inc hl
    ld a, (hl)
    sub b
    neg
    cp 2
    jr c, set_b2// calc_cdiv_loop_end
    cp 0xFF
    jr z, set_b2// calc_cdiv_loop_end // if (b-c<2) || (b-c>0xFE) continue
    exx
    ld a, (hl)
    inc hl
    exx
    ld c, a
    exx
    ld a, (hl)
    dec hl
    exx 
    ld b, a
    ld a, c
    and 0xF
    ld e, a
    ld a, b
    and 0xF
    sub e         
    and 0xF
    ld d, a       // d= (b&0xF - c&0xF)&0xF


    ld a, c
    and 0xF0
    ld e, a
    ld a, b
    and 0xF0
    sub e 
    and 0xF0       // a= (c&0xF0 - c&0xF0)&0xF0
    or d           // a =  (((a&0Xf)-(b&0xF))&0xF) | (((a&0Xf0)-(b&0xF0))&0xF0);
    jr z, set_b0
    cp 1
    jr z, set_b0
    cp 0xF
    jr z, set_b0
    cp 0x10
    jr z, set_b0
    cp 0xF0
    jr z, set_b0

    ld d, a 
    and 0xF
    jr z, check_middle_point
    ld a, d
    and 0xF0
    jr nz, set_b2

check_middle_point:
    exx
    ld a,(hl)
    exx
    ld b, a
    exx
    inc hl
    ld a,(hl)
    dec hl
    exx
    ld c,a // b,c
    and 0xF
    ld d, a
    ld a, b
    and 0xF
    add d                // b=Ybuf_i[]; c=Ybuf_i[+1]; 
    rra
    and 0xF              // a= ((b+c)>>1)&0xF

    push hl
    ld h,0xFF
    ld l, a
    rr b
    rr c
    ld a,c
    and 0x78

    ld c,a
    ld a,b 
    and 0x78
    add c
    and 0xF0
    or l
    ld l,a
    ld a,(hl)
    pop hl
    or a
    jr nz, set_b0 

set_b2:
    ex de, hl
    ld hl, _cdiv
    ld b, 0
    ld a, 31
    sub ixl
    ld c, a
    add hl, bc
    ld (hl), 2
    ex de, hl
    jp calc_cdiv_loop_end
set_b0:
    ex de, hl
    ld hl, _cdiv
    ld b, 0
    ld a, 31
    sub ixl
    ld c, a
    add hl, bc
    ld (hl), 0
    ex de, hl

calc_cdiv_loop_end:

    exx
    inc hl
    exx
    dec ixl
    jp nz, calc_cdiv_loop_start
    ld hl, _cdiv+31
    ld (hl), 0
#endasm
}

#endif

void calc_cdiv_update()
{
#asm
    ld ixl, 0
    ld b,0
calc_cdiv_update_loop:
    ld hl, _Ybuf
    ld c, ixl
    add hl, bc
    ld a, (hl) //a=Ybuf
    inc hl
    ld d, (hl) //d=Ybuf+1
    ld c, 31
    add hl, bc
    ld e, (hl)  //e=Ybuf_c
    ld c, 96
    add hl, bc // hl->Ybuf_o
    cp (hl)    // Ybuf_o[x] == Ybuf[x]
    jr nz,calc_cdiv_update_loop_end
    ld a, d
    inc hl     // hl->Ybuf_o+1
    cp (hl)    // Ybuf_o[x+1] == Ybuf[x+1]
    jr nz,calc_cdiv_update_loop_end
    ld c, 31
    add hl, bc // hl->Ybuf_co
    ld a, e
    cp (hl)   // Ybuf_co[x] == Ybuf_c[x]
    jr nz,calc_cdiv_update_loop_end
    ld c, 32
    add hl, bc
    inc (hl) // hl->cdiv

calc_cdiv_update_loop_end:
    inc ixl
    ld a, ixl
    cp 31
    jr nz, calc_cdiv_update_loop
#endasm
}

#define MIDDLE_LINE_INTERP (96+4)
void bwrender3( )  
{
    calc_cdiv();
    calc_cdiv_update();

    for( uint8_t x=0; x<31; x++)
    {
        uint16_t y0, y1;
        if (cdiv[x]&1) // no update
           continue; 


        vline_x = x;
        vline_d = vline_db= Ybuf[x];
        vline_c = Ybuf_c[x];
        vline3();

        if (cdiv[x]&2)   // plain mode - no interpolation
        {
#if 1
            if ( (x)&&((cdiv[x-1]&2)==0)&&(Ybuf_i[x-1]==Ybuf_i[x])&&(Ybuf_i[x+1]!=Ybuf_i[x]) ) // if previous had gradient & and the same as me
            {
                y0=Ybuf[x];
                y1=Ybuf[x]+Ybuf[x]-Ybuf[x-1];
                if (Ybuf[x+1]>y1) y1=Ybuf[x+1];
                if ((y0-y1<2) || (y1-y0<2)) continue;
                goto continue_please;
            }
#endif
            continue; 
        }

        y0=Ybuf[x];
        y1=Ybuf[1+x];
continue_please:
        interp_c = Ybuf_c[x];


        if (y1>y0+1)
        {
           interp_xx=0; interp_r =0;   interp_x = x;        interp_y0 = MIDDLE_LINE_INTERP+y0;        interp_y1 = MIDDLE_LINE_INTERP+y1;        interp();    
           interp_c ^=56;
                        interp_r =0xFF;                     interp_y0 = MIDDLE_LINE_INTERP-y1;        interp_y1 = MIDDLE_LINE_INTERP-y0;        interp();
        }
        else if (y0>y1+1)
        {
           interp_xx=0xff; interp_r =0xff;   interp_x = x;        interp_y0 = MIDDLE_LINE_INTERP+y1;        interp_y1 = MIDDLE_LINE_INTERP+y0;        interp();    
           interp_c ^=56;
                           interp_r =0x0;                         interp_y0 = MIDDLE_LINE_INTERP-y0;        interp_y1 = MIDDLE_LINE_INTERP-y1;        interp();
        } 
    }
    vline_x = 31;
    vline_d = vline_db= Ybuf[31];
    vline_c = Ybuf_c[31];
    vline3();
}

void maprender()
{
    uint8_t* attr = 16384+2048+4096;

    for( uint8_t x=0; x<16; x++)
        for( uint8_t y=0; y<16; y++)
        {
            uint8_t a = bmap[x+y*16];
            if (a) a|=a<<3; else a=56;
            attr[ x + 32*y+64 ] = a;
        }
    uint8_t mx = (px>>5)&127;
    uint8_t my = ((py>>5)&127)+16;

    mplot( mx, my );
}

void snd()
{
#asm
    ld a, (_fast_tick)
    inc a
    ld (_fast_tick), a
    rla
    rla
    rla
    rla
    and 0x10
    ld b, a
    ld a,(_border)
    or b
    ld bc,0xFE
    out (c),a
#endasm
}


void game_end()
{
    printf("%c%c%c Ending screen of this demo. Technical details.\n\n", 22, 32+2, 32);
    printf("Raycasting is fully hand optimized. \nIt uses 8x8 map to store position in one byte.\n");
    printf("Player location is stored in 16 bit register, \nof which oldest 4 bits are not used\n");
    printf("There are 2 multiplications per ray. \nThey take 26 %% of game time \n");
    printf("Raycasting is taking 41 %% of game time. \n (plus 26 %% spent in multiplications) \n");
    printf("Rendering (fast) takes 23 %% of time.        \n\n");
    printf("For 32fps,32 rays = 1024 rays/second. \n");
    printf("  Roughly 3500 z80 cycles needed to draw one ray.\n  It is less than 1000 z80 instructions\n");
    printf("         Press U to return to start\n");
    while( !(get_qweasdu()&0x64) );
}


uint8_t old_player_k;
int16_t player_vx=0, player_vy=5;
int8_t  player_ax, player_ay, player_rotation_v;
int16_t tick=0;
// game parameters
uint8_t render_mode=0;
uint8_t game_level=0;
uint8_t game_counter=0;
uint8_t fast_tick, border;
uint8_t r2;

void level_end()
{
    btime = get_clock()-btime; 
    if (btime<ltimes[game_level]) ltimes[game_level]=btime;
    btime = get_clock();
    player_vx=player_ax=player_vy=player_ay=0; px=8*256-30,py=2*256-30; game_level++; player_rotation_v=12;
    if (game_level>4) game_level=0;
    switch(game_level)
    {
        case 4: memcpy( bmap, bmap5, 256); pa = 192; break;
        case 3: memcpy( bmap, bmap4, 256); pa = 192; break;
        case 2: memcpy( bmap, bmap3, 256); pa = 192; break;
        case 1: memcpy( bmap, bmap1, 256); pa = 192; break;
        case 0: memcpy( bmap, bmap2, 256); break;
    }
    if (game_level)
        printf("%c%c%c Congrats! you have achieved level%c!                     ", 22, 32+23, 32, game_level+'1');
    else
    {
        switch( game_counter++ )
        {
            default: game_counter=0;
            case 0: printf("%c%c%c End of levels! PSST! use U to change rendering mode. ", 22, 32+23, 32); break;
            case 1: printf("%c%c%c Do you know that pressing Y turns speed boost?!      ", 22, 32+23, 32); break;
            case 2: printf("%c%c%c Did you know that robot is afraid of speed boost?    ", 22, 32+23, 32); break;
            case 3: printf("%c%c%c Pressing Y+U simultaneously will start perf test     ", 22, 32+23, 32); break;
            case 4: printf("%c%c%c I am impressed! You are still playing it!            ", 22, 32+23, 32); break;
            case 5: game_end(); break;
        }
    }

    printf("%c%c%c Level times: %u %u %u %u %u %u       ", 22, 32, 32, ltimes[0], ltimes[1], ltimes[2], ltimes[3], ltimes[4], ltimes[5]);
}

void render_mode_change()
{
    render_mode++; render_mode%=4;
    memset( 16384+2048+4096, 56, 768 ); 
    printf("%c%c%c Renderer mode: %c-", 22, 32+23, 32, render_mode+'0'); 
    switch( render_mode )
    {
        case 0: printf("fast renderer     "); break;
        case 1: printf("enemies enabled   "); break;
        case 2: printf("precise renderer  "); break;
        case 3: printf("map renderer      "); break;
    }
    for( uint8_t y = 16; y<176; y++)
        memset( (void*) (ytable[y]), 0, 32 );

    /* Enforce screen refresh */
    memset( Ybuf_o, 0, 32);
    memset( Ybuf_co, 0xFC, 32);
}


void player()
{
    uint8_t k;

    k = get_qweasdu();
    player_ax=player_ay=r2=0;

#asm
//    if (k&2)  { player_ay=-sin_table[(pa+64)&255]>>3; player_ax = sin_table[pa]>>3;  }
//    if (k&16) { player_ay= sin_table[(pa+64)&255]>>3; player_ax =-sin_table[pa]>>3;  }
//    if (k&8)  { player_rotation_v-=3;  /*px-=sin_table[pa]/4; */ }
//    if (k&32) { player_rotation_v+=3;  /*px+=sin_table[pa]/4; */ }
//    if (k&1)  { player_ax=-sin_table[(pa+64)&255]>>3; player_ay=-sin_table[pa]>>3;  }
//    if (k&4)  { player_ax= sin_table[(pa+64)&255]>>3; player_ay= sin_table[pa]>>3;  }


    ld a, (_pa)
    ld hl, _sin_table
    ld b, 0
    ld c, a
    add hl, bc
    add 64
    ld c, a
    ld a, (hl)
    sra a
    sra a
    sra a
    ld d, a
    ld hl, _sin_table
    add hl, bc
    ld a, (hl)
    sra a
    sra a
    sra a
    ld e, a     // e = sin_table[pa+64];

    ld a, (_get_qweasdu_result)
    ld b, a
    and 2
    jr z,kskip1
    ld a, e
    neg
    ld (_player_ay), a
    ld a, d
    ld (_player_ax), a
kskip1: 
    ld a, b
    and 16
    jr z,kskip2
    ld a, e    
    ld (_player_ay), a
    ld a, d
    neg
    ld (_player_ax), a   
kskip2:
    ld a, b
    and 1
    jr z,kskip3
    ld a, e    
    neg
    ld (_player_ax), a
    ld a, d
    neg
    ld (_player_ay), a
kskip3:
    ld a, b
    and 4
    jr z,kskip4
    ld a, e    
    ld (_player_ax), a
    ld a, d
    ld (_player_ay), a  
kskip4:
    ld a, b
    and 8
    jr z, kskip5
    ld a, (_player_rotation_v)
    sub 3
    ld (_player_rotation_v), a
kskip5:
    ld a, b
    and 32
    jr z, kskip6
    ld a, (_player_rotation_v)
    add 3
    ld (_player_rotation_v), a
kskip6:
#endasm


    if ((k&64) && (!(old_player_k&64))) render_mode_change();

    if (k&128) { // fire => turbo mode!
        snd();
        player_vx+=player_ax>>1;        player_vy+=player_ay>>1; 
        player_vx-=player_vx>>4;        player_vy-=player_vy>>4;
        if (!(tick&3))
        {
            if (player_vx>0) player_vx--; else if (player_vx<0) player_vx++;
            if (player_vy>0) player_vy--; else if (player_vy<0) player_vy++;
        }
    }
    else
    {
#asm


    ld   hl, (_player_vx)      // HL = vx
    ld   a, (_player_ax)       // A = ax (8-bit, signed)
    // make 16-bit signed (ax >> 1) in DE — cheaper to shift A first
    sra  a                     // A = ax >> 1 (arith)
    ld   e, a                  // E = low byte
    add  a, a                  // carry = sign of (ax>>1)
    sbc  a, a                  // A = 0x00 or 0xFF
    ld   d, a                  // D = sign-extend
    add  hl, de                // HL = vx + (ax>>1)
    // friction: move 1 toward zero
    ld   a, h
    or   l
    jr   z, StoreVX            // if zero, nothing to do
    bit  7, h
    jr   nz, NegVX             // negative -> ++
    dec  hl                    // positive -> --
    jr   StoreVX
NegVX:
    inc  hl
StoreVX:
    ld d, h
    ld e, l
    sra d
    rr e
    sra d
    rr e
    sra d
    rr e
    xor a
    sbc  hl, de 
    ld   (_player_vx), hl

    ld   hl, (_player_vy)      // HL = vx
    ld   a, (_player_ay)       // A = ax (8-bit, signed)
    // make 16-bit signed (ax >> 1) in DE — cheaper to shift A first
    sra  a                     // A = ax >> 1 (arith)
    ld   e, a                  // E = low byte
    add  a, a                  // carry = sign of (ax>>1)
    sbc  a, a                  // A = 0x00 or 0xFF
    ld   d, a                  // D = sign-extend
    add  hl, de                // HL = vx + (ax>>1)
    // friction: move 1 toward zero
    ld   a, h
    or   l
    jr   z, StoreVY            // if zero, nothing to do
    bit  7, h
    jr   nz, NegVY             // negative -> ++
    dec  hl                    // positive -> --
    jr   StoreVY
NegVY:
    inc  hl
StoreVY:
    ld d, h
    ld e, l
    sra d
    rr e
    sra d
    rr e
    sra d
    rr e
    xor a
    sbc  hl, de 
    ld   (_player_vy), hl

#endasm

        
//        player_vx-=player_vx>>3;         player_vx+=player_ax>>1; if (player_vx>0) player_vx--; else if (player_vx<0) player_vx++;
//        player_vy-=player_vy>>3;        player_vy+=player_ay>>1;  
  //     if (player_vy>0) player_vy--; else if (player_vy<0) player_vy++;
    }
    old_player_k=k;
#if 1
#asm
//    px+=player_vx>>2; r=bmap[ ((highbyte(py)&15)<<4)|(highbyte(px)&15) ]; if ((r)&&(r!=0xFF)) {px-=player_vx; player_vx=-player_vx>>1; r2=r;  }
    ld de, (_player_vx)
    sra d
    rr e
    sra d
    rr e
    ld hl, (_px)
    add hl, de
    ld b, h
    ld c, l

    ld a,h
    and 0x0F
    ld l, a

    ld a, (_py+1)
    rlca 
    rlca
    rlca
    rlca
    and 0xF0
    or l
    ld l, a
    ld h, BMAP_SEG
    ld a, (hl)
    or a
    jz vx_move_ok
    cp 0xFF
    jz vx_move_ok

    ld (_r2), a

    ld de, (_player_vx)
    sra d
    rr e
    ld hl, 0
    xor a
    sbc hl, de
    ld (_player_vx), hl

    jp after_vx
vx_move_ok:
    ld (_px), bc
after_vx:

//    py+=player_vy>>2; r=bmap[ ((highbyte(py)&15)<<4)|(highbyte(px)&15) ]; if ((r)&&(r!=0xFF)) {py-=player_vy; player_vy=-player_vy>>1; r2=r;  }
    ld de, (_player_vy)
    sra d
    rr e
    sra d
    rr e
    ld hl, (_py)
    add hl, de
    ld b, h
    ld c, l

    ld a,h
    rlca 
    rlca
    rlca
    rlca
    and 0xF0
    ld l, a

    ld a, (_px+1)
    and 0xF
    or l
    ld l, a
    ld h, BMAP_SEG
    ld a, (hl)
    or a
    jz vy_move_ok
    cp 0xFF
    jz vy_move_ok

    ld (_r2), a

    ld de, (_player_vy)
    sra d
    rr e
    ld hl, 0
    xor a
    sbc hl, de
    ld (_player_vy), hl

    jp after_vy
vy_move_ok:
    ld (_py), bc
after_vy:

#endasm
#endif

    #define MAX_SPEED 15

#asm

 //   pa+=player_rotation_v>>1;
 // if (player_rotation_v>0) { player_rotation_v --; if (player_rotation_v) player_rotation_v--; if (player_rotation_v>MAX_SPEED) player_rotation_v = MAX_SPEED; }
 //   if (player_rotation_v<0) { player_rotation_v ++; if (player_rotation_v) player_rotation_v++; if (player_rotation_v<-MAX_SPEED) player_rotation_v= -MAX_SPEED; }

    ld   a,(_player_rotation_v)    
    ld   c, a
    sra  a                         
    ld   b,a                       
    ld   a,(_pa)                   
    add  a,b                       
    ld   (_pa),a                   

    ld   a, c
    or   a                         
    jp   z,done   
    jp   m,negative                
// positive case
    dec a
    jr z, done
    dec a
    cp MAX_SPEED
    jr c, done
    ld a, MAX_SPEED
    jp done
negative:
    inc a
    jr z, done
    inc a
    neg
    cp MAX_SPEED
    jr c, pre_done
    ld a, MAX_SPEED
pre_done:
    neg
done:
    ld (_player_rotation_v), a

#endasm

    if (r2==0x48)
    {
         level_end();
    } else { 
         border=( r2?r2: (k&128?tick:7) );  
         zx_border(border);
    }

}


char* perf_test_helper( char* s, uint16_t x, uint16_t d)
{
    static char buf[32];
    uint32_t fps = 256*5000; fps/=x;

    printf("  %s %u  FPS: %u.%2u Delta: %u\n", s, x,  (uint16_t)(fps/100), (uint16_t)(fps%100), x-d);
    return buf;
}

#if 0
void wait_clock()
{
    uint8_t t = get_clock();
    while( t==get_clock() );
    t = get_clock();
    while( t==get_clock() );
    printf("%c%c%c %u.\n", 22, 32+21, 32, t);
}
#endif

#ifdef DO_PERF_FEST
void perf_test()
{
    uint16_t start, end;
    start = get_clock();

    uint16_t pegs[8];

    printf("%c%c%c Doing performance test. Stay tuned.\n", 22, 32+22, 32);

    for( interp_c=1; interp_c<8; interp_c++)
    {
        interp_xx=0xFF; interp_r =0;  
        for( uint8_t i=0; i<32; i++) {        interp_x = i;        interp_y0 = 4;        interp_y1 = 5+i;        interp();    }

        interp_xx=0;
        for( uint8_t i=0; i<32; i++) {        interp_x = i;        interp_y0 = 4+32;     interp_y1 = 5+i+32;     interp();    }

        interp_xx=0; interp_r =0xFF;
        for( uint8_t i=0; i<32; i++) {        interp_x = i;        interp_y0 = 4+64;     interp_y1 = 5+i+64;        interp();   }

        interp_xx=0xFF;
        for( uint8_t i=0; i<32; i++) {        interp_x = i;        interp_y0 = 4+92;     interp_y1 = 5+i+16+92;     interp();    }
    }
    pegs[0] = get_clock()-start;

    start = get_clock();
    for( pa=0; pa<255; pa++) {
        raycast2(); 
    }
    pegs[1] = get_clock()-start;

    start = get_clock();
    for( pa=0; pa<255; pa++) {
        raycast2(); 
        calc_cdiv();
        calc_cdiv_update();

    }
    pegs[2] = get_clock()-start;


    start = get_clock();
    for( pa=0; pa<255; pa++) {
        raycast2(); renderv();
    }
    pegs[3] = get_clock()-start;

    start = get_clock();
    for( pa=0; pa<255; pa++) {
        player(); raycast2(); renderv();
    }
    pegs[4] = get_clock()-start;


    start = get_clock();
    for( pa=0; pa<255; pa++) {
        raycast2(); bwrender3();
    }
    pegs[5] = get_clock()-start;

    clg(); 
    printf("%c%c%cPerformance test result: \n", 22, 32, 32);
    printf("  Triangles:      %d \n", pegs[0]);
    perf_test_helper("Raycast:       ",pegs[1],0);
    perf_test_helper("Raycast+cdiv:  ",pegs[2],pegs[1]);
    perf_test_helper("RenderV:       ",pegs[3],pegs[1]);
    perf_test_helper("RenderV+player:",pegs[4],pegs[3]);
    perf_test_helper("BWrender3:     ",pegs[5],pegs[2]);
    printf("Compilation flags:\n");
#ifdef DO_POPULATE_OTABLE
    printf("  DO_POPULATE_OTABLE\n");
#endif
#ifdef DEBUG_LENGTH
    printf("  DEBUG_LENGTH\n");
#endif
//#ifdef USE_STACK
//    printf("  USE_STACK\n");
//#endif

    printf("  Build time: " __DATE__ " "__TIME__ "\n");

    printf("Press control key (QWEASDU) to start game");

    while( !get_qweasdu() );
}
#endif


void putline( uint8_t px, uint8_t py, uint8_t l)
{
    uint8_t pt1 = 0xFF>>(px&7);
    uint8_t pt2 = 0xFF<<(px&7), c, y=py&0xF8;
    uint16_t addr ;
    uint8_t dl = (((l+px)&0xF8)-(px&0xF8)+7)>>3;
//if (dl>4) dl=4;
    for( uint8_t i=0; i<8; i++, y++)
    {
        addr = ytable[y]+(px>>3);
        for( uint8_t j=0; j<=dl; j++, addr++)
            *(unsigned char*)(addr) = 0;
    }
    addr = ytable[py]+(px>>3);
    c = 0xFF>>(px&7);
    if (l<7-(px&7)) c&=(~1<<( 7-(px&7)-l ));
    *(unsigned char*)(addr) = c;      addr++;
    if (dl)
    for( uint8_t j=0; j<dl-1; j++, addr++)
        *(unsigned char*)(addr) = 0xFF;
    if (dl)
        *(unsigned char*)(addr) = 0xFF<<( 7-((l+px)&7) );
    addr = py&0xF8; addr<<=2;
    addr|=0x5800|(px>>3);
    for( uint8_t i=0; i<=dl; i++, addr++)
    {
    c = *((unsigned char*) addr);
    c&=0xF8; c|=0x46;
    *((unsigned char*) addr) = c;
    }
}

void putline_ncu( uint8_t px, uint8_t py, uint8_t l, uint8_t last)
{
    uint8_t pt1 = 0xFF>>(px&7);
    uint8_t pt2 = 0xFF<<(px&7), c, y=py&0xF8;
    uint16_t addr ;
    uint8_t dl = (((l+px)&0xF8)-(px&0xF8))>>3;
    if (last)
    for( uint8_t i=0; i<8; i++, y++)
    {
        if (y<py) continue;
        addr = ytable[y]+(px>>3);
        for( uint8_t j=0; j<=dl; j++, addr++)
            *(unsigned char*)(addr) = 0;
    }
    addr = ytable[py]+(px>>3);
    c = 0xFF>>(px&7);
    if (l<7-(px&7)) c&=(~1<<( 7-(px&7)-l ));
    *(unsigned char*)(addr) = c;      addr++;
    if (dl)
    for( uint8_t j=0; j<dl-1; j++, addr++)
        *(unsigned char*)(addr) = 0xFF;
    if (dl)
        *(unsigned char*)(addr) = 0xFF<<( 7-((l+px)&7) );
    addr = py&0xF8; addr<<=2;
    addr|=0x5800|(px>>3);
    for( uint8_t i=0; i<=dl; i++, addr++)
    {
    c = *((unsigned char*) addr);
    c&=0xF8; c|=0x46;
    *((unsigned char*) addr) = c;
    }
}


void sprite( uint8_t px, uint8_t py, uint8_t s)
{

}


uint16_t sprite_x=0x800, sprite_y=0x800;
uint16_t fireball_x=0x800, fireball_y=0x800, fireball_vx=50, fireball_vy=0;

void playfield()
{
    uint16_t dpx = px&0xFFF;
    uint16_t dpy = py&0xFFF;
    int8_t dir = get_qweasdu_result&128?-1:1;
    for( uint8_t n=0; n<8; n++)
    {
        uint16_t nsprite_x = sprite_x;
        uint16_t nsprite_y = sprite_y;
        uint8_t mpos, opos;
        opos = ((nsprite_x>>8)) | ((nsprite_y>>8)<<4);
        if (sprite_x<dpx) { nsprite_x+=dir; } else if (sprite_x>dpx) { nsprite_x-=dir; } nsprite_x&=0xFFF;
        mpos = ((nsprite_x>>8)) | ((nsprite_y>>8)<<4);
        if (opos==mpos) sprite_x = nsprite_x; else
            if (!bmap[mpos]) { bmap[mpos]=0xFF; bmap[opos]=0; opos=mpos; sprite_x = nsprite_x; }
                else nsprite_x = sprite_x;

        if (sprite_y<dpy) { nsprite_y+=dir; } else if (sprite_y>dpy) { nsprite_y-=dir; } nsprite_y&=0xFFF;
        mpos = ((nsprite_x>>8)) | ((nsprite_y>>8)<<4);
        if (opos==mpos) sprite_y = nsprite_y; else
            if (!bmap[mpos]) { bmap[mpos]=0xFF; bmap[opos]=0; sprite_y = nsprite_y; }
                else nsprite_y = sprite_y;
    }
    fireball_x+=fireball_vx; fireball_x &= 0xFFF;
}

void dump_debug()
{
printf("%c%c%c O_ LEN XL _XH CTG AN  DT TAN D_H LAN L__	\n",22,32+5,32);
   for( uint16_t x = 0; x<16; x++)
       printf(" %2.2u %3.3u  %u %3.3u %3.3u %2.2u %3.3u %3.3u %3.3u %3.3u %3.3u\n",  
             *(uint8_t*) (0xFB00+x),
             *(uint8_t*) (0xF800+x), 
             *(uint8_t*) (0xF820+x),  
             *(uint8_t*) (0xF840+x),  
             *(uint8_t*) (0xF860+x), 
                                     *(uint8_t*) (0xF880+x),  *(uint8_t*) (0xF8A0+x), *(uint8_t*) (0xF8C0+x),  *(uint8_t*) (0xF8E0+x),  *(uint8_t*) (0xF900+x),  *(uint8_t*) (0xF920+x) );
printf("IXH=>  -L__");
   while( !(get_qweasdu()&0x64) );
}

void game_init()
{
    uint8_t i,j;

    /* Maps distance to column heigth.  column_height = d_table[ distance>>5 ]; */
    p = d_table;
    for( uint16_t d = 0; d<512; d++) 
    {
        uint16_t n = 65535;
        n/=d; n>>=5;
        if (n<1) n=1;
        if (n>80) n=80;
        *(p++) = n;
    }

    /* Create Yable lookup */
    for( i=0; i<192; i++)
        ytable[i]=(unsigned char*) (YADDR(i));
  
    for( i=0,j=0; i<192; i+=8,j++)
        ytable_rev[j]=(unsigned char*) (YADDR(i));


    /* Itable - for interpolation, rendering of steep triangles */
    for( i=2; i<64; i++)
    {
        uint8_t py=0;
        for( y=0; y<8; y++)
        {
            uint16_t yy=y; yy*=i; yy/=7;    //2. 0 2 4 6 8 10 12 14 -> 0 0 0 0 1 1 1 2
                                            //3. 0 3 6 9 12 15 18 21 -> 0 0 0 1 1 2 2 3
            itable[i*8+y] = yy - py;
            py = yy;
        }
    }
    memcpy( bmap, bmap2, 256);
    set_interrupt();
    player_rotation_v=-15;
    player_vy = 500;
}


uint16_t speed;
uint8_t old_speed;
void hud_display()
{
#ifdef DO_SND
    snd();
#endif
    static uint8_t x; if ((++x)&3) return;
    if (player_vx>0) speed += player_vx; else speed+=-player_vx;
    if (player_vy>0) speed += player_vy; else speed+=-player_vy;
//    speed = player_vx;
//    if (speed<0) speed =- speed;
    speed>>=2;
//  printf("%c%c%c Fast %u    ", 22, 32+23, 32, player_vx);
    speed-=speed>>3;
    if (speed>29) speed=29;
    if (speed!=old_speed)
    {
#asm
    ld a, (_speed)
    ld hl, 0x5823
    ld b, a
    ld c, a
    or a
    jz skip_loop0
    ld a, 0x3A
repeat_color:
    ld (hl), a
    inc hl
    djnz repeat_color
skip_loop0:
    ld a, c
    sub 29
    neg
    ld b, a
    or a
    jz skip_loop1
    ld A, 0x3F
repeat_color2:
    ld (hl), a
    inc hl
    djnz repeat_color2
skip_loop1:
#endasm
    }
#ifdef DO_SND
    snd();
#endif
}

main()
{
    char update = 1;

    game_init();
    tick=1 ;

    clg();   

#ifdef DO_PERF_FEST_AT_STARTUP
    perf_test();
#endif

    uint16_t start, end;
    printf("Find the black box.             Movement: WSAD + QE\n");
    printf("Speed|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||");
    printf("%c%c%cBLACK BOX IS A FINAL DESTINATION! BLACK BOX IS A FINAL DESTINATION!", 22, 32+12, 32);
    printf("%c%c%c Fast raycasting engine tech demo.   C2025 JTKrakow ", 22, 32+23, 32);
    start = btime = get_clock();

    while(1)
    {
        player();
        if (player_vx||player_vy||player_rotation_v) update=1;

        else if ((render_mode==1)||(render_mode==2)) update=1;
//update=1; pa++;
        if (update) {
          
           switch (render_mode)
           {
               default:render_mode = 0;

               case 0: 
                    raycast2(); 
#ifdef DO_SND
                    if (get_qweasdu_result&128) snd();
#endif
                    renderv(); hud_display(); 
               break;

               case 1:
                   playfield();
                   raycast2(); 
                   if (o_index) renderv_full(); else renderv();     
#if 0
                   for ( uint8_t n=0; n<o_index; n+=2) 
                   {
                       Ybuf_o[ o_table[n] ] = 0;
                       printf("%c%c%c++", 22, 32+12, 32+2*o_table[n]);
                   }
#endif             
                   hud_display();
                   if (o_index)
                   {
                       int16_t dpx = px&0xFFF;
                       int16_t dpy = py&0xFFF;
                       dpx-=sprite_x; dpy-=sprite_y;
                       atan_x = dpx;  atan_y = dpy;
                       uint8_t angle= int_atan2();
                       angle = -angle-pa+32;     
                       if (angle<64)
                       {
                          for ( uint8_t n=0; n<o_index; n+=2)
                              if ((angle&0xFE)==(o_table[n]<<1)) {          
                                  uint8_t height = d_table[ div16y>>3 ];
                                  uint8_t h12 = height>>3,  h2=height>>1;
                                  uint8_t x = (angle<<2) | (3-(atan_raw&3));
                                  putline( x, 100, h2);
                                  putline( x+(height>>3), 96+h2, height>>2);
                                  putline( x, 96+height, h2);
                                  height+=96;
                                  for( uint8_t opos=0; opos<h12; opos++, height++)
                                      if (height<184)putline_ncu( x-(opos>>1), height, h2+opos, 0);
                                  if (height<184)
                                  for( uint8_t opos=h12; opos>0; opos--, height++)
                                      if (height<184) putline_ncu( x-(opos>>1), height, h2+opos, 0);
                              }
                           for ( uint8_t n=0; n<o_index; n+=2)
                               Ybuf_co[ o_table[n] ] = 0xFE;
                       }
                   }
                   
              break;
//              
              case 2: 
                   playfield();
                   raycast2(); bwrender3();     hud_display();
#if 1
                   if (o_index)
                   {
                       int16_t dpx = px&0xFFF;
                       int16_t dpy = py&0xFFF;
                       dpx-=sprite_x; dpy-=sprite_y;
                       atan_x = dpx;  atan_y = dpy;
                       uint8_t angle= int_atan2();;

                       angle = -angle-pa+32;     
                       if (angle<64)
                       {
                          for ( uint8_t n=0; n<o_index; n+=2)
                              if ((angle&0xFE)==(o_table[n]<<1)) {          
                                  uint8_t height = d_table[ div16y>>3 ];
                                  uint8_t h12 = height>>3,  h2=height>>1;
                                  uint8_t x = (angle<<2) | (3-(atan_raw&3));
                                  putline( x, 100, h2);
                                  putline( x+(height>>3), 96+h2, height>>2);
                                  putline( x, 96+height, h2);
                                  height+=96;
                                  for( uint8_t opos=0; opos<h12; opos++, height++)
                                      if (height<184)putline_ncu( x-(opos>>1), height, h2+opos, 0);
                                  if (height<184)
                                  for( uint8_t opos=h12; opos>0; opos--, height++)
                                      if (height<184) putline_ncu( x-(opos>>1), height, h2+opos, 0);
                                  break;
                              }
                       }
                   }
#endif
                   if (o_index)
                   {
                       int16_t dpx = px&0xFFF;
                       int16_t dpy = py&0xFFF;
                       dpx-=fireball_x; dpy-=fireball_y;
                       atan_x = dpx;  atan_y = dpy;
                       uint8_t angle= int_atan2();;

                       angle = -angle-pa+32;     
                       if (angle<64)
                       {
                          for ( uint8_t n=0; n<o_index; n+=2)
                              if ((angle&0xFE)==(o_table[n]<<1)) {          
                                  uint8_t height = d_table[ div16y>>3 ];
                                  uint8_t h12 = height>>3,  h2=height>>1;
                                  uint8_t x = (angle<<2) | (3-(atan_raw&3));
                                  height=100;
                                  putline( x, height, h2);
                                  for( uint8_t opos=0; opos<h12; opos++, height++)
                                      if (height<184)putline_ncu( x-(opos>>1), height, h2+opos, 0);
                                  if (height<184)
                                  for( uint8_t opos=h12; opos>0; opos--, height++)
                                      if (height<184) putline_ncu( x-(opos>>1), height, h2+opos, 0);
                                  break;
                              }
                       }
                   }
              break;



              case 3:
                  playfield(); maprender(); hud_display();
              break;
           }
           update = 0;

           if (tick++%100==0) {
               end = get_clock();
//               printf("%c%c%c %u %u.%u  fps?", 22, 32+1-1, 32, tt, (end-start)/100, (end-start)%100);
               uint16_t fps = 50000/(end-start); start=end;
               printf("%c%c%c FPS:%u.%u ", 22, 32+23, 32+54, fps/10, fps%10 );
           }           
        }
//        else
        {
            if (get_qweasdu_result==0xC0)
            {
//                dump_debug();

                perf_test();

            }
        }
    }
}

void end_of_code()
{
#asm
   nop
   exx
   nop
   exx
   nop
   nop
   nop
   nop
#endasm
}
