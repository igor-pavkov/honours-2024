static port poly_port(node_t * n, char *portname, char *compass)
{
    port rv;
    boxf *bp;
    int sides;			/* bitmap of which sides the port lies along */

    if (portname[0] == '\0')
	return Center;

    if (compass == NULL)
	compass = "_";
    sides = BOTTOM | RIGHT | TOP | LEFT;
    if ((ND_label(n)->html) && (bp = html_port(n, portname, &sides))) {
	if (compassPort(n, bp, &rv, compass, sides, NULL)) {
	    agerr(AGWARN,
		  "node %s, port %s, unrecognized compass point '%s' - ignored\n",
		  agnameof(n), portname, compass);
	}
    } else {
	inside_t *ictxtp;
	inside_t ictxt;

	if (IS_BOX(n))
	    ictxtp = NULL;
	else {
	    ictxt.s.n = n;
	    ictxt.s.bp = NULL;
	    ictxtp = &ictxt;
	}
	if (compassPort(n, NULL, &rv, portname, sides, ictxtp))
	    unrecognized(n, portname);
    }

    rv.name = NULL;
    return rv;
}