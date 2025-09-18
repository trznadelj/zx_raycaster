uint8_t langle, abase, xp; //, debug, debug2, debug3, debug4/*, bb, aa, dd,ee*/;// raycast_loop_x;

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
    add a, a
    add a, a
    add a, a
    add a, a
    ld b, a
    ld a, (_px+1)
    and 0xF
    or b

    ld (_bufa+2), a

    xor a
    ld iyl, a // raycast_loop_x
#ifdef DO_POPULATE_OTABLE
    ld (_o_index), a
#endif

    ld a, (_pa)
    add 224 
    ld (_xp),a

    /* Preamble */
    ld l, a

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
    jr z, quarter_quarter1
quarter_quarter0:
    ld a, d
    ld d, e
    ld e, a
quarter_quarter1:
    ld a, b //(_quarter)            // d=d_array[2*quarter]; e= d_array[2*quarter+1];
    add a, a
    ld b, 0
    ld c, a
    ld a, d
    neg
    ld d, a


    ld a, l //preserve l
    ld hl, _d_array
    add hl, bc
    ld b,(hl) 
    inc hl          // Optimalisation: l if darray is not crossing boundary
    ld c,(hl)
    di             //4
    ld hl, sp      //6
    ld sp, _bufa+5 //10
    push de        //11    from end:  DE * BC
    dec sp
    push bc        //11
    ld sp, hl      //6
    ei             //4
#ifdef DO_OPTIMIZED_RAYCASTING
    ld e, a        //4 => 60
    ld a, b
    cp 1 
    jr z, o2_inc
    cp 255
    jr z, o2_dec

    ld a, c
    cp 255
    jr z, o1_dec
o1_inc:
    ld hl, fast_raycast_loop_preamble_o1
    ld (fast_raycast_loop_preamble-2), hl
    ld a, 0x2c
    ld (fast_raycast_loop_o1), a
    ld (fast_raycast_loop_o1_inc2), a
    jp endd
o1_dec:
    ld hl, fast_raycast_loop_preamble_o1
    ld (fast_raycast_loop_preamble-2), hl
    ld a, 0x2d
    ld (fast_raycast_loop_o1), a
    ld (fast_raycast_loop_o1_inc2), a
    jp endd
o2_inc:
    ld hl, fast_raycast_loop_preamble_o2
    ld (fast_raycast_loop_preamble-2), hl
    ld a, 0x2c
    ld (skip_move_o2_inc), a
    ld (skip_move_o2_inc2), a
    jp endd
o2_dec:
    ld hl, fast_raycast_loop_preamble_o2
    ld (fast_raycast_loop_preamble-2), hl
    ld a, 0x2d
    ld (skip_move_o2_inc), a
    ld (skip_move_o2_inc2), a
endd:
    ld l, e

#else
    ld l, a
#endif
    jp after_calculate_quarter

raycast_repeat:
    /*** Increase XP by 2, call calculate_quarter if needed ***/
    ld a, (_xp)
    ld l, a
    and 0x1E
    ld a, l
    jp z, calculate_quarter // unlikely
after_calculate_quarter:
    ld a, l
    add a, 2
    ld (_xp),a

    /*** Calculate angle ****/
    /* 33T: 36 / 30 T states.  */
    ld a, 0x3F     // 7T
    and l          // 4T  less signifant bits of angle
    cp 0x20        // 7T
    jr c, dont_rotate_angle // 7/12T // we need to map <0...pi/2) into <0...pi/4> u (pi/4...0). Only angles less than 45deg are used for tangent.
    xor 0x3f       // 7T  sub 0x40 + neg is slower by 4 cycles
    inc a          // 4T angle = _xp&0x3F>0x20?0x40-xp&0x3F:xp&0x3F;  0..0x20
