#include <im2.h>

uint8_t get_qweasdu_result;
uint8_t get_qweasdu()
{       

#asm
; get_keys_qweasdu
; Reads ZX Spectrum keyboard for q,w,e,a,s,d,u (simultaneous allowed).
; Return:  A = bitmask (bit0=q,1=w,2=e,3=a,4=s,5=d,6=u, bit7=0)
; Clobbers: AF, BC (E used internally). Preserves all others.
; Port: 0xFE (ULA). Active-low keys -> we invert & mask.

get_keys_qweasdu:
    push bc
    ld   c, $FE          ; ULA keyboard port
    ld   e, 0            ; E = result accumulator

    ; --- Row 2 (A10 low): Q W E R T -> want Q,W,E in bits 0..2
    ld   b, %11111011    ; B = 0xFB : select row 2
    in   a, (c)          ; read row
    cpl                   ; active-low -> active-high
    and  %00000111       ; keep Q,W,E (d0..d2)
    ld   e, a            ; E = qwe---

    ; --- Row 1 (A9 low): A S D F G -> want A,S,D into bits 3..5
    ld   b, %11111101    ; B = 0xFD : select row 1
    in   a, (c)
    cpl
    and  %00000111       ; a,s,d in d0..d2
    rlca                  ; shift left 3 -> bits 3..5 (upper bits were 0)
    rlca
    rlca
    or   e               ; merge with qwe
    ld   e, a

    ; --- Row 5 (A13 low): P O I U Y -> want U (d3) into bit 6
    ld   b, %11011111    ; B = 0xDF : select row 5
    in   a, (c)
    cpl
    and  %00011000       ; isolate U (d3), Y (d4)
    rlca                  ; d3 -> d4
    rlca                  ; d4 -> d5
    rlca                  ; d5 -> d6
    or   e               ; merge into result

//    and  %01111111       ; ensure bit7 is 0
    pop  bc
    ld (_get_qweasdu_result), a
#endasm                                             
    return get_qweasdu_result;
}



uint16_t set_interrupt_counter=0;
uint16_t set_interrupt_area[2];
void interrupt_handler()
{
#asm
interrupt_handler:

    ld (_set_interrupt_area), sp      // set_interrupt_area: store SP, AF
    ld sp, _set_interrupt_area+4  // need to store F register - inc will alter it.
    push af                       // will write to _set_interrupt_area+2
    ld sp, (_set_interrupt_counter)
    inc sp
    ld (_set_interrupt_counter), sp
    ld sp, _set_interrupt_area+2
    pop af                          
    ld sp, (_set_interrupt_area)    
    ei
    reti
#endasm
}

void set_interrupt()
{
    memset(INTMAP_ADDR, INTMAP_ROUTINE_SEG, 257);     // initialize 257-byte im2 vector table with all same bytes
    bpoke(INTMAP_ROUTINE_ADDR, 195);                  // POKE jump instruction (interrupt service routine entry)
    wpoke(INTMAP_ROUTINE_ADDR+1, interrupt_handler);  // POKE isr address following the jump instruction
#asm
    di
#endasm
    im2_Init(INTMAP_ADDR);                            // place z80 in im2 mode with interrupt vector table 
#asm
    ei
#endasm
}

#define get_clock()  (set_interrupt_counter)
