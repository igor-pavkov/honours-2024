void test_s_lineto(bezctx *z, double x, double y) {
    test_bezctx *p = (test_bezctx*)z;
    int i;

    if ( (i=p->len) < S_RESULTS ) {
#ifdef VERBOSE
	printf("test_s_lineto(%g,%g) [%d]%d\n",x,y,p->c_id,i);
#endif
	p->my_curve[i].x1 = x;
	p->my_curve[i].y1 = y;
	p->my_curve[p->len++].ty = 'l';
    }
}