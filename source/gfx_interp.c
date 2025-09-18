uint8_t m4[] = 
{
    0x0F, 0x00, 0x00, 0x00, // dy=2, xx=0, r=0  /// going down, down side
    0x1F, 0x07, 0x00, 0x00, // dy=3, xx=0, r=0   ----
    0x3f, 0x0f, 0x03, 0x00, // dy=4, xx=0, r=0     --
    0x3f, 0x0f, 0x07, 0x03, // dy=5, xx=0, r=0   

    0xF0, 0xFF, 0xFF, 0xFF, // dy=2, xx=1, r=0    // going down, up side
    0xE0, 0xF8, 0xFF, 0xFF, // dy=3, xx=1, r=0
    0xC0, 0xF0, 0xFC, 0xFF, // dy=4, xx=1, r=0    --
    0xC0, 0xE0, 0xF8, 0xFC, // dy=5, xx=1, r=0    -----

    0x0f, 0xFF, 0xFF, 0xFF, // dy=2, xx=0, r=1 /// going up, up side
    0x07, 0x1f, 0xFF, 0xFF, // dy=3, xx=0, r=1
    0x03, 0x0f, 0x3f, 0xFF, // dy=4, xx=0, r=1      --
    0x03, 0x0f, 0x1f, 0x3f, // dy=5, xx=0, r=1    ----

    0xF0, 0x00, 0x00, 0x00, // dy=2, xx=1, r=1   // going up, down side
    0xF8, 0xE0, 0x00, 0x00, // dy=3, xx=1, r=1
    0xFC, 0xF0, 0xC0, 0x00, // dy=4, xx=1, r=1    ----
    0xFC, 0xF8, 0xE0, 0xC0  // dy=5, xx=1, r=1    --       
};


uint8_t m8[] =
{
    0x7F, 0x1F, 0x0F, 0x3, 0x1, 0x0, 0x0, 0x0, //going down, down side
    0x7F, 0x3F, 0x1F, 0x7, 0x3, 0x1, 0x0, 0x0,
    0x7F, 0x3F, 0x1F, 0xF, 0x7, 0x3, 0x1, 0x0,
    0x7F, 0x3F, 0x1F, 0xF, 0xF, 0x7, 0x3, 0x1,


    0x80, 0xC0, 0xF0, 0xFC, 0xFE, 0xFF, 0xFF, 0xFF, //going down, up side
    0x80, 0xC0, 0xE0, 0xF8, 0xFC, 0xFE, 0xFF, 0xFF,
    0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE, 0xFF,
    0x80, 0xC0, 0xE0, 0xF0, 0xF0, 0xF8, 0xFC, 0xFE,

    0x01, 0x07, 0x0F, 0x3F, 0x7F, 0xFF, 0xFF, 0xFF,
    0x01, 0x03, 0x07, 0x0F, 0x3F, 0x7F, 0xAA, 0xAA,
    0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0xAA,
    0x01, 0x03, 0x07, 0x0f, 0x0f, 0x1f, 0x3f, 0x7f,


    0xFE, 0xF8, 0xF0, 0xE0, 0x80, 0x00, 0x00, 0x00, // going up, down side
    0xFE, 0xFC, 0xF8, 0xF0, 0xC0, 0x80, 0x00, 0x00, 
    0xFE, 0xFC, 0xF8, 0xF0, 0xE0, 0xC0, 0x80, 0x00, 
    0xFE, 0xFC, 0xF8, 0xF0, 0xF0, 0xE0, 0xC0, 0x80,
};


uint16_t* fast_stamp_y_tab, fast_stamp_sp;
uint8_t* fast_stamp_src;
uint8_t  fast_stamp_x;
void fast_stamp_4()
{
#asm
    ld (_fast_stamp_sp), sp
    ld hl, (_fast_stamp_y_tab)
    di
    ld sp, hl

    ld hl, (_fast_stamp_src)

    ld a, (_fast_stamp_x) 
    ld c, a

    pop de
    add a, e
    ld e, a  // addr in hl
    ld a, (hl)
    inc hl
    ld (de), a

    pop de
    ld a, e 
    add c
    ld e, a
    ld a, (hl)
    inc hl
    ld (de), a

    pop de
    ld a, e 
    add c
    ld e, a
    ld a, (hl)
    inc hl
    ld (de), a

    pop de
    ld a, e 
    add c
    ld e, a
    ld a, (hl)
    ld (de), a

    ld hl, (_fast_stamp_sp)
    ld sp, hl
    ei
#endasm
}


