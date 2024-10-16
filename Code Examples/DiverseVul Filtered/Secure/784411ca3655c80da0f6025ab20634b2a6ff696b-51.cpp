static void gen_fields(GVJ_t * job, node_t * n, field_t * f)
{
    int i;
    pointf AF[2], coord;

    if (f->lp) {
	f->lp->pos = add_pointf(mid_pointf(f->b.LL, f->b.UR), ND_coord(n));
	emit_label(job, EMIT_NLABEL, f->lp);
	penColor(job, n);
    }

    coord = ND_coord(n);
    for (i = 0; i < f->n_flds; i++) {
	if (i > 0) {
	    if (f->LR) {
		AF[0] = f->fld[i]->b.LL;
		AF[1].x = AF[0].x;
		AF[1].y = f->fld[i]->b.UR.y;
	    } else {
		AF[1] = f->fld[i]->b.UR;
		AF[0].x = f->fld[i]->b.LL.x;
		AF[0].y = AF[1].y;
	    }
	    AF[0] = add_pointf(AF[0], coord);
	    AF[1] = add_pointf(AF[1], coord);
	    gvrender_polyline(job, AF, 2);
	}
	gen_fields(job, n, f->fld[i]);
    }
}