#ifdef DO_PERF_TEST

char* perf_test_helper( char* s, uint16_t x, uint16_t d)
{
    static char buf[32];
    uint32_t fps = 255*5000; fps/=x;

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
        player(); raycast2(); renderv_full();
    }
    pegs[5] = get_clock()-start;


    start = get_clock();
    for( pa=0; pa<255; pa++) {
        raycast2(); bwrender3();
    }
    pegs[6] = get_clock()-start;

    clg(); 
    printf("%c%c%cPerformance test result: \n", 22, 32, 32);
    printf("  Triangles:      %d \n", pegs[0]);
    perf_test_helper("Raycast:       ",pegs[1],0);
    perf_test_helper("Raycast+cdiv:  ",pegs[2],pegs[1]);
    perf_test_helper("RenderV:       ",pegs[3],pegs[1]);
    perf_test_helper("RenderV+player:",pegs[4],pegs[3]);
    perf_test_helper("RenderV_full:  ",pegs[5],pegs[1]);
    perf_test_helper("BWrender3:     ",pegs[6],pegs[2]);
    printf("Compilation flags:\n");
#ifdef DO_POPULATE_OTABLE
    printf("  DO_POPULATE_OTABLE\n");
#endif
#ifdef DO_OPTIMIZED_RAYCASTING
    printf("  DO_OPTIMIZED_RAYCASTING\n");
#endif
#ifdef DEBUG_LENGTH
    printf("  DEBUG_LENGTH\n");
#endif
#ifdef DO_LARGE_DIST
    printf("  DO_LARGE_DIST\n");
#endif

#ifdef DO_LUT_TAN_MUL
    printf("  DO_LUT_TAN_MUL\n");
#endif

#ifdef DO_LUT_CTAN_MUL
    printf("  DO_LUT_CTAN_MUL\n");
#endif

//#ifdef USE_STACK
//    printf("  USE_STACK\n");
//#endif

    printf("  Build time: " __DATE__ " "__TIME__ "\n");

    printf("Press control key (QWEASDU) to start game");

    while( !get_qweasdu() );
}

void atan_test()
{
    for( int16_t x=-10; x<10; x+=2)
{
    for( int16_t y=-10; y<10; y+=2)
{
  //    atan_x=x; atan_y=y; uint8_t r=int_atan2();
div16x=x; div16y=y;
atan_x=x; atan_y=y;
//div16_16_8_rest(); 
uint8_t r = int_atan2();
      printf("%2.2x ", r);
}
printf("\n");
}
while(1);
}

#endif
