static void record_gencode(GVJ_t * job, node_t * n)
{
    obj_state_t *obj = job->obj;
    boxf BF;
    pointf AF[4];
    int style;
    field_t *f;
    int doMap = (obj->url || obj->explicit_tooltip);
    int filled;
    char* clrs[2];

    f = (field_t *) ND_shape_info(n);
    BF = f->b;
    BF.LL.x += ND_coord(n).x;
    BF.LL.y += ND_coord(n).y;
    BF.UR.x += ND_coord(n).x;
    BF.UR.y += ND_coord(n).y;

    if (doMap && !(job->flags & EMIT_CLUSTERS_LAST))
	gvrender_begin_anchor(job,
			      obj->url, obj->tooltip, obj->target,
			      obj->id);
    style = stylenode(job, n);
    penColor(job, n);
    clrs[0] = NULL;
    if (style & FILLED) {
	char* fillcolor = findFill (n);
	float frac;
	
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
	    filled = FILL;
            gvrender_set_fillcolor(job, fillcolor);
	}
    }
    else filled = FALSE;

    if (streq(ND_shape(n)->name, "Mrecord"))
	style |= ROUNDED;
    if (SPECIAL_CORNERS(style)) {
	AF[0] = BF.LL;
	AF[2] = BF.UR;
	AF[1].x = AF[2].x;
	AF[1].y = AF[0].y;
	AF[3].x = AF[0].x;
	AF[3].y = AF[2].y;
	round_corners(job, AF, 4, style, filled);
    } else {
	gvrender_box(job, BF, filled);
    }

    gen_fields(job, n, f);

    if (clrs[0]) free (clrs[0]);

    if (doMap) {
	if (job->flags & EMIT_CLUSTERS_LAST)
	    gvrender_begin_anchor(job,
				  obj->url, obj->tooltip, obj->target,
				  obj->id);
	gvrender_end_anchor(job);
    }
}