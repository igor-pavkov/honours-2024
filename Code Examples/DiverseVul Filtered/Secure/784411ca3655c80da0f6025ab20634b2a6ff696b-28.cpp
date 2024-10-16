static void resize_reclbl(field_t * f, pointf sz, int nojustify_p)
{
    int i, amt;
    double inc;
    pointf d;
    pointf newsz;
    field_t *sf;

    /* adjust field */
    d.x = sz.x - f->size.x;
    d.y = sz.y - f->size.y;
    f->size = sz;

    /* adjust text area */
    if (f->lp && !nojustify_p) {
	f->lp->space.x += d.x;
	f->lp->space.y += d.y;
    }

    /* adjust children */
    if (f->n_flds) {

	if (f->LR)
	    inc = d.x / f->n_flds;
	else
	    inc = d.y / f->n_flds;
	for (i = 0; i < f->n_flds; i++) {
	    sf = f->fld[i];
	    amt = ((int) ((i + 1) * inc)) - ((int) (i * inc));
	    if (f->LR)
		newsz = pointfof(sf->size.x + amt, sz.y);
	    else
		newsz = pointfof(sz.x, sf->size.y + amt);
	    resize_reclbl(sf, newsz, nojustify_p);
	}
    }
}