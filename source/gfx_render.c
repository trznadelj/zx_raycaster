uint8_t vline_x, vline_d, vline_db, vline_c, vline_c2;


void vline()
{
#asm
setup_a2hl_inc:
    ld hl,a2hl_plusde_8times
    add hl,bc
    add hl,bc
    ret

a2hl_plusde_8times:  // fastest possible fillout, right?
    ld (hl), a
    inc h
    ld (hl), a
    inc h
    ld (hl), a
    inc h
    ld (hl), a
    inc h
    ld (hl), a
    inc h
    ld (hl), a
    inc h
    ld (hl), a
    inc h
    ld (hl), a
    inc h
    cpl//xor a,0xFF
    ret
#endasm
}



void vline3()
{
#asm
    ld a, (_vline_d)
    ld iyh, a
    ld a, (_vline_x)
    ld iyl, a
    ld a, (_vline_c)
    ld c,a

vline3_start:
    /*** set return to vline_r5 ***/
    ld hl, vline3_r5
    push hl

    /*** Calculate number of filled ones 0...80 transaltes to 0...10, multiplied by 64 */
    ld a, iyh // (_vline_d)
    add a,4
    rra
    rra
    rra
    and 0x1F
    ld l, 0
    rrca
    rr l
    rrca
    rr l
    and 3
    ld h, a
    ld de, froll0
    add hl,de
    push hl           // trick. will use ret to jump

    /*** Set HL ***/
    ld h, 0x58
    ld a, iyl         //(_vline_x)
    add 0x40
    ld l, a

    /*** Calculate colors. C does contain main color */
    ld a, c
    ld ixh, a // bottom edge. black background.
    or 0x38
    ld ixl,  a // top edge. black background.
    ld a, c
    rla
    rla
    rla
    or c
    ld c, a           // middle in c: color replicated
    ld a, c
    ld a, 0x3F        // top in a: white background 
    ld b, 0           // bottom in b: black
    ret
vline3_r5:
    /*** Calculate fillout length ***/
    ld a,iyh //(_vline_d)  // c=(MIDDLE_LINE-vline_d)&7
    sub MIDDLE_LINE
    neg
    ld e, a
    and 7

    /*** Calculate function address. filler0 + 18*a ***/
    ld d, a
    // a*=20
    rla
    and 0xFC
    ld c, a
    rla
    rla
    rla
    and 0xF0
    add c
    ld c, a
    ld b, 0
    ld hl, filler0
    add hl, bc
    push hl

    // p = 18*7-18*a = 126 - 18*a
    ld a, 126
    sub c
    ld c, a
    ld hl, filler0
    add hl, bc
    push hl

    /*** Y calculation ***/
    ld   c, iyl //(_vline_x)
    ld   a, e
    add  a, 8
    rra                   
    rra
    and 0x3E
//ld b, a
    add YTABLE_REV_OFFSET
    ld h, YTABLE_REV_SEG
    ld l, a
    ld a, (hl)
    add c
    ld e, a
    inc l 
    ld d, (hl)


    ld a,iyh // (_vline_d)          
    add MIDDLE_LINE+9//1
    rra                   
    rra
    and 0x3E
//ld a, 46
//sub b
    add YTABLE_REV_OFFSET
    ld l, a
    ld a, (hl)
    add c
    inc l 
    ld h, (hl)
    ld l, a

    ld   a, 0xFF//0xCC
    ret

froll0:
 ld de, 0xFE20
 ld (hl), a  /* 0*/   \ inc h
 ld (hl), a  /* 8*/   \ inc h
 ld (hl), b  /*16*/   \ add hl,de
 ld (hl), a  /* 1*/   \ inc h
 ld (hl), a  /* 9*/   \ inc h
 ld (hl), b  /*17*/   \ add hl,de
 ld (hl), a  /* 2*/   \ inc h
 ex af, af \ ld a, ixl  \ ld (hl), a  /*10*/ \ ex af, af  \ inc h
 ld (hl), b  /*18*/   \ add hl,de
 ld (hl), a  /* 3*/   \ inc h
 ld (hl), b  /*11*/   \ inc h
 ld (hl), b  /*19*/   \ add hl,de
 ld (hl), a  /* 4*/   \ inc h
 ld (hl), b  /*12*/   \ inc h
 ld (hl), b  /*20*/   \ add hl,de
 ld (hl), a  /* 5*/   \ inc h
 ld (hl), b  /*13*/   \ add hl,de  \ inc h
 ld (hl), a  /* 6*/   \ inc h
 ld (hl), b  /*14*/   \ add hl,de  \ inc h
 ld (hl), a  /* 7*/   \ inc h
 ld (hl), b  /*15*/
 ret  \ nop  \ nop  \ nop  \ nop  \ nop  \ nop  \ nop  \ nop  \ nop  \ nop  \ nop  \ nop  \ nop
froll1:
 ld de, 0xFE20
 ld (hl), a  /* 0*/   \ inc h
 ld (hl), a  /* 8*/   \ inc h
 ld (hl), b  /*16*/   \ add hl,de
 ld (hl), a  /* 1*/   \ inc h
 ex af, af \ ld a, ixl  \ ld (hl), a  /* 9*/ \ ex af, af  \ inc h
 ld (hl), b  /*17*/   \ add hl,de
 ld (hl), a  /* 2*/   \ inc h
 ld (hl), c  /*10*/   \ inc h
 ld (hl), b  /*18*/   \ add hl,de
 ld (hl), a  /* 3*/   \ inc h
 ex af, af \ ld a, ixh \ ld (hl), a  /*11*/ \ ex af, af  \ inc h
 ld (hl), b  /*19*/   \ add hl,de
 ld (hl), a  /* 4*/   \ inc h
 ld (hl), b  /*12*/   \ inc h
 ld (hl), b  /*20*/   \ add hl,de
 ld (hl), a  /* 5*/   \ inc h
 ld (hl), b  /*13*/   \ add hl,de  \ inc h
 ld (hl), a  /* 6*/   \ inc h
 ld (hl), b  /*14*/   \ add hl,de  \ inc h
 ld (hl), a  /* 7*/   \ inc h
 ld (hl), b  /*15*/
 ret  \ nop  \ nop  \ nop  \ nop  \ nop  \ nop  \ nop  \ nop  \ nop
froll2:
 ld de, 0xFE20
 ld (hl), a  /* 0*/   \ inc h
 ex af, af \ ld a, ixl  \ ld (hl), a  /* 8*/ \ ex af, af  \ inc h
 ld (hl), b  /*16*/   \ add hl,de
 ld (hl), a  /* 1*/   \ inc h
 ld (hl), c  /* 9*/   \ inc h
 ld (hl), b  /*17*/   \ add hl,de
 ld (hl), a  /* 2*/   \ inc h
 ld (hl), c  /*10*/   \ inc h
 ld (hl), b  /*18*/   \ add hl,de
 ld (hl), a  /* 3*/   \ inc h
 ld (hl), c  /*11*/   \ inc h
 ld (hl), b  /*19*/   \ add hl,de
 ld (hl), a  /* 4*/   \ inc h
 ex af, af \ ld a, ixh \ ld (hl), a  /*12*/ \ ex af, af  \ inc h
 ld (hl), b  /*20*/   \ add hl,de
 ld (hl), a  /* 5*/   \ inc h
 ld (hl), b  /*13*/   \ add hl,de  \ inc h
 ld (hl), a  /* 6*/   \ inc h
 ld (hl), b  /*14*/   \ add hl,de  \ inc h
 ld (hl), a  /* 7*/   \ inc h
 ld (hl), b  /*15*/
 ret  \ nop  \ nop  \ nop  \ nop  \ nop  \ nop  \ nop  \ nop  \ nop
froll3:
 ld de, 0xFE20
 ld (hl), a  /* 0*/   \ inc h
 ld (hl), c  /* 8*/   \ inc h
 ld (hl), b  /*16*/   \ add hl,de
 ld (hl), a  /* 1*/   \ inc h
 ld (hl), c  /* 9*/   \ inc h
 ld (hl), b  /*17*/   \ add hl,de
 ld (hl), a  /* 2*/   \ inc h
 ld (hl), c  /*10*/   \ inc h
 ld (hl), b  /*18*/   \ add hl,de
 ld (hl), a  /* 3*/   \ inc h
 ld (hl), c  /*11*/   \ inc h
 ld (hl), b  /*19*/   \ add hl,de
 ld (hl), a  /* 4*/   \ inc h
 ld (hl), c  /*12*/   \ inc h
 ld (hl), b  /*20*/   \ add hl,de
 ld (hl), a  /* 5*/   \ inc h
 ex af, af \ ld a, ixh \ ld (hl), a  /*13*/ \ ex af, af  \ add hl,de  \ inc h
 ld (hl), a  /* 6*/   \ inc h
 ld (hl), b  /*14*/   \ add hl,de  \ inc h
 ex af, af \ ld a, ixl  \ ld (hl), a  /* 7*/ \ ex af, af  \ inc h
 ld (hl), b  /*15*/
 ret  \ nop  \ nop  \ nop  \ nop  \ nop  \ nop  \ nop  \ nop  \ nop
froll4:
 ld de, 0xFE20
 ld (hl), a  /* 0*/   \ inc h
 ld (hl), c  /* 8*/   \ inc h
 ld (hl), b  /*16*/   \ add hl,de
 ld (hl), a  /* 1*/   \ inc h
 ld (hl), c  /* 9*/   \ inc h
 ld (hl), b  /*17*/   \ add hl,de
 ld (hl), a  /* 2*/   \ inc h
 ld (hl), c  /*10*/   \ inc h
 ld (hl), b  /*18*/   \ add hl,de
 ld (hl), a  /* 3*/   \ inc h
 ld (hl), c  /*11*/   \ inc h
 ld (hl), b  /*19*/   \ add hl,de
 ld (hl), a  /* 4*/   \ inc h
 ld (hl), c  /*12*/   \ inc h
 ld (hl), b  /*20*/   \ add hl,de
 ld (hl), a  /* 5*/   \ inc h
 ld (hl), c  /*13*/   \ add hl,de  \ inc h
 ex af, af \ ld a, ixl  \ ld (hl), a  /* 6*/ \ ex af, af  \ inc h
 ex af, af \ ld a, ixh \ ld (hl), a  /*14*/ \ ex af, af  \ add hl,de  \ inc h
 ld (hl), c  /* 7*/   \ inc h
 ld (hl), b  /*15*/
 ret  \ nop  \ nop  \ nop  \ nop  \ nop  \ nop  \ nop  \ nop  \ nop
froll5:
 ld de, 0xFE20
 ld (hl), a  /* 0*/   \ inc h
 ld (hl), c  /* 8*/   \ inc h
 ld (hl), b  /*16*/   \ add hl,de
 ld (hl), a  /* 1*/   \ inc h
 ld (hl), c  /* 9*/   \ inc h
 ld (hl), b  /*17*/   \ add hl,de
 ld (hl), a  /* 2*/   \ inc h
 ld (hl), c  /*10*/   \ inc h
 ld (hl), b  /*18*/   \ add hl,de
 ld (hl), a  /* 3*/   \ inc h
 ld (hl), c  /*11*/   \ inc h
 ld (hl), b  /*19*/   \ add hl,de
 ld (hl), a  /* 4*/   \ inc h
 ld (hl), c  /*12*/   \ inc h
 ld (hl), b  /*20*/   \ add hl,de
 ex af, af \ ld a, ixl  \ ld (hl), a  /* 5*/ \ ex af, af  \ inc h
 ld (hl), c  /*13*/   \ add hl,de  \ inc h
 ld (hl), c  /* 6*/   \ inc h
 ld (hl), c  /*14*/   \ add hl,de  \ inc h
 ld (hl), c  /* 7*/   \ inc h
 ex af, af \ ld a, ixh \ ld (hl), a  /*15*/ \ ex af, af
 ret  \ nop  \ nop  \ nop  \ nop  \ nop  \ nop  \ nop  \ nop  \ nop
froll6:
 ld de, 0xFE20
 ld (hl), a  /* 0*/   \ inc h
 ld (hl), c  /* 8*/   \ inc h
 ex af, af \ ld a, ixh \ ld (hl), a  /*16*/ \ ex af, af  \ add hl,de
 ld (hl), a  /* 1*/   \ inc h
 ld (hl), c  /* 9*/   \ inc h
 ld (hl), b  /*17*/   \ add hl,de
 ld (hl), a  /* 2*/   \ inc h
 ld (hl), c  /*10*/   \ inc h
 ld (hl), b  /*18*/   \ add hl,de
 ld (hl), a  /* 3*/   \ inc h
 ld (hl), c  /*11*/   \ inc h
 ld (hl), b  /*19*/   \ add hl,de
 ex af, af \ ld a, ixl  \ ld (hl), a  /* 4*/ \ ex af, af  \ inc h
 ld (hl), c  /*12*/   \ inc h
 ld (hl), b  /*20*/   \ add hl,de
 ld (hl), c  /* 5*/   \ inc h
 ld (hl), c  /*13*/   \ add hl,de  \ inc h
 ld (hl), c  /* 6*/   \ inc h
 ld (hl), c  /*14*/   \ add hl,de  \ inc h
 ld (hl), c  /* 7*/   \ inc h
 ld (hl), c  /*15*/
 ret  \ nop  \ nop  \ nop  \ nop  \ nop  \ nop  \ nop  \ nop  \ nop
froll7:
 ld de, 0xFE20
 ld (hl), a  /* 0*/   \ inc h
 ld (hl), c  /* 8*/   \ inc h
 ld (hl), c  /*16*/   \ add hl,de
 ld (hl), a  /* 1*/   \ inc h
 ld (hl), c  /* 9*/   \ inc h
 ex af, af \ ld a, ixh \ ld (hl), a  /*17*/ \ ex af, af  \ add hl,de
 ld (hl), a  /* 2*/   \ inc h
 ld (hl), c  /*10*/   \ inc h
 ld (hl), b  /*18*/   \ add hl,de
 ex af, af \ ld a, ixl  \ ld (hl), a  /* 3*/ \ ex af, af  \ inc h
 ld (hl), c  /*11*/   \ inc h
 ld (hl), b  /*19*/   \ add hl,de
 ld (hl), c  /* 4*/   \ inc h
 ld (hl), c  /*12*/   \ inc h
 ld (hl), b  /*20*/   \ add hl,de
 ld (hl), c  /* 5*/   \ inc h
 ld (hl), c  /*13*/   \ add hl,de  \ inc h
 ld (hl), c  /* 6*/   \ inc h
 ld (hl), c  /*14*/   \ add hl,de  \ inc h
 ld (hl), c  /* 7*/   \ inc h
 ld (hl), c  /*15*/
 ret  \ nop  \ nop  \ nop  \ nop  \ nop  \ nop  \ nop  \ nop  \ nop
froll8:
 ld de, 0xFE20
 ld (hl), a  /* 0*/   \ inc h
 ld (hl), c  /* 8*/   \ inc h
 ld (hl), c  /*16*/   \ add hl,de
 ld (hl), a  /* 1*/   \ inc h
 ld (hl), c  /* 9*/   \ inc h
 ld (hl), c  /*17*/   \ add hl,de
 ex af, af \ ld a, ixl  \ ld (hl), a  /* 2*/ \ ex af, af  \ inc h
 ld (hl), c  /*10*/   \ inc h
 ex af, af \ ld a, ixh \ ld (hl), a  /*18*/ \ ex af, af  \ add hl,de
 ld (hl), c  /* 3*/   \ inc h
 ld (hl), c  /*11*/   \ inc h
 ld (hl), b  /*19*/   \ add hl,de
 ld (hl), c  /* 4*/   \ inc h
 ld (hl), c  /*12*/   \ inc h
 ld (hl), b  /*20*/   \ add hl,de
 ld (hl), c  /* 5*/   \ inc h
 ld (hl), c  /*13*/   \ add hl,de  \ inc h
 ld (hl), c  /* 6*/   \ inc h
 ld (hl), c  /*14*/   \ add hl,de  \ inc h
 ld (hl), c  /* 7*/   \ inc h
 ld (hl), c  /*15*/
 ret  \ nop  \ nop  \ nop  \ nop  \ nop  \ nop  \ nop  \ nop  \ nop
froll9:
 ld de, 0xFE20
 ld (hl), a  /* 0*/   \ inc h
 ld (hl), c  /* 8*/   \ inc h
 ld (hl), c  /*16*/   \ add hl,de
 ex af, af \ ld a, ixl  \ ld (hl), a  /* 1*/ \ ex af, af  \ inc h
 ld (hl), c  /* 9*/   \ inc h
 ld (hl), c  /*17*/   \ add hl,de
 ld (hl), c  /* 2*/   \ inc h
 ld (hl), c  /*10*/   \ inc h
 ld (hl), c  /*18*/   \ add hl,de
 ld (hl), c  /* 3*/   \ inc h
 ld (hl), c  /*11*/   \ inc h
 ex af, af \ ld a, ixh \ ld (hl), a  /*19*/ \ ex af, af  \ add hl,de
 ld (hl), c  /* 4*/   \ inc h
 ld (hl), c  /*12*/   \ inc h
 ld (hl), b  /*20*/   \ add hl,de
 ld (hl), c  /* 5*/   \ inc h
 ld (hl), c  /*13*/   \ add hl,de  \ inc h
 ld (hl), c  /* 6*/   \ inc h
 ld (hl), c  /*14*/   \ add hl,de  \ inc h
 ld (hl), c  /* 7*/   \ inc h
 ld (hl), c  /*15*/
 ret  \ nop  \ nop  \ nop  \ nop  \ nop  \ nop  \ nop  \ nop  \ nop
froll10:
 ld de, 0xFE20
 ex af, af \ ld a, ixl  \ ld (hl), a  /* 0*/ \ ex af, af  \ inc h
 ld (hl), c  /* 8*/   \ inc h
 ld (hl), c  /*16*/   \ add hl,de
 ld (hl), c  /* 1*/   \ inc h
 ld (hl), c  /* 9*/   \ inc h
 ld (hl), c  /*17*/   \ add hl,de
 ld (hl), c  /* 2*/   \ inc h
 ld (hl), c  /*10*/   \ inc h
 ld (hl), c  /*18*/   \ add hl,de
 ld (hl), c  /* 3*/   \ inc h
 ld (hl), c  /*11*/   \ inc h
 ld (hl), c  /*19*/   \ add hl,de
 ld (hl), c  /* 4*/   \ inc h
 ld (hl), c  /*12*/   \ inc h
 ex af, af \ ld a, ixh \ ld (hl), a  /*20*/ \ ex af, af  \ add hl,de
 ld (hl), c  /* 5*/   \ inc h
 ld (hl), c  /*13*/   \ add hl,de  \ inc h
 ld (hl), c  /* 6*/   \ inc h
 ld (hl), c  /*14*/   \ add hl,de  \ inc h
 ld (hl), c  /* 7*/   \ inc h
 ld (hl), c  /*15*/
 ret  

filler0:
   cpl
   ld (hl), a
   inc h
   ld (hl), a
   inc h
   ld (hl), a
   inc h
   ld (hl), a
   inc h
   ld (hl), a
   inc h
   ld (hl), a
   inc h
   ld (hl), a
   inc h
   ld (hl), a
   ex de,hl
   ret
filler1:
   ld (hl), a
   inc h
   cpl
   ld (hl), a
   inc h
   ld (hl), a
   inc h
   ld (hl), a
   inc h
   ld (hl), a
   inc h
   ld (hl), a
   inc h
   ld (hl), a
   inc h
   ld (hl), a
   ex de,hl
   ret
filler2:
   ld (hl), a
   inc h
   ld (hl), a
   inc h
   cpl
   ld (hl), a
   inc h
   ld (hl), a
   inc h
   ld (hl), a
   inc h
   ld (hl), a
   inc h
   ld (hl), a
   inc h
   ld (hl), a
   ex de,hl
   ret
filler3:
   ld (hl), a
   inc h
   ld (hl), a
   inc h
   ld (hl), a
   inc h
   cpl
   ld (hl), a
   inc h
   ld (hl), a
   inc h
   ld (hl), a
   inc h
   ld (hl), a
   inc h
   ld (hl), a
   ex de,hl
   ret
filler4:
   ld (hl), a
   inc h
   ld (hl), a
   inc h
   ld (hl), a
   inc h
   ld (hl), a
   inc h
   cpl
   ld (hl), a
   inc h
   ld (hl), a
   inc h
   ld (hl), a
   inc h
   ld (hl), a
   ex de,hl
   ret
filler5:
   ld (hl), a
   inc h
   ld (hl), a
   inc h
   ld (hl), a
   inc h
   ld (hl), a
   inc h
   ld (hl), a
   inc h
   cpl
   ld (hl), a
   inc h
   ld (hl), a
   inc h
   ld (hl), a
   ex de,hl
   ret
filler6:
   ld (hl), a
   inc h
   ld (hl), a
   inc h
   ld (hl), a
   inc h
   ld (hl), a
   inc h
   ld (hl), a
   inc h
   ld (hl), a
   inc h
   cpl
   ld (hl), a
   inc h
   ld (hl), a
   ex de,hl
   ret
filler7:
   ld (hl), a
   inc h
   ld (hl), a
   inc h
   ld (hl), a
   inc h
   ld (hl), a
   inc h
   ld (hl), a
   inc h
   ld (hl), a
   inc h
   ld (hl), a
   inc h
   cpl
   ld (hl), a
   ex de,hl
   ret
filler8:
   ld (hl), a
   inc h
   ld (hl), a
   inc h
   ld (hl), a
   inc h
   ld (hl), a
   inc h
   ld (hl), a
   inc h
   ld (hl), a
   inc h
   ld (hl), a
   inc h
   ld (hl), a
   cpl
   ex de,hl
   ret


#endasm

}


