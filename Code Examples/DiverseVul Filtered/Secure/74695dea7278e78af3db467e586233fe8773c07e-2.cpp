GF_Err nhmldmx_configure_pid(GF_Filter *filter, GF_FilterPid *pid, Bool is_remove)
{
	const GF_PropertyValue *p;
	GF_NHMLDmxCtx *ctx = gf_filter_get_udta(filter);

	if (is_remove) {
		ctx->ipid = NULL;
		//gf_filter_pid_remove(st->opid);

		return GF_OK;
	}
	if (! gf_filter_pid_check_caps(pid))
		return GF_NOT_SUPPORTED;

	ctx->ipid = pid;
	gf_filter_pid_set_framing_mode(pid, GF_TRUE);

	p = gf_filter_pid_get_property(pid, GF_PROP_PID_MIME);
	if (p && p->value.string && strstr(p->value.string, "dims")) ctx->is_dims = GF_TRUE;
	else {
		p = gf_filter_pid_get_property(pid, GF_PROP_PID_FILE_EXT);
		if (p && p->value.string && strstr(p->value.string, "dims")) ctx->is_dims = GF_TRUE;
	}

	return GF_OK;
}