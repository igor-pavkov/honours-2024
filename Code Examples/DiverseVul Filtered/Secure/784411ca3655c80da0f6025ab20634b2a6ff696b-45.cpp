static pointf size_reclbl(node_t * n, field_t * f)
{
    int i;
    char *p;
    double marginx, marginy;
    pointf d, d0;
    pointf dimen;

    if (f->lp) {
	dimen = f->lp->dimen;

	/* minimal whitespace around label */
	if ((dimen.x > 0.0) || (dimen.y > 0.0)) {
	    /* padding */
	    if ((p = agget(n, "margin"))) {
		i = sscanf(p, "%lf,%lf", &marginx, &marginy);
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
	d = dimen;
    } else {
	d.x = d.y = 0;
	for (i = 0; i < f->n_flds; i++) {
	    d0 = size_reclbl(n, f->fld[i]);
	    if (f->LR) {
		d.x += d0.x;
		d.y = MAX(d.y, d0.y);
	    } else {
		d.y += d0.y;
		d.x = MAX(d.x, d0.x);
	    }
	}
    }
    f->size = d;
    return d;
}