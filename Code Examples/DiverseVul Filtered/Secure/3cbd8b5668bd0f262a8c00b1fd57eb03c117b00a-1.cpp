static void hsv(unsigned int c, unsigned int *r, unsigned int *g, unsigned int *b, unsigned int *s, unsigned int *v, unsigned int *delta) {
    unsigned int min, max;
    *r=(c>>16)&0xff;
    *g=(c>>8)&0xff;
    *b=c&0xff;
    min = MIN(*r,MIN(*g, *b));
    max = MAX(*r,MAX(*g,*b));
    *v = max;
    *delta = max-min;
    if(!*delta)
	*s = 0;
    else
	*s = 255 * (*delta) / max;
}