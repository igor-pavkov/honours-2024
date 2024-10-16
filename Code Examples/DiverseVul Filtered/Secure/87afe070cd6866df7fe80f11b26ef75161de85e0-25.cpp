static GF_Err xml_bs_to_bin(char *inName, char *outName, u32 dump_std)
{
	GF_Err e;
	GF_XMLNode *root;
	u8 *data = NULL;
	u32 data_size;

	GF_DOMParser *dom = gf_xml_dom_new();
	e = gf_xml_dom_parse(dom, inName, NULL, NULL);
	if (e) {
		gf_xml_dom_del(dom);
		M4_LOG(GF_LOG_ERROR, ("Failed to parse XML file: %s\n", gf_error_to_string(e)));
		return e;
	}
	root = gf_xml_dom_get_root_idx(dom, 0);
	if (!root) {
		gf_xml_dom_del(dom);
		return GF_OK;
	}

	e = gf_xml_parse_bit_sequence(root, inName, &data, &data_size);
	gf_xml_dom_del(dom);

	if (e) {
		M4_LOG(GF_LOG_ERROR, ("Failed to parse binary sequence: %s\n", gf_error_to_string(e)));
		return e;
	}

	if (dump_std) {
		gf_fwrite(data, data_size, stdout);
	} else {
		FILE *t;
		char szFile[GF_MAX_PATH];
		if (outName) {
			strcpy(szFile, outName);
		} else {
			strcpy(szFile, inName);
			strcat(szFile, ".bin");
		}
		t = gf_fopen(szFile, "wb");
		if (!t) {
			M4_LOG(GF_LOG_ERROR, ("Failed to open file %s\n", szFile));
			e = GF_IO_ERR;
		} else {
			if (gf_fwrite(data, data_size, t) != data_size) {
				M4_LOG(GF_LOG_ERROR, ("Failed to write output to file %s\n", szFile));
				e = GF_IO_ERR;
			}
			gf_fclose(t);
		}
	}
	gf_free(data);
	return e;
}