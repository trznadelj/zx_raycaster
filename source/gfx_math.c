uint8_t atan_table[256]= {
0, 0, 1, 1, 2, 3, 3, 4, 5, 5, 6, 6, 7, 8, 8, 9, 10, 10, 11, 12, 12, 13, 13, 14, 15, 15, 16, 17, 17, 18, 19, 19, 20, 20,
21, 22, 22, 23, 24, 24, 25, 25, 26, 27, 27, 28, 28, 29, 30, 30, 31, 32, 32, 33, 33, 34, 35, 35, 36, 36, 37, 38, 38, 39,
39, 40, 41, 41, 42, 42, 43, 44, 44, 45, 45, 46, 47, 47, 48, 48, 49, 49, 50, 51, 51, 52, 52, 53, 53, 54, 55, 55, 56, 56,
57, 57, 58, 59, 59, 60, 60, 61, 61, 62, 62, 63, 63, 64, 65, 65, 66, 66, 67, 67, 68, 68, 69, 69, 70, 70, 71, 71, 72, 72,
73, 74, 74, 75, 75, 76, 76, 77, 77, 78, 78, 79, 79, 80, 80, 81, 81, 82, 82, 83, 83, 83, 84, 84, 85, 85, 86, 86, 87, 87,
88, 88, 89, 89, 90, 90, 91, 91, 91, 92, 92, 93, 93, 94, 94, 95, 95, 95, 96, 96, 97, 97, 98, 98, 99, 99, 99, 100, 100, 101, 
101, 101, 102, 102, 103, 103, 104, 104, 104, 105, 105, 106, 106, 106, 107, 107, 108, 108, 108, 109, 109, 110, 110, 110, 
111, 111, 111, 112, 112, 113, 113, 113, 114, 114, 114, 115, 115, 116, 116, 116, 117, 117, 117, 118, 118, 118, 119, 119, 
119, 120, 120, 121, 121, 121, 122, 122, 122, 123, 123, 123, 124, 124, 124, 125, 125, 125, 126, 126, 126, 127, 127, 127 };


	
uint8_t l_table[64]= { 0, 0, 0, 1, 2, 3, 5, 7, 10, 12, 15, 19, 23, 27, 31, 36, 42, 48, 54, 61, 68, 76, 84, 94, 103, 114,
 125, 137, 150, 164, 179, 194, 212, 194, 179, 164, 150, 137, 125, 114, 103, 94, 84, 76, 68, 61, 54, 48, 42, 36, 31, 27,
23, 19, 15, 12, 10, 7, 5, 3, 2, 1, 0, 0  };


//                            UP RIGH RIGH DOWN DOWN LEFT LEFT   UP
//                          RIGH   UP DOWN RIGH LEFT DOWN   UP LEFT
const uint8_t d_array[16] = {  // used by raycast function
240, 1, 1, 240, 1, 16, 16, 1, 16, 255, 255, 16, 255, 240, 240, 255 };

#ifdef DO_HIGH_CTAN_PRECISION
uint8_t ctan_tan_table[66]= { 0, 255, 6, 162, 12, 81, 18, 54, 25, 162, 31, 129, 37, 107, 44, 92, 50, 80, 57, 71, 64, 63,
 70, 57, 77, 52, 84, 48, 91, 44, 98, 41, 106, 154, 113, 144, 121, 135, 128, 127, 136, 119, 145, 112, 153, 106, 162, 101,
 171, 95, 180, 90, 189, 86, 199, 82, 210, 77, 220, 74, 232, 70, 243, 67, 255,  64  };
#else
uint8_t ctan_tan_table[66]= { 0, 255, 6, 162, 12, 81, 18, 54, 25, 162, 31, 129, 37, 107, 44, 92, 50, 80, 57, 71, 64, 63,
 70, 57, 77, 52, 84, 48, 91, 44, 98, 41, 106, 38, 113, 36, 121, 33, 128, 31, 136, 29, 145, 28, 153, 26, 162, 25, 171, 23
, 180, 22, 189, 21, 199, 20, 210, 19, 220, 18, 232, 17, 243, 16, 255, 16  };
#endif

