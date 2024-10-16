static pointf compassPoint(inside_t * ictxt, double y, double x)
{
    pointf curve[4];		/* bezier control points for a straight line */
    node_t *n = ictxt->s.n;
    graph_t* g = agraphof(n);
    int rd = GD_rankdir(g);
    pointf p;

    p.x = x;
    p.y = y;
    if (rd)
	p = cwrotatepf(p, 90 * rd);

    curve[0].x = curve[0].y = 0;
    curve[1] = curve[0];
    curve[3] = curve[2] = p;

    bezier_clip(ictxt, ND_shape(n)->fns->insidefn, curve, 1);

    if (rd)
	curve[0] = ccwrotatepf(curve[0], 90 * rd);
    return curve[0];
}