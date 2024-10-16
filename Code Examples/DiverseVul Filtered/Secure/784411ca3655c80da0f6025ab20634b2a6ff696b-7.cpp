static void point_init(node_t * n)
{
    polygon_t *poly = NEW(polygon_t);
    int sides, outp, peripheries = ND_shape(n)->polygon->peripheries;
    double sz;
    pointf P, *vertices;
    int i, j;
    double w, h;

    /* set width and height, and make them equal
     * if user has set weight or height, use it.
     * if both are set, use smallest.
     * if neither, use default
     */
    w = late_double(n, N_width, MAXDOUBLE, 0.0);
    h = late_double(n, N_height, MAXDOUBLE, 0.0);
    w = MIN(w, h);
    if ((w == MAXDOUBLE) && (h == MAXDOUBLE))	/* neither defined */
	ND_width(n) = ND_height(n) = DEF_POINT;
    else {
	w = MIN(w, h);
	/* If w == 0, use it; otherwise, make w no less than MIN_POINT due
         * to the restrictions mentioned above.
         */
	if (w > 0.0) 
	    w = MAX(w,MIN_POINT);
	ND_width(n) = ND_height(n) = w;
    }

    sz = ND_width(n) * POINTS_PER_INCH;
    peripheries = late_int(n, N_peripheries, peripheries, 0);
    if (peripheries < 1)
	outp = 1;
    else
	outp = peripheries;
    sides = 2;
    vertices = N_NEW(outp * sides, pointf);
    P.y = P.x = sz / 2.;
    vertices[0].x = -P.x;
    vertices[0].y = -P.y;
    vertices[1] = P;
    if (peripheries > 1) {
	for (j = 1, i = 2; j < peripheries; j++) {
	    P.x += GAP;
	    P.y += GAP;
	    vertices[i].x = -P.x;
	    vertices[i].y = -P.y;
	    i++;
	    vertices[i].x = P.x;
	    vertices[i].y = P.y;
	    i++;
	}
	sz = 2. * P.x;
    }
    poly->regular = 1;
    poly->peripheries = peripheries;
    poly->sides = 2;
    poly->orientation = 0;
    poly->skew = 0;
    poly->distortion = 0;
    poly->vertices = vertices;

    ND_height(n) = ND_width(n) = PS2INCH(sz);
    ND_shape_info(n) = (void *) poly;
}