uint8_t tan_table[33] = { 
0, 6, 12, 18, 25, 31, 37, 44, 50, 57, 64, 70, 77, 84, 91, 98, 106, 113, 121, 128, 136, 145, 153, 162, 171, 180, 189, 199, 210, 220, 232, 243, 255  };
//uint8_t ctan_table[33]= { 
//255, 162, 81, 54, 40, 32, 26, 23, 20, 17, 15, 14, 13, 12, 11, 10, 154, 144, 135, 127, 119, 112, 106, 101, 95, 90, 86, 82, 77, 74, 70, 67, 64  };

uint8_t ctan_table[33]= { 255, 162, 81, 54, 162, 129, 107, 92, 80, 71, 63, 57, 52, 48, 44, 41, 154, 144, 135, 127, 119, 112, 106, 101, 95, 90, 86, 82, 77, 74, 70, 67, 64  };
//                                           /4                                                /16
// cc table combines two rotations neccessary at the end:
//   first to calculate the length of a vector having only one triangle edge and angle
//   second to project its length to screen.
//   thus one lookup instead of extra multiplication.
uint8_t cc_table[16*33]= { // 512 * cos(2*a*3.14159265/128) / cos(d*3.14159265/128) /sqrt(2) -256
106, 105, 104, 102, 99, 95, 90, 84, 78, 71, 63, 54, 45, 34, 23, 12,
106, 105, 104, 102, 99, 95, 90, 84, 78, 71, 63, 54, 45, 34, 23, 12,
106, 106, 104, 102, 99, 95, 90, 85, 78, 71, 63, 54, 45, 35, 24, 12,
107, 106, 105, 103, 100, 96, 91, 85, 79, 72, 64, 55, 45, 35, 24, 12,
107, 107, 106, 103, 100, 96, 92, 86, 80, 72, 64, 56, 46, 36, 25, 13,
108, 108, 107, 104, 101, 97, 93, 87, 81, 73, 65, 56, 47, 36, 25, 14,
110, 109, 108, 106, 102, 99, 94, 88, 82, 74, 66, 57, 48, 37, 26, 15,
111, 111, 109, 107, 104, 100, 95, 89, 83, 76, 68, 59, 49, 39, 28, 16,
113, 112, 111, 109, 106, 102, 97, 91, 85, 77, 69, 60, 50, 40, 29, 17,
115, 114, 113, 111, 107, 103, 99, 93, 86, 79, 71, 62, 52, 42, 30, 18,
117, 116, 115, 113, 110, 106, 101, 95, 88, 81, 73, 64, 54, 43, 32, 20,
119, 119, 117, 115, 112, 108, 103, 97, 91, 83, 75, 66, 56, 45, 34, 22,
122, 121, 120, 118, 115, 110, 106, 100, 93, 86, 77, 68, 58, 47, 36, 24,
125, 124, 123, 121, 117, 113, 108, 102, 96, 88, 80, 71, 61, 50, 38, 26,
128, 128, 126, 124, 121, 116, 111, 106, 99, 91, 83, 73, 63, 52, 41, 28,
132, 131, 130, 127, 124, 120, 115, 109, 102, 94, 86, 76, 66, 55, 43, 31,
135, 135, 133, 131, 128, 124, 118, 112, 106, 98, 89, 80, 69, 58, 46, 34,
140, 139, 138, 135, 132, 128, 122, 116, 109, 101, 93, 83, 73, 62, 50, 37,
144, 144, 142, 140, 136, 132, 127, 121, 114, 106, 97, 87, 76, 65, 53, 40,
149, 148, 147, 144, 141, 137, 131, 125, 118, 110, 101, 91, 81, 69, 57, 44,
154, 154, 152, 150, 146, 142, 136, 130, 123, 115, 106, 96, 85, 73, 61, 48,
160, 159, 158, 155, 152, 147, 142, 135, 128, 120, 110, 100, 89, 78, 65, 52,
166, 165, 164, 161, 157, 153, 147, 141, 133, 125, 116, 106, 94, 83, 70, 56,
172, 172, 170, 167, 164, 159, 154, 147, 139, 131, 121, 111, 100, 88, 75, 61,
179, 178, 177, 174, 171, 166, 160, 153, 146, 137, 128, 117, 106, 93, 80, 66,
186, 186, 184, 182, 178, 173, 167, 160, 153, 144, 134, 123, 112, 99, 86, 72,
194, 194, 192, 189, 186, 181, 175, 168, 160, 151, 141, 130, 118, 106, 92, 77,
203, 202, 201, 198, 194, 189, 183, 176, 168, 159, 149, 137, 125, 112, 98, 84,
212, 211, 210, 207, 203, 198, 192, 184, 176, 167, 157, 145, 133, 120, 106, 91,
222, 221, 219, 216, 212, 207, 201, 194, 185, 176, 165, 154, 141, 128, 113, 98,
232, 232, 230, 227, 223, 217, 211, 204, 195, 185, 174, 163, 150, 136, 121, 106,
243, 243, 241, 238, 234, 228, 222, 214, 205, 195, 184, 172, 159, 145, 130, 114,
255, 255, 253, 250, 246, 240, 233, 226, 217, 206, 195, 183, 169, 155, 139, 123,
};

