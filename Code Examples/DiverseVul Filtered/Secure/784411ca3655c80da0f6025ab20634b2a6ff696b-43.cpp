static char *closestSide(node_t * n, node_t * other, port * oldport)
{
    boxf b;
    int rkd = GD_rankdir(agraphof(n)->root);
    point p = { 0, 0 };
    point pt = cvtPt(ND_coord(n), rkd);
    point opt = cvtPt(ND_coord(other), rkd);
    int sides = oldport->side;
    char *rv = NULL;
    int i, d, mind = 0;

    if ((sides == 0) || (sides == (TOP | BOTTOM | LEFT | RIGHT)))
	return rv;		/* use center */

    if (oldport->bp) {
	b = *oldport->bp;
    } else {
	if (GD_flip(agraphof(n))) {
	    b.UR.x = ND_ht(n) / 2;
	    b.LL.x = -b.UR.x;
	    b.UR.y = ND_lw(n);
	    b.LL.y = -b.UR.y;
	} else {
	    b.UR.y = ND_ht(n) / 2;
	    b.LL.y = -b.UR.y;
	    b.UR.x = ND_lw(n);
	    b.LL.x = -b.UR.x;
	}
    }

    for (i = 0; i < 4; i++) {
	if ((sides & (1 << i)) == 0)
	    continue;
	switch (i) {
	case 0:
	    p.y = b.LL.y;
	    p.x = (b.LL.x + b.UR.x) / 2;
	    break;
	case 1:
	    p.x = b.UR.x;
	    p.y = (b.LL.y + b.UR.y) / 2;
	    break;
	case 2:
	    p.y = b.UR.y;
	    p.x = (b.LL.x + b.UR.x) / 2;
	    break;
	case 3:
	    p.x = b.LL.x;
	    p.y = (b.LL.y + b.UR.y) / 2;
	    break;
	}
	p.x += pt.x;
	p.y += pt.y;
	d = DIST2(p, opt);
	if (!rv || (d < mind)) {
	    mind = d;
	    rv = side_port[i];
	}
    }
    return rv;
}