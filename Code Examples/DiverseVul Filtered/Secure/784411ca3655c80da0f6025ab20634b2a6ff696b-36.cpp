shape_kind shapeOf(node_t * n)
{
    shape_desc *sh = ND_shape(n);
    void (*ifn) (node_t *);

    if (!sh)
	return SH_UNSET;
    ifn = ND_shape(n)->fns->initfn;
    if (ifn == poly_init)
	return SH_POLY;
    else if (ifn == record_init)
	return SH_RECORD;
    else if (ifn == point_init)
	return SH_POINT;
    else if (ifn == epsf_init)
	return SH_EPSF;
    else
	return SH_UNSET;
}