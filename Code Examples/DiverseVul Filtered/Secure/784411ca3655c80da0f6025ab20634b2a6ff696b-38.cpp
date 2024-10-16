static boolean point_inside(inside_t * inside_context, pointf p)
{
    static node_t *lastn;	/* last node argument */
    static double radius;
    pointf P;
    node_t *n;

    if (!inside_context) {
	lastn = NULL;
	return FALSE;
    }

    n = inside_context->s.n;
    P = ccwrotatepf(p, 90 * GD_rankdir(agraphof(n)));

    if (n != lastn) {
	int outp;
	polygon_t *poly = (polygon_t *) ND_shape_info(n);

	/* index to outer-periphery */
	outp = 2 * (poly->peripheries - 1);
	if (outp < 0)
	    outp = 0;

	radius = poly->vertices[outp + 1].x;
	lastn = n;
    }

    /* inside bounding box? */
    if ((fabs(P.x) > radius) || (fabs(P.y) > radius))
	return FALSE;

    return (hypot(P.x, P.y) <= radius);
}