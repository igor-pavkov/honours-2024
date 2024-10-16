static void epsf_gencode(GVJ_t * job, node_t * n)
{
    obj_state_t *obj = job->obj;
    epsf_t *desc;
    int doMap = (obj->url || obj->explicit_tooltip);

    desc = (epsf_t *) (ND_shape_info(n));
    if (!desc)
	return;

    if (doMap && !(job->flags & EMIT_CLUSTERS_LAST))
	gvrender_begin_anchor(job,
			      obj->url, obj->tooltip, obj->target,
			      obj->id);
    if (desc)
	fprintf(job->output_file,
		"%.5g %.5g translate newpath user_shape_%d\n",
		ND_coord(n).x + desc->offset.x,
		ND_coord(n).y + desc->offset.y, desc->macro_id);
    ND_label(n)->pos = ND_coord(n);

    emit_label(job, EMIT_NLABEL, ND_label(n));
    if (doMap) {
	if (job->flags & EMIT_CLUSTERS_LAST)
	    gvrender_begin_anchor(job,
				  obj->url, obj->tooltip, obj->target,
				  obj->id);
	gvrender_end_anchor(job);
    }
}