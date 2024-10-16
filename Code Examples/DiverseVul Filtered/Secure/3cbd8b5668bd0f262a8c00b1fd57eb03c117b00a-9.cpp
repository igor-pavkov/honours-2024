static uint32_t labdiff2(unsigned int b) {
    unsigned int r2, g2, b2;
    int32_t L1, A1, B1, L2, A2, B2;
    int64_t ld,ad,bd;

     r2 = (b>>16) & 0xff;
     g2 = (b>>8) & 0xff;
     b2 = b & 0xff;

     /* ref = 0x7f7f7f -> L*a*b ~ (53,0,0) */
     L1 = 53*(1<<24);
     A1 = 0;
     B1 = 0;
     lab2(r2, g2, b2, &L2, &A2, &B2);
     ld = L1 - L2;
     ld *= ld;
     ad = A1 - A2;
     ad *= ad;
     bd = B1 - B2;
     bd *= bd;
     ld += ad + bd;
     return ((uint32_t)(sqrt(ld/1024.0)))>>17;
}