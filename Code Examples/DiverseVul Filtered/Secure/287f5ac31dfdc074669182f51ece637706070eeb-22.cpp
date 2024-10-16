static void de_run_pict(deark *c, de_module_params *mparams)
{
	lctx *d = NULL;
	i64 pos = 0;
	i64 picsize;
	struct pict_rect framerect;

	d = de_malloc(c, sizeof(lctx));

	do_detect_version(c, &d->dti, 1);
	if(d->dti.file_version>0) {
		de_declare_fmtf(c, "PICT v%d%s", d->dti.file_version,
			d->dti.has_fileheader?"":", without file header");
	}

	d->version = 1;

	if(d->dti.has_fileheader) {
		pos += 512;
	}

	picsize = de_getu16be(pos);
	de_dbg(c, "picSize: %d", (int)picsize);
	pos+=2;
	pict_read_rect(c->infile, pos, &framerect, "picFrame");
	pos+=8;

	do_read_items(c, d, pos);

	dbuf_close(d->iccprofile_file);
	de_free(c, d);
}