void fast_stamp_8()
{
#asm
    ld (_fast_stamp_sp), sp
    ld hl, (_fast_stamp_y_tab)
    di
    ld sp, hl

    ld hl, (_fast_stamp_src)

    ld a, (_fast_stamp_x) 
    ld c, a

    pop de
    add a, e
    ld e, a  // addr in hl
    ld a, (hl)
    inc hl
    ld (de), a

    pop de
    ld a, e 
    add c
    ld e, a
    ld a, (hl)
    inc hl
    ld (de), a

    pop de
    ld a, e 
    add c
    ld e, a
    ld a, (hl)
    inc hl
    ld (de), a

    pop de
    ld a, e 
    add c
    ld e, a
    ld a, (hl)
    inc hl
    ld (de), a

    pop de
    ld a, e 
    add c
    ld e, a
    ld a, (hl)
    inc hl
    ld (de), a

    pop de
    ld a, e 
    add c
    ld e, a
    ld a, (hl)
    inc hl
    ld (de), a

    pop de
    ld a, e 
    add c
    ld e, a
    ld a, (hl)
    inc hl
    ld (de), a

    pop de
    ld a, e 
    add c
    ld e, a
    ld a, (hl)
    ld (de), a

    ld hl, (_fast_stamp_sp)
    ld sp, hl
    ei
#endasm
}

uint8_t interp_x;
uint8_t interp_y0; // y0<y1
uint8_t interp_y1;
uint8_t interp_dy;
uint8_t interp_xx;
uint8_t interp_r;
uint8_t interp_c;
uint8_t y, y0, *p, c, ya, ys, *it;
uint16_t tmpsp;

