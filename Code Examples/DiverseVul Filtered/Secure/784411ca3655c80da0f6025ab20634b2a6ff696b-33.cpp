static char **checkStyle(node_t * n, int *flagp)
{
    char *style;
    char **pstyle = 0;
    int istyle = 0;
    polygon_t *poly;

    style = late_nnstring(n, N_style, "");
    if (style[0]) {
	char **pp;
	char **qp;
	char *p;
	pp = pstyle = parse_style(style);
	while ((p = *pp)) {
	    if (streq(p, "filled")) {
		istyle |= FILLED;
		pp++;
	    } else if (streq(p, "rounded")) {
		istyle |= ROUNDED;
		qp = pp;	/* remove rounded from list passed to renderer */
		do {
		    qp++;
		    *(qp - 1) = *qp;
		} while (*qp);
	    } else if (streq(p, "diagonals")) {
		istyle |= DIAGONALS;
		qp = pp;	/* remove diagonals from list passed to renderer */
		do {
		    qp++;
		    *(qp - 1) = *qp;
		} while (*qp);
	    } else if (streq(p, "invis")) {
		istyle |= INVISIBLE;
		pp++;
	    } else if (streq(p, "radial")) {
		istyle |= (RADIAL|FILLED);
		qp = pp;	/* remove radial from list passed to renderer */
		do {
		    qp++;
		    *(qp - 1) = *qp;
		} while (*qp);
	    } else if (streq(p, "striped") && isBox(n)) {
		istyle |= STRIPED;
		qp = pp;	/* remove striped from list passed to renderer */
		do {
		    qp++;
		    *(qp - 1) = *qp;
		} while (*qp);
	    } else if (streq(p, "wedged") && isEllipse(n)) {
		istyle |= WEDGED;
		qp = pp;	/* remove wedged from list passed to renderer */
		do {
		    qp++;
		    *(qp - 1) = *qp;
		} while (*qp);
	    } else
		pp++;
	}
    }
    if ((poly = ND_shape(n)->polygon))
	istyle |= poly->option;

    *flagp = istyle;
    return pstyle;
}