dont_rotate_angle:

    /*** Get Langle ***/
    /* 132.5T   */
    // Original code: uint8_t langle = cc_table[((x>=16)?(x-16):(15-x))+(((uint16_t)angle)<<4)];

    // there are 6 possible steps: angle: 0....0x20, steps by 2.  x_ is 0...16, steps by 1 from 15...0...16
    //  step = (x<16)?-1:1  +  big_angle&0x20?0xFFE0:0x20, if angle==1 | 1F angle=
    //    21  : angle +, x +
    //  FFE1  : angle -, x +
    //     1  : angle =, x +
    //    1F  : angle +, x -
    //  FFDF  : angle -, x -
    //  FFFF  : angle =, x -

    //  pop hl             10
    //  ld de, nn          10
    //  add hl, de         12
    //  push hl            10
    //  ld a, (hl)          7
    //  push af            10 => 59 states!!!!!!!
    //  ld hl, set         10
    //   dec (hl)           7
    //  jz do_recalc        7 => 24  :). Then it runs for 105 states. + 1-5 recalcs. Recalc is 21, E1, 1, 1F, DF, FF. Older byte is taken from msb
    //   ld a, (recalc_index)    13
    //   add 3                    7
    //   ld (recalc_index), a    13 33
    //   ld h, recalc_seg         4
    //   ld l, a                  4
    //   ld a, (hl)               7
    //   ld (set), a             13
    //   inc l                    4
    //   ld de, _code_offset     10
    //   ld a, (hl)               7
    //   ld (de),a                7
    //   rla                      4
    //   ld a, 0xFF               7
    //   adc a                    4
    //   inc e                    4
    //   ld (de),a                7  20+35 = 55 => 120 states
    //   jp back

    // So...
    //     4 recals avg.  83 + 4/32*120 (15) => 98 vs 132.5. 34 cycles! but complicated as hell?

    add a,a
    ld b, a        // b = 2*angle
    add a,a        // a is 0x20 at most. We can do 2 shifts on 8 bits.
    ld h, 0
    ld l, a
    add hl, hl
    add hl, hl     // 42T
    ld de, _cc_table   // 10T
    add hl,de          // 11T; hl=&ctable[angle<<4]
    ld a, iyl
    sub 16
    jr nc, skip_x_rotation
    cpl
skip_x_rotation:
    and 0xF
    ld d, 0
    ld e, a
    add hl,de
    ld a, (hl)
    push af      // store cc_table value on stack. ctable[ angle<<4  + raycast_loop_x>15?raycast_loop_x-16:16-raycast_loop_x ]

    /*** Get TAN->c, CTAN, angle->stack ***/
    /* 57T. b: angle, d: 0 */
    ld hl, _ctan_tan_table   // 10T
    ld e, b      // 4T
    add hl, de   // 11T Optimalisation: 11 cycles better if _ctan_tan table is aligned to 0.
    ld c, (hl)   // 7T  store tan in C register
    inc hl       // 6T  Optimalisation: 2 cycles better if ctan_tan table is not on boundary
    ld d, (hl)   // 7T  ctan (with hardcoded exponent)
    ld e, b      // 4
    push de      // 10T store ctan + angle on stack

    /*** Setup loop starts ***/
    /* 53T */
    ld de, (_bufa)   // 20T
    ld hl, (_bufa+2) // 16T
    ld a,  (_bufa+4) // 13T 

    /*** First correction ***/
    or a
    jp nz,do_local_fix // likely
    ld ix, 1  // tmpl1=tmpl2=ixl=0
    ld  a, h  // _pp+=tan_a
    add c
    jp setup_loop

do_local_fix:
    ld ixl, 0  // tmpl1=tmpl2=ixl=0
    ld ixh, a

#ifdef DO_LUT_TAN_MUL
    exx
    pop de
    push de
    // e contains angle, a contains distance (0...255)
    sra e
    sra e
    rra
    ld l, a
    ld a, e
    add FAST_MUL_SEG
    ld h, a
    ld a, (hl)
    exx
#else
    ld a, c
    exx        // we do care about bc, hl here
    ld c, ixh          // tan*a
    call mul8u_a_c_reg // tmpl2 * ; first mul, needed to obtain x-shift when ray crosses 2 fast. Happens with 50% chance?
    ld a, h            // optimalisation: LUT here. ixh has only 16 possible values.
    exx
#endif
    add h

setup_loop:
    ld b, a
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
       90 cycles on average. 
       Could be improved by 8 +4/2 cycles by exchanging one of D,E additions with inc/dec.
       This would require multiple version of the loop, even with self modyfing code.
*/

#ifdef DO_OPTIMIZED_RAYCASTING
    jp fast_raycast_loop_preamble
#endif  
fast_raycast_loop_preamble:

    /*** Main raycaster loop - fallback only after object detection ***/
    jr nc, skip_move       // 12
fast_raycast_loop:
    ld a, l                // 4
    add e                  // 4      
    ld l, a                // 4      addr+=daddrs;  L+=E
skip_move:
    ld a,(hl)              // 7
    or a                   //  4      if (bmap) goto object_on_angled_crossing
    jp nz, object_on_angled_crossing   // 7 --> 30 T
object_found_continue1:
    ld a, l                // 4
    add d                  // 4
    ld l, a                // 4      addr+=daddr;  L+=D
    ld a,(hl)              // 7
    or a                   // 4      if (bmap) goto object_on_angled_crossing
    jp nz, object_on_straight_crossing // 7 --> 30 T
