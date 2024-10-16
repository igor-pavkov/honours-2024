static boolean record_inside(inside_t * inside_context, pointf p)
{

    field_t *fld0;
    boxf *bp = inside_context->s.bp;
    node_t *n = inside_context->s.n;
    boxf bbox;

    /* convert point to node coordinate system */
    p = ccwrotatepf(p, 90 * GD_rankdir(agraphof(n)));

    if (bp == NULL) {
	fld0 = (field_t *) ND_shape_info(n);
	bbox = fld0->b;
    } else
	bbox = *bp;

    return INSIDE(p, bbox);
}