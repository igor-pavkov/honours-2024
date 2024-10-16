static jpc_dec_importopts_t *jpc_dec_opts_create(const char *optstr)
{
	jpc_dec_importopts_t *opts;
	jas_tvparser_t *tvp;

	opts = 0;

	if (!(opts = jas_malloc(sizeof(jpc_dec_importopts_t)))) {
		goto error;
	}

	opts->debug = 0;
	opts->maxlyrs = JPC_MAXLYRS;
	opts->maxpkts = -1;
	opts->max_samples = JAS_DEC_DEFAULT_MAX_SAMPLES;

	if (!(tvp = jas_tvparser_create(optstr ? optstr : ""))) {
		goto error;
	}

	while (!jas_tvparser_next(tvp)) {
		switch (jas_taginfo_nonull(jas_taginfos_lookup(decopts,
		  jas_tvparser_gettag(tvp)))->id) {
		case OPT_MAXLYRS:
			opts->maxlyrs = atoi(jas_tvparser_getval(tvp));
			break;
		case OPT_DEBUG:
			opts->debug = atoi(jas_tvparser_getval(tvp));
			break;
		case OPT_MAXPKTS:
			opts->maxpkts = atoi(jas_tvparser_getval(tvp));
			break;
		case OPT_MAXSAMPLES:
			opts->max_samples = strtoull(jas_tvparser_getval(tvp), 0, 10);
			break;
		default:
			jas_eprintf("warning: ignoring invalid option %s\n",
			  jas_tvparser_gettag(tvp));
			break;
		}
	}

	jas_tvparser_destroy(tvp);

	return opts;

error:
	if (opts) {
		jpc_dec_opts_destroy(opts);
	}
	return 0;
}