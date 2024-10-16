static void lab2(uint32_t r, uint32_t g, uint32_t b, int32_t *L, int32_t *A, int32_t *B) {
    uint32_t xx,yy,zz;

    xx = rtable[r][0] + gtable[g][0] + btable[b][0];
    yy = rtable[r][1] + gtable[g][1] + btable[b][1];
    zz = rtable[r][2] + gtable[g][2] + btable[b][2];
    if (xx > 148587) {
	xx = (1<<24)*pow(xx/(95.047*(1<<24)), 1.0/3.0);
    }
    else {
	xx = xx * 24389/3132 + 2314099;
    }
    if (yy > 148587) {
	yy = (1<<24)*pow(yy/(100.0*(1<<24)), 1.0/3.0);
    }
    else {
	yy = yy * 24389/3132 + 2314099;
    }
    if (zz > 148587) {
	zz = (1<<24)*pow(zz/(108.883*(1<<24)), 1.0/3.0);
    }
    else {
	zz = zz * 24389/3132 + 2314099;
    }
    *L = (116*yy - 116*2314099);
    *A = 500/4*(xx - yy);
    *B = 200/4*(yy - zz);/* /4 to avoid overflow */
}