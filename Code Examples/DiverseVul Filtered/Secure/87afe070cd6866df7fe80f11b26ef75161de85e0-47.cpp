Bool mp4box_check_isom_fileopt(char *opt)
{
	GF_GPACArg *arg = NULL;
	u32 i=0;

	while (m4b_imp_fileopt_args[i].name) {
		arg = (GF_GPACArg *) &m4b_imp_fileopt_args[i];
		i++;
		if (!stricmp(arg->name, opt)) break;
		arg = NULL;
	}
	if (!arg) {
		fprintf(stderr, "Option %s not described in doc, please report to GPAC devs!\n", opt);
		return GF_FALSE;
	}
	if (arg->description[0] != '`')
		return GF_FALSE;
	const char *d = arg->description+1;
	while (d[0] != '`') {
		if (d[0]=='S') return GF_TRUE;
		if (d[0]=='C') return GF_TRUE;
		d++;
	}
	return GF_FALSE;
}