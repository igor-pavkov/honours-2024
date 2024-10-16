static void poly_gencode(GVJ_t * job, node_t * n)
{
    obj_state_t *obj = job->obj;
    polygon_t *poly;
    double xsize, ysize;
    int i, j, peripheries, sides, style;
    pointf P, *vertices;
    static pointf *AF;
    static int A_size;
    boolean filled;
    boolean usershape_p;
    boolean pfilled;		/* true if fill not handled by user shape */
    char *color, *name;
    int doMap = (obj->url || obj->explicit_tooltip);
    char* fillcolor=NULL;
    char* pencolor=NULL;
    char* clrs[2];

    if (doMap && !(job->flags & EMIT_CLUSTERS_LAST))
	gvrender_begin_anchor(job,
			      obj->url, obj->tooltip, obj->target,
			      obj->id);

    poly = (polygon_t *) ND_shape_info(n);
    vertices = poly->vertices;
    sides = poly->sides;
    peripheries = poly->peripheries;
    if (A_size < sides) {
	A_size = sides + 5;
	AF = ALLOC(A_size, AF, pointf);
    }

    /* nominal label position in the center of the node */
    ND_label(n)->pos = ND_coord(n);

    xsize = (ND_lw(n) + ND_rw(n)) / POINTS(ND_width(n));
    ysize = ND_ht(n) / POINTS(ND_height(n));

    style = stylenode(job, n);
    clrs[0] = NULL;

    if (ND_gui_state(n) & GUI_STATE_ACTIVE) {
	pencolor = late_nnstring(n, N_activepencolor, DEFAULT_ACTIVEPENCOLOR);
	gvrender_set_pencolor(job, pencolor);
	color =
	    late_nnstring(n, N_activefillcolor, DEFAULT_ACTIVEFILLCOLOR);
	gvrender_set_fillcolor(job, color);
	filled = FILL;
    } else if (ND_gui_state(n) & GUI_STATE_SELECTED) {
	pencolor =
	    late_nnstring(n, N_selectedpencolor, DEFAULT_SELECTEDPENCOLOR);
	gvrender_set_pencolor(job, pencolor);
	color =
	    late_nnstring(n, N_selectedfillcolor,
			  DEFAULT_SELECTEDFILLCOLOR);
	gvrender_set_fillcolor(job, color);
	filled = FILL;
    } else if (ND_gui_state(n) & GUI_STATE_DELETED) {
	pencolor =
	    late_nnstring(n, N_deletedpencolor, DEFAULT_DELETEDPENCOLOR);
	gvrender_set_pencolor(job, pencolor);
	color =
	    late_nnstring(n, N_deletedfillcolor, DEFAULT_DELETEDFILLCOLOR);
	gvrender_set_fillcolor(job, color);
	filled = FILL;
    } else if (ND_gui_state(n) & GUI_STATE_VISITED) {
	pencolor =
	    late_nnstring(n, N_visitedpencolor, DEFAULT_VISITEDPENCOLOR);
	gvrender_set_pencolor(job, pencolor);
	color =
	    late_nnstring(n, N_visitedfillcolor, DEFAULT_VISITEDFILLCOLOR);
	gvrender_set_fillcolor(job, color);
	filled = FILL;
    } else {
	if (style & FILLED) {
	    float frac;
	    fillcolor = findFill (n);
	    if (findStopColor (fillcolor, clrs, &frac)) {
        	gvrender_set_fillcolor(job, clrs[0]);
		if (clrs[1]) 
		    gvrender_set_gradient_vals(job,clrs[1],late_int(n,N_gradientangle,0,0), frac);
		else 
		    gvrender_set_gradient_vals(job,DEFAULT_COLOR,late_int(n,N_gradientangle,0,0), frac);
		if (style & RADIAL)
		    filled = RGRADIENT;
	 	else
		    filled = GRADIENT;
	    }
	    else {
        	gvrender_set_fillcolor(job, fillcolor);
		filled = FILL;
	    }
	}
	else if (style & (STRIPED|WEDGED))  {
	    fillcolor = findFill (n);
            /* gvrender_set_fillcolor(job, fillcolor); */
	    filled = TRUE;
	}
	else {
	    filled = FALSE;
	}
	pencolor = penColor(job, n);	/* emit pen color */
    }

    pfilled = !ND_shape(n)->usershape || streq(ND_shape(n)->name, "custom");

    /* if no boundary but filled, set boundary color to transparent */
    if ((peripheries == 0) && filled && pfilled) {
	peripheries = 1;
	gvrender_set_pencolor(job, "transparent");
    }

    /* draw peripheries first */
    for (j = 0; j < peripheries; j++) {
	for (i = 0; i < sides; i++) {
	    P = vertices[i + j * sides];
	    AF[i].x = P.x * xsize + ND_coord(n).x;
	    AF[i].y = P.y * ysize + ND_coord(n).y;
	}
	if (sides <= 2) {
	    if ((style & WEDGED) && (j == 0) && multicolor(fillcolor)) {
		int rv = wedgedEllipse (job, AF, fillcolor);
		if (rv > 1)
		    agerr (AGPREV, "in node %s\n", agnameof(n));
		filled = 0;
	    }
	    gvrender_ellipse(job, AF, sides, filled);
	    if (style & DIAGONALS) {
		Mcircle_hack(job, n);
	    }
	} else if (style & STRIPED) {
	    if (j == 0) {
		int rv = stripedBox (job, AF, fillcolor, 1);
		if (rv > 1)
		    agerr (AGPREV, "in node %s\n", agnameof(n));
	    }
	    gvrender_polygon(job, AF, sides, 0);
	} else if (style & UNDERLINE) {
	    gvrender_set_pencolor(job, "transparent");
	    gvrender_polygon(job, AF, sides, filled);
	    gvrender_set_pencolor(job, pencolor);
	    gvrender_polyline(job, AF+2, 2);
	} else if (SPECIAL_CORNERS(style)) {
	    round_corners(job, AF, sides, style, filled);
	} else {
	    gvrender_polygon(job, AF, sides, filled);
	}
	/* fill innermost periphery only */
	filled = FALSE;
    }

    usershape_p = FALSE;
    if (ND_shape(n)->usershape) {
	name = ND_shape(n)->name;
	if (streq(name, "custom")) {
	    if ((name = agget(n, "shapefile")) && name[0])
		usershape_p = TRUE;
	} else
	    usershape_p = TRUE;
    } else if ((name = agget(n, "image")) && name[0]) {
	usershape_p = TRUE;
    }
    if (usershape_p) {
	/* get coords of innermost periphery */
	for (i = 0; i < sides; i++) {
	    P = vertices[i];
	    AF[i].x = P.x * xsize + ND_coord(n).x;
	    AF[i].y = P.y * ysize + ND_coord(n).y;
	}
	/* lay down fill first */
	if (filled && pfilled) {
	    if (sides <= 2) {
		if ((style & WEDGED) && (j == 0) && multicolor(fillcolor)) {
		    int rv = wedgedEllipse (job, AF, fillcolor);
		    if (rv > 1)
			agerr (AGPREV, "in node %s\n", agnameof(n));
		    filled = 0;
		}
		gvrender_ellipse(job, AF, sides, filled);
		if (style & DIAGONALS) {
		    Mcircle_hack(job, n);
		}
	    } else if (style & STRIPED) {
		int rv = stripedBox (job, AF, fillcolor, 1);
		if (rv > 1)
		    agerr (AGPREV, "in node %s\n", agnameof(n));
		gvrender_polygon(job, AF, sides, 0);
	    } else if (style & (ROUNDED | DIAGONALS)) {
		round_corners(job, AF, sides, style, filled);
	    } else {
		gvrender_polygon(job, AF, sides, filled);
	    }
	}
	gvrender_usershape(job, name, AF, sides, filled,
			   late_string(n, N_imagescale, "false"),
			   late_string(n, N_imagepos, "mc"));
	filled = FALSE;		/* with user shapes, we have done the fill if needed */
    }

    free (clrs[0]);

    emit_label(job, EMIT_NLABEL, ND_label(n));
    if (doMap) {
	if (job->flags & EMIT_CLUSTERS_LAST)
	    gvrender_begin_anchor(job,
				  obj->url, obj->tooltip, obj->target,
				  obj->id);
	gvrender_end_anchor(job);
    }
}