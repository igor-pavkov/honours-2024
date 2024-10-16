bezctx *new_bezctx_test(void) {
    bezctx *result = malloc(1*sizeof(bezctx));

    result->moveto = test_moveto;
    result->lineto = test_lineto;
    result->quadto = test_quadto;
    result->curveto = test_curveto;
    result->mark_knot = test_mark_knot;
    return result;
}