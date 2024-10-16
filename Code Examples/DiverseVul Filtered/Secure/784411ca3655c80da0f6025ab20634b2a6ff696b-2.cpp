static void pos_reclbl(field_t * f, pointf ul, int sides)
{
    int i, last, mask;

    f->sides = sides;
    f->b.LL = pointfof(ul.x, ul.y - f->size.y);
    f->b.UR = pointfof(ul.x + f->size.x, ul.y);
    last = f->n_flds - 1;
    for (i = 0; i <= last; i++) {
	if (sides) {
	    if (f->LR) {
		if (i == 0) {
		    if (i == last)
			mask = TOP | BOTTOM | RIGHT | LEFT;
		    else
			mask = TOP | BOTTOM | LEFT;
		} else if (i == last)
		    mask = TOP | BOTTOM | RIGHT;
		else
		    mask = TOP | BOTTOM;
	    } else {
		if (i == 0) {
		    if (i == last)
			mask = TOP | BOTTOM | RIGHT | LEFT;
		    else
			mask = TOP | RIGHT | LEFT;
		} else if (i == last)
		    mask = LEFT | BOTTOM | RIGHT;
		else
		    mask = LEFT | RIGHT;
	    }
	} else
	    mask = 0;
	pos_reclbl(f->fld[i], ul, sides & mask);
	if (f->LR)
	    ul.x = ul.x + f->fld[i]->size.x;
	else
	    ul.y = ul.y - f->fld[i]->size.y;
    }
}