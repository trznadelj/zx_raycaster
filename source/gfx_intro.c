#if 0
void intro1()
{
#if 0
    interp_c=5|56;
    for( int16_t i=0; i<1256; i++)
    {
       uint8_t o=i, o2=i;
       for( int8_t j=0; j<32; j++, o+=5, o2+=16)
       {
           interp_c=5|56;
           interp_x = j;        interp_y0 = 40+sin_table[o]/4;        interp_y1 = 40+sin_table[(o+5)&0xFF]/4;        
           if (interp_y0>interp_y1) { 
               k=interp_y0;
               interp_y0=interp_y1;
               interp_y1=k;
               interp_xx=0; interp_r=0xFF;
           } else { interp_xx=0xFF; interp_r =0; }
           interp();    


           interp_c=7|40;
	   interp_y0+=40; interp_y1+=40; interp();
/*
                  interp_c=1|56;
           interp_y0 = 120+sin_table[(o2)&0xFF]/4;        interp_y1 = 120+sin_table[(o2+16)&0xFF]/4;
           if (interp_y0>interp_y1) { 
               k=interp_y0;
               interp_y0=interp_y1;
               interp_y1=k;
               interp_xx=0; interp_r=0xFF;
           } else {interp_xx=0xFF; interp_r =0;}

           interp();*/
       }
       k = getk();
       if (k=='w') break;
    }
#endif
}

void intro2()
{
     uint8_t a;

     for( a=0; a<255; a++)
     {

         int16_t x0=((int16_t)sin_table[a+64*0])*2/3+128;
         int16_t y0=((int16_t)sin_table[a+64*1])  /2+ 96;
         int16_t x1=((int16_t)sin_table[a+64*1])*2/3+128;
         int16_t y1=((int16_t)sin_table[a+64*2])  /2+ 96;
         int16_t x2=((int16_t)sin_table[a+64*2])*2/3+128;
         int16_t y2=((int16_t)sin_table[a+64*3])  /2+ 96;
         int16_t x3=((int16_t)sin_table[a+64*3])*2/3+128;
         int16_t y3=((int16_t)sin_table[a     ])  /2+ 96;

         memset( Ybuf, 0, 32 );
         memset( Ybuf_o, 0, 32 );

         for( uint8_t x=0; x<32; x++)
         {
             if ( (x>=(x0>>3))&&(x<=(x1>>3)) ) Ybuf[x]   = ((int16_t)(x-(x0>>3)))*(y1-y0)*8 / (x1-x0) + y0;
             if ( (x>=(x1>>3))&&(x<=(x2>>3)) ) Ybuf_o[x] = ((int16_t)(x-(x1>>3)))*(y2-y1)*8 / (x2-x1) + y1;
             if ( (x>=(x2>>3))&&(x<=(x3>>3)) ) Ybuf[x]   = ((int16_t)(x-(x2>>3)))*(y3-y2)*8 / (x3-x2) + y2;
             if ( (x>=(x3>>3))&&(x<=(x0>>3)) ) Ybuf_o[x] = ((int16_t)(x-(x3>>3)))*(y0-y3)*8 / (x0-x3) + y3;

             if ( (x>=(x1>>3))&&(x<=(x0>>3)) ) Ybuf[x]   = ((int16_t)(x-(x1>>3)))*(y1-y0)*8 / (x0-x1) + y0;
             if ( (x>=(x2>>3))&&(x<=(x1>>3)) ) Ybuf_o[x] = ((int16_t)(x-(x2>>3)))*(y2-y1)*8 / (x1-x2) + y1;
             if ( (x>=(x3>>3))&&(x<=(x2>>3)) ) Ybuf[x]   = ((int16_t)(x-(x3>>3)))*(y3-y2)*8 / (x2-x3) + y2;
             if ( (x>=(x0>>3))&&(x<=(x3>>3)) ) Ybuf_o[x] = ((int16_t)(x-(x0>>3)))*(y0-y3)*8 / (x3-x0) + y3;
         }

         for( uint8_t x=0; x<32; x++)
         {
             vline_x = x;
             vline_d = Ybuf[x]-96;
             vline_db= Ybuf_o[x]-96;
             vline_c = 1;//Ybuf_c[x];
             vline();
         }
     }  
}

#endif
