static void lsr_read_byte_align_string_list(GF_LASeRCodec *lsr, GF_List *l, const char *name, Bool is_iri, Bool is_font)
{
	XMLRI *iri;
	char *text, *sep, *sep2, *cur;
	while (gf_list_count(l)) {
		char *str = (char *)gf_list_last(l);
		gf_list_rem_last(l);
		gf_free(str);
	}
	text = NULL;
	lsr_read_byte_align_string(lsr, &text, name);
	cur = text;
	while (cur) {
		sep = strchr(cur, '\'');
		if (!sep && is_font) {
			sep = strchr(cur, ',');
			if (!sep) sep = strchr(cur, ';');
		}
		if (!sep) {
			if (is_iri) {
				GF_SAFEALLOC(iri, XMLRI);
				if (iri) {
					iri->string = gf_strdup(cur);
					iri->type = XMLRI_STRING;
					gf_list_add(l, iri);
				} else {
					lsr->last_error = GF_OUT_OF_MEM;
				}
			} else {
				gf_list_add(l, gf_strdup(cur));
			}
			break;
		}
		sep2 = strchr(sep + 1, '\'');
		if (!sep2 && !is_font) {
			if (is_iri) {
				GF_SAFEALLOC(iri, XMLRI);
				if (iri) {
					iri->string = gf_strdup(cur);
					iri->type = XMLRI_STRING;
					gf_list_add(l, iri);
				} else {
					lsr->last_error = GF_OUT_OF_MEM;
				}
			} else {
				gf_list_add(l, gf_strdup(cur));
			}
			break;
		}
		if (sep2)
			sep2[0] = 0;
		else
			sep[0] = 0;
		if (is_iri) {
			GF_SAFEALLOC(iri, XMLRI);
			if (iri) {
				iri->string = gf_strdup(sep+1);
				iri->type = XMLRI_STRING;
				gf_list_add(l, iri);
			} else {
				lsr->last_error = GF_OUT_OF_MEM;
			}
		} else {
			gf_list_add(l, gf_strdup(sep+1));
		}
		if (sep2) {
			sep2[0] = '\'';
			cur = sep2 + 1;
		} else {
			sep[0] = ';';
			cur = sep + 1;
		}
	}
	gf_free(text);
}