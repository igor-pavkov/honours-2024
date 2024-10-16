void test_s_mark_knot(bezctx *z, int knot_idx) {
    test_bezctx *p = (test_bezctx*)z;

    if ( p->len < S_RESULTS )
#ifdef VERBOSE
	printf("test_s_mark_knot()_%d [%d]%d\n",knot_idx,p->c_id,p->len);
#endif
	p->my_curve[p->len++].ty = 'k';
}