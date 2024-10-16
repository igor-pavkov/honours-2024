static void point_gencode(GVJ_t * job, node_t * n)
{
    obj_state_t *obj = job->obj;
    polygon_t *poly;
    int i, j, sides, peripheries, style;
    pointf P, *vertices;
    static pointf *AF;
    static int A_size;
    boolean filled;
    char *color;
    int doMap = (obj->url || obj->explicit_tooltip);

    if (doMap && !(job->flags & EMIT_CLUSTERS_LAST))
	gvrender_begin_anchor(job,
			      obj->url, obj->tooltip, obj->target,
			      obj->id);

    poly = (polygon_t *) ND_shape_info(n);
    vertices = poly->vertices;
    sides = poly->sides;
    peripheries = poly->peripheries;
    if (A_size < sides) {
	A_size = sides + 2;
	AF = ALLOC(A_size, AF, pointf);
    }

    checkStyle(n, &style);
    if (style & INVISIBLE)
	gvrender_set_style(job, point_style);
    else
	gvrender_set_style(job, &point_style[1]);
    if (N_penwidth)
	gvrender_set_penwidth(job, late_double(n, N_penwidth, 1.0, 0.0));

    if (ND_gui_state(n) & GUI_STATE_ACTIVE) {
	color = late_nnstring(n, N_activepencolor, DEFAULT_ACTIVEPENCOLOR);
	gvrender_set_pencolor(job, color);
	color =
	    late_nnstring(n, N_activefillcolor, DEFAULT_ACTIVEFILLCOLOR);
	gvrender_set_fillcolor(job, color);
    } else if (ND_gui_state(n) & GUI_STATE_SELECTED) {
	color =
	    late_nnstring(n, N_selectedpencolor, DEFAULT_SELECTEDPENCOLOR);
	gvrender_set_pencolor(job, color);
	color =
	    late_nnstring(n, N_selectedfillcolor,
			  DEFAULT_SELECTEDFILLCOLOR);
	gvrender_set_fillcolor(job, color);
    } else if (ND_gui_state(n) & GUI_STATE_DELETED) {
	color =
	    late_nnstring(n, N_deletedpencolor, DEFAULT_DELETEDPENCOLOR);
	gvrender_set_pencolor(job, color);
	color =
	    late_nnstring(n, N_deletedfillcolor, DEFAULT_DELETEDFILLCOLOR);
	gvrender_set_fillcolor(job, color);
    } else if (ND_gui_state(n) & GUI_STATE_VISITED) {
	color =
	    late_nnstring(n, N_visitedpencolor, DEFAULT_VISITEDPENCOLOR);
	gvrender_set_pencolor(job, color);
	color =
	    late_nnstring(n, N_visitedfillcolor, DEFAULT_VISITEDFILLCOLOR);
	gvrender_set_fillcolor(job, color);
    } else {
	color = findFillDflt(n, "black");
	gvrender_set_fillcolor(job, color);	/* emit fill color */
	penColor(job, n);	/* emit pen color */
    }
    filled = TRUE;

    /* if no boundary but filled, set boundary color to fill color */
    if (peripheries == 0) {
	peripheries = 1;
	if (color[0])
	    gvrender_set_pencolor(job, color);
    }

    for (j = 0; j < peripheries; j++) {
	for (i = 0; i < sides; i++) {
	    P = vertices[i + j * sides];
	    AF[i].x = P.x + ND_coord(n).x;
	    AF[i].y = P.y + ND_coord(n).y;
	}
	gvrender_ellipse(job, AF, sides, filled);
	/* fill innermost periphery only */
	filled = FALSE;
    }

    if (doMap) {
	if (job->flags & EMIT_CLUSTERS_LAST)
	    gvrender_begin_anchor(job,
				  obj->url, obj->tooltip, obj->target,
				  obj->id);
	gvrender_end_anchor(job);
    }
}