object_found_continue2:
    inc ixl                // 8      ixl++

    ld a, b                // 4
    add c                  // 4
    ld b, a                // 4 (20) _pp+=tan_a ;  B+=C    
    jr nc, skip_move       // 12                                            -- 32 => 92

    /*** second copy of loop. Save on one jump. ***/
    /* Could be: jp fast_raycast_loop             */

    ld a, l                // 4
    add e                  // 4                         // inc /dec => 8 cycles faster
    ld l, a                // 4 addr+=daddrs,  L+=E
    ld a,(hl)              // 7
    or a                   // 4
    jp nz, object_on_angled_crossing

    ld a, l                // 4
    add d                  // 4
    ld l, a                // 4 addr+=daddr;  L+=D

    ld a,(hl)              // 7
    or a                   // 4
    jp nz, object_on_straight_crossing
    inc ixl                // 8 ixl++

    ld a, b                // 4
    add c                  // 4
    ld b, a                // 4 _pp+=tan_a ;  B+=C    
    jr nc, skip_move       // 12
    jp fast_raycast_loop   // 10


#ifdef DO_OPTIMIZED_RAYCASTING
    /*** Main raycaster loop - E optimized. Saves 8 cycles/iteration ***/
fast_raycast_loop_preamble_o1:
    jr nc, skip_move_o1       // 12
fast_raycast_loop_o1:
    inc l                  // 4      
skip_move_o1:
    ld a,(hl)              // 7
    or a                   // 4      if (bmap) goto object_on_angled_crossing
    jr nz, object_on_angled_crossing   // 7 --> 30 T
    ld a, l                // 4
    add d                  // 4
    ld l, a                // 4      addr+=daddr;  L+=D
    ld a,(hl)              // 7
    or a                   // 4      if (bmap) goto object_on_angled_crossing
    jp nz, object_on_straight_crossing // 7 --> 30 T
    inc ixl                // 8      ixl++

    ld a, b                // 4
    add c                  // 4
    ld b, a                // 4 (20) _pp+=tan_a ;  B+=C    
    jr nc, skip_move_o1    // 12                                            -- 32 => 92

    // second copy of the loop to save on one jp
fast_raycast_loop_o1_inc2:
    inc l                  // 4      
    xor a                  // 7
    cp (hl)                // 4      if (bmap) goto object_on_angled_crossing
    jr nz, object_on_angled_crossing   // 7 --> 30 T
    ld a, l                // 4
    add d                  // 4
    ld l, a                // 4      addr+=daddr;  L+=D
    ld a,(hl)              // 7
    or a                   // 4      if (bmap) goto object_on_angled_crossing
    jr nz, object_on_straight_crossing // 7 --> 30 T
    inc ixl                // 8      ixl++

    ld a, b                // 4
    add c                  // 4
    ld b, a                // 4 (20) _pp+=tan_a ;  B+=C    
    jr nc, skip_move_o1    // 12                                            -- 32 => 92

    jp fast_raycast_loop_o1

    /*** Main raycaster loop - D optimized. Saves 12 cycles/iteration ***/
fast_raycast_loop_preamble_o2:
    jr nc, skip_move_o2       // 12
fast_raycast_loop_o2:
    ld a, l                // 4
    add e                  // 4      
    ld l, a                // 4      addr+=daddrs;  L+=E
skip_move_o2:
    xor a                  // 7
    cp (hl)                // 4      if (bmap) goto object_on_angled_crossing
    jr nz, object_on_angled_crossing   // 7 --> 30 T
skip_move_o2_inc:
    inc l                  // 4
    cp (hl)                // 7      if (bmap) goto object_on_angled_crossing
    jr nz, object_on_straight_crossing // 7 --> 30 T
    inc ixl                // 8      ixl++

    ld a, b                // 4
    add c                  // 4
    ld b, a                // 4 (20) _pp+=tan_a ;  B+=C    
    jr nc, skip_move_o2       // 12                                            -- 32 => 92
    // second copy

    ld a, l                // 4
    add e                  // 4      
    ld l, a                // 4      addr+=daddrs;  L+=E
    xor a                  // 7
    cp (hl)                // 4      if (bmap) goto object_on_angled_crossing
    jr nz, object_on_angled_crossing   // 7 --> 30 T
skip_move_o2_inc2:
    inc l                  // 4
    cp (hl)                // 7      if (bmap) goto object_on_angled_crossing
    jr nz, object_on_straight_crossing // 7 --> 30 T
    inc ixl                // 8      ixl++

    ld a, b                // 4
    add c                  // 4
    ld b, a                // 4 (20) _pp+=tan_a ;  B+=C    
    jr nc, skip_move_o2       // 12                                            -- 32 => 92

    jp fast_raycast_loop_o2
