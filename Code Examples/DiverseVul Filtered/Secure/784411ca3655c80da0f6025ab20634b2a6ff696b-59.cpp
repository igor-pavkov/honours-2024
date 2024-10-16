static boolean star_inside(inside_t * inside_context, pointf p)
{
    static node_t *lastn;	/* last node argument */
    static polygon_t *poly;
    static int outp, sides;
    static pointf *vertex;
    static pointf O;		/* point (0,0) */

    if (!inside_context) {
	lastn = NULL;
	return FALSE;
    }
    boxf *bp = inside_context->s.bp;
    node_t *n = inside_context->s.n;
    pointf P, Q, R;
    int i, outcnt;

    P = ccwrotatepf(p, 90 * GD_rankdir(agraphof(n)));

    /* Quick test if port rectangle is target */
    if (bp) {
	boxf bbox = *bp;
	return INSIDE(P, bbox);
    }

    if (n != lastn) {
	poly = (polygon_t *) ND_shape_info(n);
	vertex = poly->vertices;
	sides = poly->sides;

	/* index to outer-periphery */
	outp = (poly->peripheries - 1) * sides;
	if (outp < 0)
	    outp = 0;
	lastn = n;
    }

    outcnt = 0;
    for (i = 0; i < sides; i += 2) {
	Q = vertex[i + outp];
	R = vertex[((i+4) % sides) + outp];
	if (!(same_side(P, O, Q, R))) {
	    outcnt++;
	}
	if (outcnt == 2) {
	    return FALSE;
	}
    }
    return TRUE;
}