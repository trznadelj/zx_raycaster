uint8_t langle, abase, xp; //, debug, debug2, debug3, debug4/*, bb, aa, dd,ee*/;// raycast_loop_x;
uint8_t bufa[5];
uint8_t o_table[256], o_index; // x-addr list
void raycast2()
{
    // uses bmap located at 0xFF00. 16x16.
    // Player position are in _px, _py - 4 oldest bits are not used.
    // produces table with 32 entries, one for each column of zx spectrum screen.
    //   Ybuf   - wall height
    //   Ybuf_c - wall color
    //   Ybuf_i - wall position within 16x16 area
    //   Ybuf_s - zeroed. Not used/for removal
    // If wall color is 0xFF, it skips it as a wall, but populates o_table with following:
    //   [x_index, wall_position].
    // This can be used to signal that there is monster/object visible.
    //
    // Register mapping
    //   hl: for most part, maps to current bmap location
    //  iyh: bmaddr, address within map.
    //  iyl: raycast_loop_x. 0...31
    //  ixl: loop counter, used for tmpl1 as well.
    //  ixh: tmpl2
#asm
    //    xp = pa+224;
    //    abase = ((px>>8)&0xF)|((py>>4)&0xF0);
    //    raycast_loop_x=0;
    ld a, (_py+1)
    rla
    rla
    rla
    rla
    and 0xF0
    ld b, a
    ld a, (_px+1)
    and 0xF
    or b

    ld (_bufa+4), a

    xor a
    ld iyl, a // raycast_loop_x
#ifdef DO_POPULATE_OTABLE
    ld (_o_index), a
#endif

    ld a, (_pa)
    add 224 
    ld (_xp),a
    ld b, a
    and 0x1E
    ld a, b
    jr z, raycast_repeat2 // don't do calculate quarter if it iwll be called from within the loop. Unlikely

    ld hl, raycast_repeat
    push hl

calculate_quarter: // a contains _xp
    // quarter is quadrant. One of eigth. 3 oldest bits of angle in xp (0...255)
    // \ 7|0 /
    //  \ | /
    // 6 \|/ 1
    // ---+---
    // 5 /|\ 2
    //  / | \
    // / 4|3 \

    rlca
    rlca
    rlca
    and 7
    ld b, a
    ld a, (_py)             // 13
    ld d, a                 // 4
    ld a, (_px)             // 13
    ld e, a                 // 4
    ld a, b
    add 2
    and 4
    jnz quarter_pyinversionskip  // if ((q<2)||(q>5)) _py=_py^0xFF;

    ld a, d         // invert D (_py) for quarters 0,1,6,7
    cpl 
    ld d, a	
quarter_pyinversionskip:
    ld a, b
    cp 4
    jr c,quarter_pxinversion_skip  // invert E (_px) for quarterss 4,5,6,7

    ld a, e
    cpl 
    ld e, a	
quarter_pxinversion_skip:
    ld a, b  // if ( (q^(q>>1))&1 ) -> quadrants: 1,2,5,6 
    rra 
    xor b
    and 1
    jr nz, quarter_quarter1
quarter_quarter0:
    ld a, d
    ld d, e
    ld e, a
quarter_quarter1:
    ld a, b //(_quarter)            // d=d_array[2*quarter]; e= d_array[2*quarter+1];
    add a, a
    ld b, 0
    ld c, a
    ld a, e
    neg
    ld e, a


    ld a, l //preserve l
    ld hl, _d_array
    add hl, bc
    ld b,(hl) 
    inc hl
    ld c,(hl)
    di             //4
    ld hl, sp      //6
    ld sp, _bufa+4 //10
    push de        //11
    push bc        //11
    ld sp, hl      //6
    ei             //4
    ld l, a        //4 => 60
       
    ret

raycast_repeat:
    ld a, (_xp)
raycast_repeat2:
    /*** Increase XP by 2, call calculate_quarter if needed ***/
    ld l, a
    and 0x1E
    ld a, l
    call z, calculate_quarter
    ld a, l
    add a, 2
    ld (_xp),a

    /*** Calculate angle ****/
    ld a, 0x3F
    and l                   // 5 less signifant bits of angle
    cp 0x20
    jr c, dont_rotate_angle // we need to map <0...pi/2) into <0...pi/4> u (pi/4...0). Only angles less than 45deg are used for tangent.
    xor 0x3f       // sub 0x40 + neg is slower by 4 cycles
    inc a
dont_rotate_angle:
    ld b, a                     // angle = _xp&0x3F>0x20?0x40-xp&0x3F:xp&0x3F;  0..0x20

    /*** Get Langle ***/
    //uint8_t  zaddr  = ((x>=16)?(x-16):(15-x));
    //uint8_t langle = cc_table[zaddr+(((uint16_t)angle)<<4)];
    add a,a
    add a,a           // a is 0x20 at most. We can do 2 shifts on 8 bits.
    ld h,0
    ld l,a
    add hl, hl
    add hl, hl
    ld de, _cc_table
    add hl,de          // hl=&ctable[angle<<4]

    ld a, iyl
    sub 16
    jr nc, skip_x_rotation
    cpl
skip_x_rotation:
    and 0xF
    ld d,0
    ld e,a
    add hl,de
    ld a, (hl)
push af
///    ld (_langle), a // langle = ctable[ angle<<4  + raycast_loop_x>15?raycast_loop_x-16:16-raycast_loop_x ]   

    /*** Get TAN->c, CTAN->ctan_a ***/
    ld hl, _ctan_tan_table
    ld e, b
    sla e
    add hl, de
    ld c, (hl)   // store tan in C register
    inc hl
    ld d, (hl)   // ctan (with hardcoded exponent)
    ld e, b
    push de      // store ctan + angle on stack

    /*** Setup loop starts ***/
    ld hl, _bufa // 10
    ld e, (hl)   // 7
    inc hl       // 6
    ld d, (hl)   // 7
    inc hl       // 6
    ld a, (hl)   // 7
    inc hl       // 6
    ld b, (hl)   // 7 -> 39+17 => 56
    inc hl
    ld l, (hl)   // => 69 
//    neg
    or a
    jp nz,do_local_fix
    ld ix, 1  // tmpl1=tmpl2=ixl=0
    ld  a, b  // _pp+=tan_a
    add c
    ld  b, a 
    jp setup_loop

do_local_fix:

    ld ixl, 0  // tmpl1=tmpl2=ixl=0
    ld ixh, a
    ld a, c
    exx        // we do care about bc, hl here
    ld c, ixh
    call mul8u_a_c_reg // tmpl2 * ; first mul, needed to obtain x-shift when ray crosses 2 fast. Happens with 50% chance?
    ld a, h
    exx

    add b
    ld b, a
setup_loop:
    ld h, BMAP_SEG

/*  Original source code:
    while(1)
    {
        if (_pp>255) { bmaddr+=daddrs; _pp&=0xFF; }
        if (bmap[bmaddr]) goto after_the_loop; 
        bmaddr+=daddr; 
        if (bmap[bmaddr]) goto loop_end;
        _pp+=tan_a;
        len+=256;
     } 

     Register mapping:
       HL  - points to bmap. It is located at 0xFF00 (16x16) so we only need to do math on L
       B   - ray position 
       C   - value to be added to ray position each iteration. If overflow then ray crossing is assumed.
       D,E - values to be added to L register. D added each iteration, E if angled ray crosses border.
             Values inside can be only of: +1, -1, +16, -16 only (or: 1, 255, 16, 240)
       IXL - loop counter. Used to calculate length afterwards.

     Speed:
       95 cycles on average. 
       Could be improved by 8 cycles by exchanging one of D,E additions with inc/dec.
       This would require multiple version of the loop, even with self modyfing code.
*/

/*
fast_raycast_loop1:
    ld a, l                4
    add e                  4
    ld l, a                4           
skip_move:
    xor a                  4           --- 16
    cp (hl)                7
    jr nz, object_check1   7
    dec/inc l              4
    cp (hl)                7
    jr nz, object_check2   7           ---- 32
    inc d                  4
    ld a,b                 4
    add c                  4     
    ld b,a                 4           -----16
    jr nc, skip_move       12          -----: 64+12 => 76
    jp fast_raycast_loop   10


fast_raycast_loop2 - second variety is not so good.
    dec/inc l              4
skip_move:
    xor a                  4           --- 16
    cp (hl)                7
    jr nz, object_check1   7
    ld a, l
    add d
    ld l, a
    xor a                  4           --- 16
    cp (hl)                7
    jr nz, object_check2   7           ---- 32
    inc e                  4
    ld a,b                 4
    add c                  4     
    ld b,a                 4           -----16
    jr nc, skip_move       12          -----: 64+12 => 76
    jp fast_raycast_loop   10

*/
fast_raycast_loop_preamble:
    jr nc, skip_move       // 12
fast_raycast_loop:
    ld a, l                // 4
    add e                  // 4                         // inc /dec => 8 cycles faster
    ld l, a                // 4 addr+=daddrs,  L+=E
skip_move:
    ld a,(hl)              // 7
    or a                   // 4
#ifdef DO_POPULATE_OTABLE
    jr nz,object_check1    // 7  (30) if (bmap[addr]) goto after_the_loop;   check if there is wall in this place
object_found_continue1:
#else
    jr nz,object_not_found1         // --- 30

#endif

    ld a, l                // 4
    add d                  // 4
    ld l, a                // 4 addr+=daddr;  L+=D

    ld a,(hl)              // 7
    or a                   // 4
#ifdef DO_POPULATE_OTABLE
    jr nz,object_check2    // 7 (30) if (bmap[addr]) goto loop_end;        -- 30
object_found_continue2:
#else
    jr nz,object_not_found2

#endif

    inc ixl                // 8 ixl++

    ld a, b                // 4
    add c                  // 4
    ld b, a                // 4 (20) _pp+=tan_a ;  B+=C    
    jr nc, skip_move       // 12                                            -- 32 => 92

    // second copy of loop. Save on one jump.
    //jmp fast_raycast_loop  // 10

    ld a, l                // 4
    add e                  // 4                         // inc /dec => 8 cycles faster
    ld l, a                // 4 addr+=daddrs,  L+=E
    ld a,(hl)              // 7
    or a                   // 4
#ifdef DO_POPULATE_OTABLE
    jr nz,object_check1    // 7   if (bmap[addr]) goto after_the_loop;   check if there is wall in this place
#else
    jr nz,object_not_found1
#endif

    ld a, l                // 4
    add d                  // 4
    ld l, a                // 4 addr+=daddr;  L+=D

    ld a,(hl)              // 7
    or a                   // 4
#ifdef DO_POPULATE_OTABLE
    jr nz,object_check2       // 7 if (bmap[addr]) goto loop_end;
#else
    jr nz,object_not_found2
#endif
    inc ixl                // 8 ixl++

    ld a, b                // 4
    add c                  // 4
    ld b, a                // 4 _pp+=tan_a ;  B+=C    
    jr nc, skip_move       // 12
    jmp fast_raycast_loop  // 10


#ifdef DO_POPULATE_OTABLE
object_check1:
    cmp a,0xFF             // 0xFF is used to signal that is an object, not actual wall. Lets populate o_table here.
    jnz object_not_found1     
object_found1:
    exx
    ld hl, _o_table
    ld b, a
    ld a, (_o_index)
    ld c, a
    add a, 2
    ld (_o_index), a
    ld a, b
    ld b, 0
    add hl, bc                   // hl = &o_table[ o_index ]; o_index+=2;
    ld a, iyl                    // iyl: raycast_loop_x
    ld (hl), a                   // o_table[ o_index ] = raycast_loop_x;
    exx
    ld a, l
    exx
    inc hl
    ld (hl), a                   // o_table[ o_index+1 ] = address in bmap.
    exx
    jp object_found_continue1
object_check2:
    cmp a,0xFF
    jnz object_not_found2
object_found2:
    exx
    ld hl, _o_table
    ld b, a
    ld a, (_o_index)
    ld c, a
    add a, 2
    ld (_o_index), a
    ld a, b
    ld b, 0
    add hl, bc
    ld a, iyl // iyl: (_raycast_loop_x)
    ld (hl), a
    exx
    ld a, l
    exx
    inc hl
    ld (hl), a
    exx
    jp object_found_continue2
#endif
object_not_found2:
    ld a,   l
    ld iyh, a   // iyh: bmaddr
    pop de     // ctan will not be used.
    jp skip_tmpl1_dec2

object_not_found1:
    ld a,   l
    ld iyh, a   // iyh: bmaddr
    ld c,   b
    pop de  // d: ctan_a, e: angle
    ld a, d // (_ctan_a)
    call mul8u_a_c_reg  // ctan * counter (in d). Second mul. Needed if we hit the wall not directly. 50 chance
    ld a, e //(_angle)
    cp 4
    jr c,skip4

#ifdef DO_HIGH_CTAN_PRECISION
    cp 16
    jr c,skip16
#endif

    ld a, l   // rra is 4 cycles,  rrl 8. 
    srl h
    rra
    srl h
    rra
skip4:
    srl h
    rra
    srl h
    rra
#ifdef DO_HIGH_CTAN_PRECISION
skip16:
    srl h
    rra
    srl h
    rra
#endif
    ld l, a

/*** IX(l,h) -= l - 30 cycles (on average) ***/
skip_tmpl3:
    ld a, ixh              // 8       // ixh:(_tmpl2)         if (tmpl2<tmpl3) tmpl1--;
    sub l                  // 4  // tmpl2-=tmpl3;
    jp nc, skip_tmpl1_dec  // 10
    dec ixl                // 8     
skip_tmpl1_dec:
    ld ixh, a              // 8 ixh: tmpl2
skip_tmpl1_dec2:
    // Alternative not worth it.
    //  ld   a, l         // 4
    //  neg               // 8
    //  ld   e, a         // 4
    //  ld   d, 0xff      // 7
    //  add ix, de        // 15

    /* Original C code snippet:
        uint16_t cc;
        cc = tmpl1; cc<<=8; cc |= tmpl2; //cc <<=1;   // cc= [tmpl1|tmpl2]<<1;
        if (tmpl1)
            cc+= mul8all(tmpl1, langle);            // cc+=[tmpl1|tmpl2]*ltable[a]>>8
        if (tmpl1<4)
           cc+= mul8( tmpl2, langle);
        cc>>=3; */


    ld a, ixl
    cp 3
    jr nc, skip_second_mul 
    ld d, a           // preserve A


    pop af           //    ld a, (_langle) - slightly faster to go through stack
    ld e, a
    ld c, a

    ld a, ixh
    call mul8u_a_c_reg   // hl = langle*ixh // third MUL. Needed to rotate distance into camera view
    ld  l, h       //ixl=0
    ld  h, 0

    ld  a, d
    cp 1
    jr c, after_second_mul // ixl==0
    ld  d, h // h is already 0
    jr z, ixl_is_1
    add hl,de  // ixl is 2
ixl_is_1:
    add hl,de 
    ld d, a        //ixl
    jp after_second_mul
skip_second_mul:
    ld d, a        //ixl
    ld c, a
//    ld a, (_langle)
    pop af
    call mul8u_a_c_reg                      // third MUL. Needed to rotate distance into camera view
after_second_mul:
    // not needed anymore:   ld d, ixl 
    ld e, ixh // ixh: tmpl2
    add hl,de 

    ld a, h
    cp 15
    jr nc, hl_is_filled

    ld a,l
    srl h     // drop 3 bits from distance. our LUT is just 512, not 4096
    rra
    srl h
    rra
    srl h
    rra
    ld l, a   
//      ld (_cc), hl
//      if (cc>=511) cc=511; 
//        Ybuf  [x]=d_table[ cc ];
//        Ybuf_c[x]=bmap[bmaddr];
//        Ybuf_i[x]=bmaddr;


    /*** Store results **/
    ld a, DTABLE_SEG
    add h
    ld h, a
    ld a, (hl)
hl_is_filled_cnt:
    ld e, iyl // iyl: (_raycast_loop_x)
    ld h, YBUF_SEG
    ld l, e
    ld (hl), a  // Ybuf[x] = a
    set 5, l
    ld b, BMAP_SEG
    ld c, iyh    
    ld a, (bc)  //  a = bmap[a]
    ld (hl), a  // Ybuf_c = bmap[a]
    set 6, l    // faster than ld a, 32; add l; add l, a - 16 cycles vs 18
    res 5, l
    ld (hl), c  // Ybuf_i

    inc e      // 4 - faster than inc iyl, ld a,iyl, cp 32 by 3 cycles
    ld iyl, e  // 8
    bit 5, e   // 8
    jp z, raycast_repeat
    ret
hl_is_filled:
    ld a,4
    jp hl_is_filled_cnt
//    ld hl, 511         // can saturate if looking through all the map...
    
#endasm
}

