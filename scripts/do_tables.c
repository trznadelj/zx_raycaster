#include <stdint.h>
#include <stdio.h>
#include <math.h>

uint8_t T[16*128];

int8_t sin_table[256];

void main()
{
    int i, v;

    printf("int8_t sin_table[256]= { ");
    for(  i=0; i<256; i++) 
    {
        int v = sin(i*3.14159265/128)*128;
        if (v>127) v=127;  
        sin_table[i] = v;
        printf("%i%c ", v, i<255?',':' ');
    }
    printf("};\n");

    printf("int8_t d_table[512]= { ");
    for( uint16_t d = 0; d<512; d++) 
    {
        uint16_t n = 65535;
        if (d)
        n/=d; n>>=5;
        if (n<1) n=1;
        if (n>80) n=80;
        printf("%i, ", (int)n);
        if (d%16==0) printf("\n");
    }

    printf("};\n");

    printf("uint8_t l_table[64]= { ");
    for(  i=0; i<64; i++) 
    {
        int j = i<32?i:64-i;
        int v = 512/cos(j*3.14159265/128)-512;
        printf("%i%c ", v, i<95?',':' ');
    }
    printf("};\n");

    printf("uint8_t tan_table[33]= { ");
    for(  i=0; i<33; i++) 
    {
        int v = tan(i*3.14159265/128)*256;
        printf("%i%c ", v, i<32?',':' ');
    }
    printf("};\n");

    printf("uint8_t ctan_table[33]= { ");
    for(  i=0; i<33; i++) 
    {

        float f = 4/tan(i*3.14159265/128);
        if (i==0) f=255;
        if (i>3) f*=4;
        if (i>15) f*=4;
        v=f;
        printf("%i%c ", v, i<32?',':' ');
    }
    printf("};\n");

    printf("uint8_t ctan_tan_table[66]= { ");
    for(  i=0; i<33; i++) 
    {
        int v = tan(i*3.14159265/128)*256;
        printf("%i%c ", v, 1?',':' ');

        float f = 4/tan(i*3.14159265/128);
        if (i==0) f=255;
        if (i>3) f*=4;
//        if (i>15) f*=4;
        v=f;
        printf("%i%c ", v, i<32?',':' ');

    }
    printf("};\n");

    printf("uint8_t ctan_table[33]= { ");
    for(  i=0; i<33; i++) 
    {

        float f = 4/tan(i*3.14159265/128);
        if (i==0) f=255;
        if (i>3) f*=4;
        if (i>15) f*=4;
        v=f;
        printf("%i%c ", v, i<32?',':' ');
    }


    printf("};\n");

    printf("uint8_t inv_table[16*128]= { ");
    for( uint8_t a = 0; a<16; a++)
    {

        for( uint8_t d = 0; d<128; d++) 
        {
            uint16_t n = 65535;
            uint16_t m = sin_table[2*a+64];
            m*=d;
            if (m) { n/=m; } else n=99999;
            if (n<1) n=1;
            if (n>80) n=80;
//            *(p++) = n;
            printf("%i, ", (int) n );
        }
        printf("\n");
    }
    printf("};\n");


    printf("uint8_t cc_table[16*33]= { \n");
    for( uint8_t d = 0; d<=32; d++) 
    {
        for( uint8_t a = 0; a<16; a++)
        {
            int m = 512 * cos(2*a*3.14159265/128) / cos(d*3.14159265/128) /sqrt(2) -256;   // 1 / (sqrt(2)/2)^2 = 1/(2/4) = 1...2
            printf("%i, ", (int) m );
        }
        printf("\n");
    }
    printf("};\n");
    printf("uint8_t atan_table[256]= { \n");
       for( int i=0; i<256; i++) printf("%i, ", (int) ( atan( i/256.0 )/3.14159265*128*4));
    printf("};\n");

    printf("FROLL!\n");
    printf("#asm \n");
    int index = 0, size;

    for( int f=0; f<=10; f++)
    {
        index = size = 0;
        printf("froll%i: \n", f );
        printf(" ld de, 0xFE20 \n" ); size+=3;

        for( uint8_t d=0; d<21; d++)
        {
            uint8_t c;
            if (index<10-f)  c=0; else // top  - a
            if (index>10+f)  c=1; else // bottom  - b
            if (index==10-f) c=2; else // top edge    (_vline_c)
            if (index==10+f) c=3; else // bottom edge (_vline_c2)
                             c=4;      // middle - c

            switch(c)
            {
                case 0: printf(" ld (hl), a  /*%2i*/  ", index ); size++; break;
                case 1: printf(" ld (hl), b  /*%2i*/  ", index ); size++; break;
                case 4: printf(" ld (hl), c  /*%2i*/  ", index ); size++; break;
                case 2: printf(" ex af, af \\ ld a, ixl  \\ ld (hl), a  /*%2i*/ \\ ex af, af ", index ); size+=5; break;
                case 3: printf(" ex af, af \\ ld a, ixh \\ ld (hl), a  /*%2i*/ \\ ex af, af ", index ); size+=5; break;
            }
            if (d<20)
            if (index<21-8)
            {
                printf(" \\ inc h " ); index+=8; size++;
            }
            else
            {
                printf(" \\ add hl,de " ); index-=15; size++;
                if (index<0)
                {
                    printf(" \\ inc h " ); index+=8; size++;
                }
            }
            printf("\n", size);
        }
        printf(" ret "); size++;
        for(; size<64; size++) printf(" \\ nop ");
        printf("\n");
    }
    printf("#endasm \n");

#if 0
    printf("\n\n\nFill8!\n");
    printf("#asm \n");
    for( int f=0; f<9; f++)
    {
        printf("filler%i:\n", f);
        for( uint8_t d=0; d<8; d++)
        {
            if (d==f) 
                printf("   cpl \n");

            printf("   ld (hl), a \n");
            if (d<7)
                printf("   inc h \n");
        }
        if (8==f) 
            printf("   cpl \n");

        printf("   ex de,hl\n");
        printf("   ret \n");
    }
    printf("#endasm \n");
#endif
}