void interp()
{
//    interp_dy = interp_y1-interp_y0;
    y =((interp_y0-1)&0xF8) ;
    p = ytable[y]+interp_x;
    y =interp_y0-y;
    c= 0xFF^interp_xx^interp_r;

#asm
    //    for( ; y>0; y--, p+=256)  *p=c;    
    ld hl, interp2
    push hl
    ld a, (_y)
    sub 8
    neg
    ld b, 0
    ld c, a
    call setup_a2hl_inc
    push hl
    ld hl, (_p)
    ld a, (_c)
    ret
interp2:
    ld a, (_interp_y0)
    ld b, a   
    ld a, (_interp_y1)
    sub b
    ld (_interp_dy), a

    ld b, a
    cp 9
    jr nc, interp1
    ld a, (_interp_y0)
    ld l, a
    ld h, 0
    add hl, hl
    ld  a, h
    add a, YTABLE_SEG
    ld h,a
    ld (_fast_stamp_y_tab), hl

    ld a, (_interp_x)
    ld (_fast_stamp_x), a

    ld c, b
    ld a, b
    cp 5
    jr c, less_than_5_first
    dec c
    dec c
    dec c

less_than_5_first:
    dec c
    dec c
    sla c
    sla c
    ld a, (_interp_r)
    or a
    jr z,interp_r_zero
    set 5, c
interp_r_zero:
    ld a, (_interp_xx)
    or a
    jr z,interp_xx_zero
    set 4, c
interp_xx_zero:
    ld a, b
    cp 5
    jr nc, more_than_5
    ld hl, _m4
    ld b, 0
    add hl, bc
    ld (_fast_stamp_src), hl
    call _fast_stamp_4
    jp interp1
more_than_5:
    ld hl, _m8
    ld b, 0
    sla c
//    ld a, c
//    sub 24
//    ld c, a
    add hl, bc
    ld (_fast_stamp_src), hl
    call _fast_stamp_8
interp1:
#endasm


    if (interp_dy<9)
    {   
/*                    
        uint16_t *fast_y = ytable+interp_y0;//+((interp_r&1));
        uint8_t *m4i;

        if (interp_dy<5) 
        {
            m4i = m4+ (((interp_dy-2) | (interp_xx?4:0) | (interp_r?8:0))<<2);
            fast_stamp_y_tab = fast_y; fast_stamp_src = m4i; fast_stamp_x=interp_x; fast_stamp_4();
        }
        else
        {
            m4i = m8+ (((interp_dy-5) | (interp_xx?4:0) | (interp_r?8:0))<<3);
            fast_stamp_y_tab = fast_y; fast_stamp_src = m4i; fast_stamp_x=interp_x; fast_stamp_8();
        }

        /*for( uint8_t i=0; i<4; i++)
        {
            p=fast_y[i]+interp_x;
            *p=m4i[i];
        } */

#asm
end_stamping:
        ld a, (_interp_y0)
        and 0xF8
        ld l, a
        ld h, 0x16 //0x58>>2  // 16
        sla l
        rl h
        sla l
        rl h
        ld a, (_interp_x)
        add a, l
        ld l, a
        ld a,(_interp_c)
        ld (hl),a

        ld a, (_interp_y1)
        and 0xF8
        ld l, a
        ld h, 0x16 //0x58>>2  // 16
        sla l
        rl h
        sla l
        rl h
        ld a, (_interp_x)
        add a, l
        ld l, a
        ld a,(_interp_c)
        ld (hl),a

#endasm

/*        p = (unsigned char*) (0x5800+interp_x+(((uint16_t)interp_y0&0xF8)<<2));
        *p=interp_c;
        p = (unsigned char*) (0x5800+interp_x+(((uint16_t)interp_y1&0xF8)<<2));
        *p=interp_c;*/

    }

    else 
    {
     if (interp_dy>64) return;
    y0=0; ya=0; ys=interp_r?1:0x80; p=(ytable[interp_y0]+interp_x); c=(ys-1)^interp_xx;
    y = (interp_y0&0xF8);
//    ya = interp_y1 - interp_y0;
    p = (unsigned char*) (0x5800+interp_x+(y<<2));
    ya = interp_dy;
    it = itable+ya*8;
#asm
    // for( y= (interp_y1-y)>>3; y; y--, p+=32) *p=interp_c;
    ld hl, (_p)
    ld a, (_y)
    ld b, a
    ld a, (_interp_y1)
    sub b
    add 7
    rra 
    rra 
    rra 
    and 0x1F
    ld b,a
    ld a, (_interp_c)
repeat:    
    ld (hl),a
    add hl,32
    dec b
    jr nz,repeat

    di

 /*
        for( y0=0; y0<8; y0++, it++)
        {
            for( uint8_t i=*it; i>0; i--)
            {
                 *p=c;
                 //p = ytable[y]+interp_x;
                 y++;
                 if (!(y&7)) p=(ytable[y]+interp_x); else p+=256;
            }
            ys=interp_r?ys<<1:ys>>1;
            c=(ys-1)^interp_xx;
        }
 */

     ld hl, sp
     ld (_tmpsp), hl

     ld hl, do_sr

     ld a, (_ys)
     ld b, a
     ld a, (_interp_r)
     and a
     jr z, lll1
     ld a, 0x17
     jp lll2
lll1:
     ld a, 0x1F
lll2:
     ld (hl), a // self-modyfing code

     ld hl, (_it)
     ld c, 8
     ld a, (_interp_xx)
     ld d, a
     ld a, (_c)
     ld e, a

     // exx: hl = it
     // exx: b = _ys
     // exx: c = 8
     // exx: d = _interp_xx
     // exx: e = _c

     exx

     ld a, (_interp_y0)
     ld l, a
     ld h, YTABLE_SEG/2
     add hl, hl
     ld sp, hl

/*
     ld hl, _ytable
     ld a, (_interp_y0)
     ld d, 0
     ld e, a
     add hl, de // de = &(ytable[interp_y0])
     add hl, de // de = &(ytable[interp_y0])
     ld sp, hl
     //ex de, hl  // de = &(ytable[interp_y0])                             
 */
     ld a, (_interp_x)     
     ld b, 0
     ld c, a     

     //init. de = pointer to ytable.  bc=interp_x
     ld a, 8
     ld (_y0), a
     //ld iyl, 8

     ld a, (_c)
     ld e, a
     ld b, 0     
loop0:
     exx
     ld a, (hl)  
     inc hl
     and a
     jr z, end_loop1
     exx
     ld d, a      // ixh = *(it++);
     ld a, e
loop1_cont:     // DE: ytable pointer.  A: color. IXH: loop cnt BC: offset_x, HL: graphics pointer
     pop hl     // 10C
     add hl, bc // 11        
     ld (hl),a  // 7   ytable[de]+interp_x = a
     dec d      // 4
     jr nz,loop1_cont // 12/7
     exx
end_loop1:
     ld a, b
do_sr:
     rra // self-modyfing code
     ld b,a
     dec a
     xor d
     dec c
     exx
     ld e,a
     jr nz,loop0

     ld  hl, (_tmpsp)
     ld  sp, hl

     ei
#endasm
     }
/*    }
    else
        for( y=interp_y0; y<interp_y1; y0++)
        {
            *p = c;
            ya+=8;
            while( ya>=interp_dy) { ys=interp_r?ys<<1:ys>>1; ya-=interp_dy; c=(ys-1)^interp_xx; }
            y++;
            if (!(y&7)) p=(ytable[y]+interp_x); else p+=256;
        }*/

    c=interp_xx^interp_r;
    y=interp_y1;
    p = ytable[y]+interp_x;    
    y = 8-(interp_y1&7);


#if 1
#asm
//    for( ; y>0; y--, p+=256)
//         *p=c;
    
    ld hl, interp3
    push hl
    ld a, (_y)
    sub 8
    neg
    ld b, 0
    ld c, a
    call setup_a2hl_inc
    push hl
    ld hl, (_p)
    ld a, (_c)
    ret
interp3:
#endasm
#endif	

//    for( y=; y>0; y--,p+=256)
//         *p=c;
}

