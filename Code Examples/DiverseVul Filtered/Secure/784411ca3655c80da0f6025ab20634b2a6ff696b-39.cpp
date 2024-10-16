static void poly_init(node_t * n)
{
    pointf dimen, min_bb, bb;
    point imagesize;
    pointf P, Q, R;
    pointf *vertices;
    char *p, *sfile, *fxd;
    double temp, alpha, beta, gamma;
    double orientation, distortion, skew;
    double sectorangle, sidelength, skewdist, gdistortion, gskew;
    double angle, sinx, cosx, xmax, ymax, scalex, scaley;
    double width, height, marginx, marginy, spacex;
    int regular, peripheries, sides;
    int i, j, isBox, outp;
    polygon_t *poly = NEW(polygon_t);
    boolean isPlain = IS_PLAIN(n);

    regular = ND_shape(n)->polygon->regular;
    peripheries = ND_shape(n)->polygon->peripheries;
    sides = ND_shape(n)->polygon->sides;
    orientation = ND_shape(n)->polygon->orientation;
    skew = ND_shape(n)->polygon->skew;
    distortion = ND_shape(n)->polygon->distortion;
    regular |= mapbool(agget(n, "regular"));

    /* all calculations in floating point POINTS */

    /* make x and y dimensions equal if node is regular
     *   If the user has specified either width or height, use the max.
     *   Else use minimum default value.
     * If node is not regular, use the current width and height.
     */
    if (isPlain) {
	width = height = 0;
    }
    else if (regular) {
	double sz = userSize(n);
	if (sz > 0.0)
	    width = height = sz;
	else {
	    width = ND_width(n);
	    height = ND_height(n);
	    width = height = POINTS(MIN(width, height));
	}
    } else {
	width = POINTS(ND_width(n));
	height = POINTS(ND_height(n));
    }

    peripheries = late_int(n, N_peripheries, peripheries, 0);
    orientation += late_double(n, N_orientation, 0.0, -360.0);
    if (sides == 0) {		/* not for builtins */
	skew = late_double(n, N_skew, 0.0, -100.0);
	sides = late_int(n, N_sides, 4, 0);
	distortion = late_double(n, N_distortion, 0.0, -100.0);
    }

    /* get label dimensions */
    dimen = ND_label(n)->dimen;

    /* minimal whitespace around label */
    if ((dimen.x > 0) || (dimen.y > 0)) {
	/* padding */
	if (!isPlain) {
	    if ((p = agget(n, "margin"))) {
		marginx = marginy = 0;
		i = sscanf(p, "%lf,%lf", &marginx, &marginy);
		if (marginx < 0)
		    marginx = 0;
		if (marginy < 0)
		    marginy = 0;
		if (i > 0) {
		    dimen.x += 2 * POINTS(marginx);
		    if (i > 1)
			dimen.y += 2 * POINTS(marginy);
		    else
			dimen.y += 2 * POINTS(marginx);
		} else
		    PAD(dimen);
	    } else
		PAD(dimen);
	}
    }
    spacex = dimen.x - ND_label(n)->dimen.x;

    /* quantization */
    if ((temp = GD_drawing(agraphof(n))->quantum) > 0.0) {
	temp = POINTS(temp);
	dimen.x = quant(dimen.x, temp);
	dimen.y = quant(dimen.y, temp);
    }

    imagesize.x = imagesize.y = 0;
    if (ND_shape(n)->usershape) {
	/* custom requires a shapefile
	 * not custom is an adaptable user shape such as a postscript
	 * function.
	 */
	if (streq(ND_shape(n)->name, "custom")) {
	    sfile = agget(n, "shapefile");
	    imagesize = gvusershape_size(agraphof(n), sfile);
	    if ((imagesize.x == -1) && (imagesize.y == -1)) {
		agerr(AGWARN,
		      "No or improper shapefile=\"%s\" for node \"%s\"\n",
		      (sfile ? sfile : "<nil>"), agnameof(n));
		imagesize.x = imagesize.y = 0;
	    } else {
		GD_has_images(agraphof(n)) = TRUE;
		imagesize.x += 2;	/* some fixed padding */
		imagesize.y += 2;
	    }
	}
    } else if ((sfile = agget(n, "image")) && (*sfile != '\0')) {
	imagesize = gvusershape_size(agraphof(n), sfile);
	if ((imagesize.x == -1) && (imagesize.y == -1)) {
	    agerr(AGWARN,
		  "No or improper image=\"%s\" for node \"%s\"\n",
		  (sfile ? sfile : "<nil>"), agnameof(n));
	    imagesize.x = imagesize.y = 0;
	} else {
	    GD_has_images(agraphof(n)) = TRUE;
	    imagesize.x += 2;	/* some fixed padding */
	    imagesize.y += 2;
	}
    }

    /* initialize node bb to labelsize */
    bb.x = MAX(dimen.x, imagesize.x);
    bb.y = MAX(dimen.y, imagesize.y);

    /* I don't know how to distort or skew ellipses in postscript */
    /* Convert request to a polygon with a large number of sides */
    if ((sides <= 2) && ((distortion != 0.) || (skew != 0.))) {
	sides = 120;
    }

    /* extra sizing depends on if label is centered vertically */
    p = agget(n, "labelloc");
    if (p && (p[0] == 't' || p[0] == 'b'))
	ND_label(n)->valign = p[0];
    else
	ND_label(n)->valign = 'c';

    isBox = (sides == 4 && (ROUND(orientation) % 90) == 0
	     && distortion == 0. && skew == 0.);
    if (isBox) {
	/* for regular boxes the fit should be exact */
    } else if (ND_shape(n)->polygon->vertices) {
	poly_desc_t* pd = (poly_desc_t*)ND_shape(n)->polygon->vertices;
	bb = pd->size_gen(bb);
    } else {
	/* for all other shapes, compute a smallest ellipse
	 * containing bb centered on the origin, and then pad for that.
	 * We assume the ellipse is defined by a scaling up of bb.
	 */
	temp = bb.y * SQRT2;
	if (height > temp && ND_label(n)->valign == 'c') {
	    /* if there is height to spare
	     * and the label is centered vertically
	     * then just pad x in proportion to the spare height */
	    bb.x *= sqrt(1. / (1. - SQR(bb.y / height)));
	} else {
	    bb.x *= SQRT2;
	    bb.y = temp;
	}
#if 1
	if (sides > 2) {
	    temp = cos(M_PI / sides);
	    bb.x /= temp;
	    bb.y /= temp;
	    /* FIXME - for odd-sided polygons, e.g. triangles, there
	       would be a better fit with some vertical adjustment of the shape */
	}
#endif
    }

    /* at this point, bb is the minimum size of node that can hold the label */
    min_bb = bb;

    /* increase node size to width/height if needed */
    fxd = late_string(n, N_fixed, "false");
    if ((*fxd == 's') && streq(fxd,"shape")) {
	bb.x = width;
	bb.y = height;
	poly->option |= FIXEDSHAPE;
    } else if (mapbool(fxd)) {
	/* check only label, as images we can scale to fit */
	if ((width < ND_label(n)->dimen.x) || (height < ND_label(n)->dimen.y))
	    agerr(AGWARN,
		  "node '%s', graph '%s' size too small for label\n",
		  agnameof(n), agnameof(agraphof(n)));
	bb.x = width;
	bb.y = height;
    } else {
	bb.x = width = MAX(width, bb.x);
	bb.y = height = MAX(height, bb.y);
    }

    /* If regular, make dimensions the same.
     * Need this to guarantee final node size is regular.
     */
    if (regular) {
	width = height = bb.x = bb.y = MAX(bb.x, bb.y);
    }

    /* Compute space available for label.  Provides the justification borders */
    if (!mapbool(late_string(n, N_nojustify, "false"))) {
	if (isBox) {
	    ND_label(n)->space.x = MAX(dimen.x,bb.x) - spacex;
	}
	else if (dimen.y < bb.y) {
	    temp = bb.x * sqrt(1.0 - SQR(dimen.y) / SQR(bb.y));
	    ND_label(n)->space.x = MAX(dimen.x,temp) - spacex;
        }
	else
	    ND_label(n)->space.x = dimen.x - spacex;
    } else {
	ND_label(n)->space.x = dimen.x - spacex;
    }

    if ((poly->option & FIXEDSHAPE) == 0) {
	temp = bb.y - min_bb.y;
	if (dimen.y < imagesize.y)
	    temp += imagesize.y - dimen.y;
	ND_label(n)->space.y = dimen.y + temp;
    }

    outp = peripheries;
    if (peripheries < 1)
	outp = 1;
    if (sides < 3) {		/* ellipses */
	sides = 2;
	vertices = N_NEW(outp * sides, pointf);
	P.x = bb.x / 2.;
	P.y = bb.y / 2.;
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
	    bb.x = 2. * P.x;
	    bb.y = 2. * P.y;
	}
    } else {

/*
 * FIXME - this code is wrong - it doesn't work for concave boundaries.
 *          (e.g. "folder"  or "promoter")
 *   I don't think it even needs sectorangle, or knowledge of skewed shapes.
 *   (Concepts that only work for convex regular (modulo skew/distort) polygons.)
 *
 *   I think it only needs to know inside v. outside (by always drawing
 *   boundaries clockwise, say),  and the two adjacent segments.
 *
 *   It needs to find the point where the two lines, parallel to
 *   the current segments, and outside by GAP distance, intersect.   
 */

	vertices = N_NEW(outp * sides, pointf);
	if (ND_shape(n)->polygon->vertices) {
	    poly_desc_t* pd = (poly_desc_t*)ND_shape(n)->polygon->vertices;
	    pd->vertex_gen (vertices, &bb);
	    xmax = bb.x/2;
	    ymax = bb.y/2;
	} else {
	    sectorangle = 2. * M_PI / sides;
	    sidelength = sin(sectorangle / 2.);
	    skewdist = hypot(fabs(distortion) + fabs(skew), 1.);
	    gdistortion = distortion * SQRT2 / cos(sectorangle / 2.);
	    gskew = skew / 2.;
	    angle = (sectorangle - M_PI) / 2.;
	    sincos(angle, &sinx, &cosx);
	    R.x = .5 * cosx;
	    R.y = .5 * sinx;
	    xmax = ymax = 0.;
	    angle += (M_PI - sectorangle) / 2.;
	    for (i = 0; i < sides; i++) {

	    /*next regular vertex */
		angle += sectorangle;
		sincos(angle, &sinx, &cosx);
		R.x += sidelength * cosx;
		R.y += sidelength * sinx;

	    /*distort and skew */
		P.x = R.x * (skewdist + R.y * gdistortion) + R.y * gskew;
		P.y = R.y;

	    /*orient P.x,P.y */
		alpha = RADIANS(orientation) + atan2(P.y, P.x);
		sincos(alpha, &sinx, &cosx);
		P.x = P.y = hypot(P.x, P.y);
		P.x *= cosx;
		P.y *= sinx;

	    /*scale for label */
		P.x *= bb.x;
		P.y *= bb.y;

	    /*find max for bounding box */
		xmax = MAX(fabs(P.x), xmax);
		ymax = MAX(fabs(P.y), ymax);

	    /* store result in array of points */
		vertices[i] = P;
		if (isBox) { /* enforce exact symmetry of box */
		    vertices[1].x = -P.x;
		    vertices[1].y = P.y;
		    vertices[2].x = -P.x;
		    vertices[2].y = -P.y;
		    vertices[3].x = P.x;
		    vertices[3].y = -P.y;
		    break;
		}
	    }
	}

	/* apply minimum dimensions */
	xmax *= 2.;
	ymax *= 2.;
	bb.x = MAX(width, xmax);
	bb.y = MAX(height, ymax);
	scalex = bb.x / xmax;
	scaley = bb.y / ymax;

	for (i = 0; i < sides; i++) {
	    P = vertices[i];
	    P.x *= scalex;
	    P.y *= scaley;
	    vertices[i] = P;
	}

	if (peripheries > 1) {
	    Q = vertices[(sides - 1)];
	    R = vertices[0];
	    beta = atan2(R.y - Q.y, R.x - Q.x);
	    for (i = 0; i < sides; i++) {

		/*for each vertex find the bisector */
		P = Q;
		Q = R;
		R = vertices[(i + 1) % sides];
		alpha = beta;
		beta = atan2(R.y - Q.y, R.x - Q.x);
		gamma = (alpha + M_PI - beta) / 2.;

		/*find distance along bisector to */
		/*intersection of next periphery */
		temp = GAP / sin(gamma);

		/*convert this distance to x and y */
		sincos((alpha - gamma), &sinx, &cosx);
		sinx *= temp;
		cosx *= temp;

		/*save the vertices of all the */
		/*peripheries at this base vertex */
		for (j = 1; j < peripheries; j++) {
		    Q.x += cosx;
		    Q.y += sinx;
		    vertices[i + j * sides] = Q;
		}
	    }
	    for (i = 0; i < sides; i++) {
		P = vertices[i + (peripheries - 1) * sides];
		bb.x = MAX(2. * fabs(P.x), bb.x);
		bb.y = MAX(2. * fabs(P.y), bb.y);
	    }
	}
    }
    poly->regular = regular;
    poly->peripheries = peripheries;
    poly->sides = sides;
    poly->orientation = orientation;
    poly->skew = skew;
    poly->distortion = distortion;
    poly->vertices = vertices;

    if (poly->option & FIXEDSHAPE) {
	/* set width and height to reflect label and shape */
	ND_width(n) = PS2INCH(MAX(dimen.x,bb.x));
	ND_height(n) = PS2INCH(MAX(dimen.y,bb.y));
    } else {
	ND_width(n) = PS2INCH(bb.x);
	ND_height(n) = PS2INCH(bb.y);
    }
    ND_shape_info(n) = (void *) poly;
}