void renderv_full()
{
    for( uint8_t x=0; x<32; x++)
    {
        vline_x = x;
        vline_d = Ybuf[x];
        vline_db = Ybuf[x];
        vline_c = Ybuf_c[x];
//        if ((Ybuf_o[x]==vline_d)&&(Ybuf_co[x]==Ybuf_c[x])) continue;
        Ybuf_o[x] = vline_d;
        Ybuf_co[x] = vline_c;
        vline3();
    }
}


void renderv()
{
// uint8_t Ybuf[32], Ybuf_c[32], Ybuf_i[32], Ybuf_s[32], Ybuf_o[32], Ybuf_co[32];;
#asm
    xor a
    ld iyl, a
    ld h, YBUF_SEG
    jp renderv_loop_inside
renderv_loop:
    inc iyl
    ld a, iyl
    cp 32
    ret nc
renderv_loop_inside:
    ld l, a
    ld b, (hl)
    set 5, l
    ld c, (hl)                  // hl: 0x20
    res 5, l
    set 7, l
    ld a, (hl)
    cp b                       // b: Ybuf, a: Ybuf_o
    jr nz,call_vline_no_color  // changed length, unsure about color
    set 5, l
    ld a, (hl)
    cp c
    jr z,renderv_loop          // if color not changed as well (likely!) - repeat.
call_vline:
    ld (hl), c                 // Ybuf_co = c: current color
    ld iyh, b
    call vline3_start
    ld h, YBUF_SEG
    jp renderv_loop
call_vline_no_color:
    ld (hl), b
    set 5, l

    add 4
    ld d, a
    ld a, b
    add 4
    xor d
    and 0xF8 // check if more significant bits were changed
    jr nz,call_vline

    ld a, (hl)
    cp c
    jr nz,call_vline
    ld iyh, b
    call vline3_r5
    ld h, YBUF_SEG
    jp renderv_loop
#endasm
}