const int8_t sin_table[256] = {
     0,   3,   6,   9,  12,  16,  19,  22,  25,  28,  31,  34,  37,  40,  43,  46,
    49,  51,  54,  57,  60,  63,  65,  68,  71,  73,  76,  78,  81,  83,  85,  88,
    90,  92,  94,  96,  98, 100, 102, 104, 106, 107, 109, 111, 112, 113, 115, 116,
   117, 118, 120, 121, 122, 122, 123, 124, 125, 125, 126, 126, 126, 127, 127, 127,
   127, 127, 127, 127, 126, 126, 126, 125, 125, 124, 123, 122, 122, 121, 120, 118,
   117, 116, 115, 113, 112, 111, 109, 107, 106, 104, 102, 100,  98,  96,  94,  92,
    90,  88,  85,  83,  81,  78,  76,  73,  71,  68,  65,  63,  60,  57,  54,  51,
    49,  46,  43,  40,  37,  34,  31,  28,  25,  22,  19,  16,  12,   9,   6,   3,
     0,  -3,  -6,  -9, -12, -16, -19, -22, -25, -28, -31, -34, -37, -40, -43, -46,
   -49, -51, -54, -57, -60, -63, -65, -68, -71, -73, -76, -78, -81, -83, -85, -88,
   -90, -92, -94, -96, -98,-100,-102,-104,-106,-107,-109,-111,-112,-113,-115,-116,
  -117,-118,-120,-121,-122,-122,-123,-124,-125,-125,-126,-126,-126,-127,-127,-127,
  -127,-127,-127,-127,-126,-126,-126,-125,-125,-124,-123,-122,-122,-121,-120,-118,
  -117,-116,-115,-113,-112,-111,-109,-107,-106,-104,-102,-100, -98, -96, -94, -92,
   -90, -88, -85, -83, -81, -78, -76, -73, -71, -68, -65, -63, -60, -57, -54, -51,
   -49, -46, -43, -40, -37, -34, -31, -28, -25, -22, -19, -16, -12,  -9,  -6,  -3
};


/* Simple 8bit multiplication
   mul8r = (mul8u_x*mul8u_y) >> 8;
   HL = (mul8u_x*mul8u_y)

   Takes around 56 cycles for 1bit. 
   Terminates early if there are not many bits set in mul8u_x

   Alters: HL, BC, A.
 */

/*
    around 26 cycles for 1 bit. 
    full 8bit around 212.5 cycles.
 */
uint8_t mul8u_x, mul8u_y;
uint16_t mul8r;

void mul8u_unrolled()
{
#asm
    /* For C callers only */
    ld a,(_mul8u_y)
    ld c,a
    ld a,(_mul8u_x)
    call mul8u_a_c_reg
    ld (_mul8r), hl
    ld a, h
    ld (_mul8u_x), a
    ret

    /*** Actuall code for assembly use ***/
mul8u_a_c_reg:
    ld h, a
mul8u_h_c_reg:
    ld b,0                             // 7
    sla h                              // 8
    sbc a,a                            // 11
    and c                              // 4
    ld l,a                             // 4  => 34 cycles 11 + (12 + 7+11)/2 => 26 avg
          add hl,hl \ jr nc,mmul1 \ add hl,bc  
mmul1:    add hl,hl \ jr nc,mmul2 \ add hl,bc
mmul2:    add hl,hl \ jr nc,mmul3 \ add hl,bc
mmul3:    add hl,hl \ jr nc,mmul4 \ add hl,bc
mmul4:    add hl,hl \ jr nc,mmul5 \ add hl,bc
mmul5:    add hl,hl \ jr nc,mmul6 \ add hl,bc
mmul6:    add hl,hl \ ret nc \ add hl,bc \ ret // 34 + 26*7 => 182+34 =~= 212.5 cycles.
#endasm
}


