static port record_port(node_t * n, char *portname, char *compass)
{
    field_t *f;
    field_t *subf;
    port rv;
    int sides;			/* bitmap of which sides the port lies along */

    if (portname[0] == '\0')
	return Center;
    sides = BOTTOM | RIGHT | TOP | LEFT;
    if (compass == NULL)
	compass = "_";
    f = (field_t *) ND_shape_info(n);
    if ((subf = map_rec_port(f, portname))) {
	if (compassPort(n, &subf->b, &rv, compass, subf->sides, NULL)) {
	    agerr(AGWARN,
		  "node %s, port %s, unrecognized compass point '%s' - ignored\n",
		  agnameof(n), portname, compass);
	}
    } else if (compassPort(n, &f->b, &rv, portname, sides, NULL)) {
	unrecognized(n, portname);
    }

    return rv;
}