#endif
            
#ifdef DO_POPULATE_OTABLE
object_on_angled_crossing:
    cmp a,0xFF             // 0xFF is used to signal that is an object, not actual wall. Lets populate o_table here.
    jp nz, object_on_angled_crossing_regular
//object_found1:
    /*** Populate Otable ***/
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
object_found2:
    /*** Populate Otable and return to object_found_continue2 ***/
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

object_on_straight_crossing:
    cp  a,0xFF
    jr  z, object_found2
#endif

#ifdef DO_POPULATE_OTABLE
object_on_straight_crossing_regular:
#else
object_on_straight_crossing:
#endif
    ld a,   l
    ld iyh, a   // iyh: bmaddr
    pop de     // ctan will not be used.
    jp skip_tmpl1_dec2

#ifdef DO_POPULATE_OTABLE
object_on_angled_crossing_regular:
#else
object_on_angled_crossing:
#endif
    ld a,   l
    ld iyh, a   // iyh: bmaddr
    ld c,   b
#ifdef DO_LUT_CTAN_MUL

    ld a, c
    exx
    pop de
    push de
    sra e
    and 0xFE
    ld l, a
    ld a, e
    add FAST_CTAN_MUL_SEG
    ld h, a
    ld a, (hl)
    exx
    ld h, a
    exx
    inc hl
    ld a, (hl)
    exx
    ld l, a
    pop de
#else
    pop de  // d: ctan_a, e: angle
    ld a, d // (_ctan_a)
    call mul8u_a_c_reg  // ctan * counter (in d). Second mul. Needed if we hit the wall not directly. 50 chance
#endif
    ld a, e //(_angle*2)

#ifdef DO_HIGH_CTAN_PRECISION
    cp 32
    jr c,skip16
#endif
    cp 8
    jr c,skip4


    ld a, l   // rra is 4 cycles,  rra l 8. 
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
    /*** IXL:IXH -= l ***/
skip_tmpl3:
    ld a, ixh              // 8       // ixh:(_tmpl2)         if (tmpl2<tmpl3) tmpl1--;
    sub l                  // 4  // tmpl2-=tmpl3;
    jp nc, skip_tmpl1_dec  // 10
    dec ixl                // 8     
skip_tmpl1_dec:
    ld ixh, a              // 8 ixh: tmpl2
skip_tmpl1_dec2:
    /*  IXL:IXH contains distance in dominating dimension  */

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
    jr nc, skip_second_mul // if ixl is greater than 3 (large distance - skip small distance from calculus)

    /* IXL is 0...3. We will do multiplication by hand here */
    ld d, a           // preserve A
    pop af           //    ld a, (_langle) - slightly faster to go through stack
    ld e, a
    ld h, a
    ld c, ixh
    call mul8u_h_c_reg   // hl = langle*ixh // third MUL. Needed to rotate distance into camera view
    ld  l, h       // ixl=0
    ld  h, 0

    ld  a, d
    cp 1
    jr  c, after_second_mul // ixl==0. nothing to be done here.
    ld  d, h       // h is already 0
    jr  z, ixl_is_1
    add hl,de      // ixl is 2
ixl_is_1:
    add hl,de 
    ld d, a        // ixl
    jp after_second_mul
skip_second_mul:
    ld d, a        // ixl
    ld c, a
    pop af         // (_langle)
    call mul8u_a_c_reg                      // third MUL. Needed to rotate distance into camera view
after_second_mul:
    // not needed anymore:   ld d, ixl 
    ld e, ixh // ixh: tmpl2
    add hl,de 

    ld a, h
#ifdef DO_LARGE_DIST
    cp 15   // if farther than 15 don't bother calculating further. It is quite often.
    jr nc, hl_is_filled // hl now in range: 0...0xFFF  
    srl h
    rr l
#else

    cp 15   // if farther than 15 don't bother calculating further. It is quite often.
    jr nc, hl_is_filled // hl now in range: 0...0xFFF  
    ld a,l
    srl h     // drop 3 bits from distance. our LUT is just 512, not 4096
    rra
    srl h
    rra
    srl h
    rra
    ld l, a    // HL now in range 0...0x1FF
#endif
    /*** Store results **/
    /* HL: distance, 0...512. IYL: x, IYH: bmap address */
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
//    ld hl, 511         // can saturate if looking through all the map...
    jp hl_is_filled_cnt

    
#endasm
}

