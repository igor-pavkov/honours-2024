static void header(RBinFile *bf) {
	r_return_if_fail (bf && bf->o);

	RCoreSymCacheElement *element = bf->o->bin_obj;
	if (!element) {
		return;
	}

	RBin *bin = bf->rbin;
	PrintfCallback p = bin->cb_printf;
	PJ *pj = pj_new ();
	if (!pj) {
		return;
	}

	pj_o (pj);
	pj_kn (pj, "cs_version", element->hdr->version);
	pj_kn (pj, "size", element->hdr->size);
	if (element->file_name) {
		pj_ks (pj, "name", element->file_name);
	}
	if (element->binary_version) {
		pj_ks (pj, "version", element->binary_version);
	}
	char uuidstr[R_UUID_LENGTH];
	r_hex_bin2str (element->hdr->uuid, 16, uuidstr);
	pj_ks (pj, "uuid", uuidstr);
	pj_kn (pj, "segments", element->hdr->n_segments);
	pj_kn (pj, "sections", element->hdr->n_sections);
	pj_kn (pj, "symbols", element->hdr->n_symbols);
	pj_kn (pj, "lined_symbols", element->hdr->n_lined_symbols);
	pj_kn (pj, "line_info", element->hdr->n_line_info);
	pj_end (pj);

	p ("%s\n", pj_string (pj));
	pj_free (pj);
}