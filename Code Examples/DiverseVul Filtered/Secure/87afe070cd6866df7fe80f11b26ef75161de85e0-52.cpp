static GF_Err do_itunes_tag()
{
	GF_Err e;
	char *itunes_data = NULL;
	char *tags = itunes_tags;

	if (gf_file_exists(itunes_tags)) {
		u32 len;
		e = gf_file_load_data(itunes_tags, (u8 **) &itunes_data, &len);
		if (e) return e;;
		tags = itunes_data;
	}

	while (tags) {
		char *val;
		Bool clear = GF_FALSE;
		Bool is_wma = GF_FALSE;
		u32 tlen, tagtype=0, itag = 0;
		s32 tag_idx;
		char *sep = itunes_data ? strchr(tags, '\n') : gf_url_colon_suffix(tags);
		while (sep) {
			char *eq = strchr(sep+1, '=');
			if (eq) eq[0] = 0;
			s32 next_tag_idx = gf_itags_find_by_name(sep+1);
			if ((next_tag_idx<0) && strlen(sep+1)==4)
				next_tag_idx = 0;

			if (eq) eq[0] = '=';
			if (next_tag_idx>=0) {
				sep[0] = 0;
				break;
			}
			sep = itunes_data ? strchr(sep+1, '\n') : gf_url_colon_suffix(sep+1);
		}
		val = strchr(tags, '=');
		if (val) val[0] = 0;
		if (!strcmp(tags, "clear") || !strcmp(tags, "reset")) {
			clear = GF_TRUE;
		} else if (!strncmp(tags, "WM/", 3) ) {
			is_wma = GF_TRUE;
		} else {
			tag_idx = gf_itags_find_by_name(tags);
			if (tag_idx<0) {
				if (strlen(tags)==4) {
					itag = GF_4CC(tags[0], tags[1], tags[2], tags[3]);
					tagtype = GF_ITAG_STR;
				} else if (strlen(tags)==3) {
					itag = GF_4CC(0xA9, tags[0], tags[1], tags[2]);
					tagtype = GF_ITAG_STR;
				}
			}
		}
		if (val) {
			val[0] = '=';
			val++;
		}
		if (!itag && !clear && !is_wma) {
			if (tag_idx<0) {
				M4_LOG(GF_LOG_WARNING, ("Invalid iTune tag name \"%s\" - ignoring\n", tags));
				break;
			}
			itag = gf_itags_get_itag(tag_idx);
			tagtype = gf_itags_get_type(tag_idx);
		}
		if (!val || (val[0]==':') || !val[0] || !stricmp(val, "NULL") ) val = NULL;

		tlen = val ? (u32) strlen(val) : 0;
		if (clear) {
			e = gf_isom_apple_set_tag(file, GF_ISOM_ITUNE_RESET, NULL, 0, 0, 0);
		}
		else if (is_wma) {
			if (val) val[-1] = 0;
			e = gf_isom_wma_set_tag(file, tags, val);
			if (val) val[-1] = '=';
		}
		else if (val && (tagtype==GF_ITAG_FILE)) {
			u32 flen = (u32) strlen(val);
			u8 *d=NULL;
			while (flen && val[flen-1]=='\n') flen--;
			val[flen] = 0;
			e = gf_file_load_data(val, (u8 **) &d, &tlen);
			val[flen] = '\n';

			if (!e)
				e = gf_isom_apple_set_tag(file, itag, d, tlen, 0, 0);

			if (d) gf_free(d);
		} else {
			e = gf_isom_apple_set_tag(file, itag, (u8 *) val, tlen, 0, 0);
		}
		if (e) {
			M4_LOG(GF_LOG_ERROR, ("Error assigning tag %s: %s\n", tags, gf_error_to_string(e) ));
		}

		do_save = GF_TRUE;

		if (sep) {
			sep[0] = itunes_data ? '\n' : ':';
			tags = sep+1;
		} else {
			tags = NULL;
		}
	}
	if (itunes_data) gf_free(itunes_data);
	return GF_OK;
}