uint8_t mul8( uint8_t a, uint8_t b)
{
    mul8u_x = a; mul8u_y = b;
//    mul8u();
    mul8u_unrolled();
    return mul8u_x;
}

uint16_t mul8all( uint8_t a, uint8_t b)
{
    mul8u_x = a; mul8u_y = b;
//    mul8u();
    mul8u_unrolled();
    return mul8r;
}


uint16_t div16x, div16y;
uint8_t div16r; // = 256*div16x/div16y
                // assert: div16y>div16x
void div16_16_8_rest()
{
#asm
#if 0
; div16r = floor( (div16x << 8) / div16y ), with 0 < x < y
; Input:  _div16x (16-bit), _div16y (16-bit)
; Output: _div16r (8-bit)

    ld   hl,(_div16x)          ; HL = remainder
    ld   de,(_div16y)          ; DE = divisor (positive)
    ld   b,8                   ; 8 fractional bits
    xor  a                     ; A = quotient (fractional byte)

div16_16_8_rest_loop:
    add  hl,hl                 ; remainder <<= 1
    or   a                     ; clear C before SBC (Z80 idiom)
    sbc  hl,de                 ; trial subtract: HL = HL - DE
    jr   c,div16_16_8_rest_fail ; if borrow, too big => restore and emit 0
    add  a,a                   ; success: (A<<1)|1
    inc  a
    djnz div16_16_8_rest_loop
    jr   div16_16_8_rest_end

div16_16_8_rest_fail:
    add  a,a                   ; (A<<1)|0
    add  hl,de                 ; restore remainder
    djnz div16_16_8_rest_loop

div16_16_8_rest_end:
    ld   (_div16r),a
#endif
#if 1
; div16r = floor( (div16x << 8) / div16y ), with 0 < x < y
    ld   hl,(_div16x)          ; HL = remainder
    ld   de,(_div16y)          ; DE = divisor
    ld   a,d                  ; negate DE -> DE = -DE
    cpl
    ld   d,a
    ld   a,e
    cpl
    ld   e,a
    inc  de

    ld   b,8                  ; 8 fractional bits
    xor  a
div16_16_8_rest_loop:
    add  hl,hl                ; remainder <<= 1
    add  hl,de                ; remainder += (-divisor)  (trial subtract)
    jp   nc,div16_16_8_rest_fail               ; negative => too big, restore and emit 0
    add  a,a                   ; A = (A<<1) | 1
    inc  a
    djnz div16_16_8_rest_loop
    jp   div16_16_8_rest_end
div16_16_8_rest_fail:
    add  a,a                   ; A = (A<<1) | 0, clear carry flag. x<y is assumed
    sbc  hl,de                 ; restore: remainder -= (-divisor) = +divisor
    djnz div16_16_8_rest_loop
div16_16_8_rest_end:
    ld (_div16r),a
#endif
#endasm


}




int16_t atan_x, atan_y;
uint8_t atanr, atanq, atanx, atan_raw;

/*      y
     \ 7| 0/
    6 \ | / 1
    ----+----> x
    5 / | \ 2
     / 4| 3\
 */

 #if 1
uint8_t atan_rest;
uint8_t int_atan2()
{
    uint16_t t;
    atanq=atanx=0;
    if (atan_x<0) { atan_x=-atan_x; atanq^=0xE0; atanx^=31; }
    if (atan_y<0) { atan_y=-atan_y; atanq^=0x60; atanx^=31; }
    if (atan_x>atan_y) { t=atan_x; atan_x = atan_y; atan_y = t; atanq^=0x20; atanx^=31;} else
    if (atan_x==atan_y) return atanq|32;
    div16x = atan_x; div16y = atan_y;
    div16_16_8_rest();
//    printf("%c%c%cdiv16r=%u  ", 22, 32+22, 32, div16r);
    atan_raw = atan_table[div16r];
//return atan_raw;
    atan_rest =3&(atan_raw^ (atanx&3)); // I will die in hell for this trick...

    return atanx^((atan_raw>>2) | atanq);
}
#endif

