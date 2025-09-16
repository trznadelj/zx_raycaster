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