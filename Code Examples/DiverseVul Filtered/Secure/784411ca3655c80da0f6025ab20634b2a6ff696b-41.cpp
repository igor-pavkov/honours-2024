static int record_path(node_t * n, port * prt, int side, boxf rv[],
		       int *kptr)
{
    int i, ls, rs;
    pointf p;
    field_t *info;

    if (!prt->defined)
	return 0;
    p = prt->p;
    info = (field_t *) ND_shape_info(n);

    for (i = 0; i < info->n_flds; i++) {
	if (!GD_flip(agraphof(n))) {
	    ls = info->fld[i]->b.LL.x;
	    rs = info->fld[i]->b.UR.x;
	} else {
	    ls = info->fld[i]->b.LL.y;
	    rs = info->fld[i]->b.UR.y;
	}
	if (BETWEEN(ls, p.x, rs)) {
	    /* FIXME: I don't understand this code */
	    if (GD_flip(agraphof(n))) {
		rv[0] = flip_rec_boxf(info->fld[i]->b, ND_coord(n));
	    } else {
		rv[0].LL.x = ND_coord(n).x + ls;
		rv[0].LL.y = ND_coord(n).y - (ND_ht(n) / 2);
		rv[0].UR.x = ND_coord(n).x + rs;
	    }
	    rv[0].UR.y = ND_coord(n).y + (ND_ht(n) / 2);
	    *kptr = 1;
	    break;
	}
    }
    return side;
}