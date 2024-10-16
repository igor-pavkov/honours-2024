void test_s_moveto(bezctx *z, double x, double y, int is_open) {
    test_bezctx *p = (test_bezctx*)z;
    int i;

    if ( (i=p->len) < S_RESULTS ) {
#ifdef VERBOSE
	printf("test_s_moveto(%g,%g)_%d [%d]%d\n",x,y,is_open,p->c_id,i);
#endif
	p->is_open = is_open;
	p->my_curve[i].x1 = x;
	p->my_curve[i].y1 = y;
	p->my_curve[p->len++].ty = 'm';
    }
}