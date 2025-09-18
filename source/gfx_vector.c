uint8_t hline_x0, hline_y0, hline_l;
void hline_clr()
{
    uint8_t d = ((hline_x0&7)^7);
    uint8_t c = 0xFF>>d;

    uint8_t *p = ytable[hline_y0]+(hline_x0>>3);

    for( uint8_t l=hline_l; l; l-=d)
    {
        if (d<l) c&=0xFF>>(l-d);
        *p=c;
    }
}

void putline( uint8_t px, uint8_t py, uint8_t l)
{
    uint8_t pt1 = 0xFF>>(px&7);
    uint8_t pt2 = 0xFF<<(px&7), c, y=py&0xF8;
    uint16_t addr ;
    uint8_t dl = (((l+px)&0xF8)-(px&0xF8)+7)>>3;
    // clean above  
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
