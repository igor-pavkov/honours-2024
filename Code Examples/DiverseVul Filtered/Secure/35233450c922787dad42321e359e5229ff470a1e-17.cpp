void test_s_quadto(bezctx *z, double x1, double y1, double x2, double y2) {
    test_bezctx *p = (test_bezctx*)z;
    int i;

    if ( (i=p->len) < S_RESULTS ) {
#ifdef VERBOSE
	printf("test_s_quadto(%g,%g, %g,%g) [%d]%d\n",x1,y1,x2,y2,p->c_id,i);
#endif
	p->my_curve[i].x1 = x1;
	p->my_curve[i].y1 = y1;
	p->my_curve[i].x2 = x2;
	p->my_curve[i].y2 = y2;
	p->my_curve[p->len++].ty = 'q';
    }
}