#if 0
uint8_t int_atan2()
{
    uint16_t t;
    uint8_t base_angle;

    // Handle zero cases
    if (atan_x == 0 && atan_y == 0) return 0;
    if (atan_y == 0) return (atan_x < 0) ? 192 : 64; // ±90°

    uint8_t quadrant = 0;

    // Make atan_x positive
    if (atan_x < 0) {
        atan_x = -atan_x;
        quadrant ^= 0x80; // X negative
    }

    // Make atan_y positive
    if (atan_y < 0) {
        atan_y = -atan_y;
        quadrant ^= 0x40; // Y negative
    }

    // Swap if slope > 1
    if (atan_x > atan_y) {
        t = atan_x; atan_x = atan_y; atan_y = t;
        quadrant ^= 0x20;
    }

    // Do fixed-point division: (atan_x << 8) / atan_y
    div16x = atan_x;
    div16y = atan_y;
    div16_16_8_rest();

    // Lookup in table
    base_angle = atan_table[div16r];

    // Combine with quadrant
    return base_angle | quadrant;
}
#endif
 #if 0

int8_t ax, ay, ar;
uint8_t atan_internet()
{
#asm
 ld a, (_ax)
 ld b, a
 ld a, (_ay)
 ld c, a

fatan2:	
		ld	de,0x8000			
		
		ld	a,c
		add	a,d
		rl	e				; y-					
		
		ld	a,b
		add	a,d
		rl	e				; x-					
		
		dec	e
		jp	z,q1
		dec	e
		jp	z,q2
		dec	e
		jp	z,q3
		
q0:			

		ld	h,log2_tab / 256
		ld	l,b
		
		ld	a,(hl)			; 32*log2(x)
		ld	l,c
		
		sub	a,(hl)			; 32*log2(x/y)
		
		jr	nc,f1f			; |x|>|y|

		neg				; |x|<|y|	A = 32*log2(y/x)
f1f:
  	        ld	l,a

		ld	h, atan_tab / 256

		ld	a,(hl)
		ret	c			; |x|<|y|

		neg
		and	0x3F			; |x|>|y|
		ret

q1:		ld	a,b
		neg
		ld	b,a
		call	q0
		neg
		and	0x7F
		ret
		
q2:		ld	a,c
		neg
		ld	c,a
		call	q0
		neg
		ret		
		
q3:		ld	a,b
		neg
		ld	b,a
		ld	a,c
		neg
		ld	c,a
		call	q0
		add	a,128
               ld (_ar),a
		ret


		; align to byte
		
		align 0x100
		
//		;;;;;;;; atan(2^(x/32))*128/pi ;;;;;;;;
atan_tab:	
  	        db 0x20,0x20,0x20,0x21,0x21,0x22,0x22,0x23,0x23,0x23,0x24,0x24,0x25,0x25,0x26,0x26
		db 0x26,0x27,0x27,0x28,0x28,0x28,0x29,0x29,0x2A,0x2A,0x2A,0x2B,0x2B,0x2C,0x2C,0x2C
		db 0x2D,0x2D,0x2D,0x2E,0x2E,0x2E,0x2F,0x2F,0x2F,0x30,0x30,0x30,0x31,0x31,0x31,0x31
		db 0x32,0x32,0x32,0x32,0x33,0x33,0x33,0x33,0x34,0x34,0x34,0x34,0x35,0x35,0x35,0x35
		db 0x36,0x36,0x36,0x36,0x36,0x37,0x37,0x37,0x37,0x37,0x37,0x38,0x38,0x38,0x38,0x38
		db 0x38,0x39,0x39,0x39,0x39,0x39,0x39,0x39,0x39,0x3A,0x3A,0x3A,0x3A,0x3A,0x3A,0x3A
		db 0x3A,0x3B,0x3B,0x3B,0x3B,0x3B,0x3B,0x3B,0x3B,0x3B,0x3B,0x3B,0x3C,0x3C,0x3C,0x3C
		db 0x3C,0x3C,0x3C,0x3C,0x3C,0x3C,0x3C,0x3C,0x3C,0x3D,0x3D,0x3D,0x3D,0x3D,0x3D,0x3D
		db 0x3D,0x3D,0x3D,0x3D,0x3D,0x3D,0x3D,0x3D,0x3D,0x3D,0x3D,0x3D,0x3E,0x3E,0x3E,0x3E
		db 0x3E,0x3E,0x3E,0x3E,0x3E,0x3E,0x3E,0x3E,0x3E,0x3E,0x3E,0x3E,0x3E,0x3E,0x3E,0x3E
		db 0x3E,0x3E,0x3E,0x3E,0x3E,0x3E,0x3E,0x3E,0x3E,0x3E,0x3E,0x3E,0x3F,0x3F,0x3F,0x3F
		db 0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F
		db 0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F
		db 0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F
		db 0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F
		db 0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F
 
log2_tab:  
		db 0x00,0x00,0x20,0x32,0x40,0x4A,0x52,0x59,0x60,0x65,0x6A,0x6E,0x72,0x76,0x79,0x7D
		db 0x80,0x82,0x85,0x87,0x8A,0x8C,0x8E,0x90,0x92,0x94,0x96,0x98,0x99,0x9B,0x9D,0x9E
		db 0xA0,0xA1,0xA2,0xA4,0xA5,0xA6,0xA7,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,0xB0,0xB1
		db 0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xB9,0xBA,0xBB,0xBC,0xBD,0xBD,0xBE,0xBF
		db 0xC0,0xC0,0xC1,0xC2,0xC2,0xC3,0xC4,0xC4,0xC5,0xC6,0xC6,0xC7,0xC7,0xC8,0xC9,0xC9
		db 0xCA,0xCA,0xCB,0xCC,0xCC,0xCD,0xCD,0xCE,0xCE,0xCF,0xCF,0xD0,0xD0,0xD1,0xD1,0xD2
		db 0xD2,0xD3,0xD3,0xD4,0xD4,0xD5,0xD5,0xD5,0xD6,0xD6,0xD7,0xD7,0xD8,0xD8,0xD9,0xD9
		db 0xD9,0xDA,0xDA,0xDB,0xDB,0xDB,0xDC,0xDC,0xDD,0xDD,0xDD,0xDE,0xDE,0xDE,0xDF,0xDF
		db 0xDF,0xE0,0xE0,0xE1,0xE1,0xE1,0xE2,0xE2,0xE2,0xE3,0xE3,0xE3,0xE4,0xE4,0xE4,0xE5
		db 0xE5,0xE5,0xE6,0xE6,0xE6,0xE7,0xE7,0xE7,0xE7,0xE8,0xE8,0xE8,0xE9,0xE9,0xE9,0xEA
		db 0xEA,0xEA,0xEA,0xEB,0xEB,0xEB,0xEC,0xEC,0xEC,0xEC,0xED,0xED,0xED,0xED,0xEE,0xEE
		db 0xEE,0xEE,0xEF,0xEF,0xEF,0xEF,0xF0,0xF0,0xF0,0xF1,0xF1,0xF1,0xF1,0xF1,0xF2,0xF2
		db 0xF2,0xF2,0xF3,0xF3,0xF3,0xF3,0xF4,0xF4,0xF4,0xF4,0xF5,0xF5,0xF5,0xF5,0xF5,0xF6
		db 0xF6,0xF6,0xF6,0xF7,0xF7,0xF7,0xF7,0xF7,0xF8,0xF8,0xF8,0xF8,0xF9,0xF9,0xF9,0xF9
		db 0xF9,0xFA,0xFA,0xFA,0xFA,0xFA,0xFB,0xFB,0xFB,0xFB,0xFB,0xFC,0xFC,0xFC,0xFC,0xFC
		db 0xFD,0xFD,0xFD,0xFD,0xFD,0xFD,0xFE,0xFE,0xFE,0xFE,0xFE,0xFF,0xFF,0xFF,0xFF,0xFF

#endasm
}
#endif