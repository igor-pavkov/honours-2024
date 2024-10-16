proto_registrar_dump_values(void)
{
	header_field_info	*hfinfo;
	int			i, len, vi;
	const value_string	*vals;
	const val64_string	*vals64;
	const range_string	*range;
	const true_false_string	*tfs;
	const unit_name_string	*units;

	len = gpa_hfinfo.len;
	for (i = 0; i < len ; i++) {
		if (gpa_hfinfo.hfi[i] == NULL)
			continue; /* This is a deregistered protocol or field */

		PROTO_REGISTRAR_GET_NTH(i, hfinfo);

		if (hfinfo->id == hf_text_only) {
			continue;
		}

		/* ignore protocols */
		if (proto_registrar_is_protocol(i)) {
			continue;
		}
		/* process header fields */
#if 0 /* XXX: We apparently allow fields with the same name but with differing "strings" content */
		/*
		 * If this field isn't at the head of the list of
		 * fields with this name, skip this field - all
		 * fields with the same name are really just versions
		 * of the same field stored in different bits, and
		 * should have the same type/radix/value list, and
		 * just differ in their bit masks.	(If a field isn't
		 * a bitfield, but can be, say, 1 or 2 bytes long,
		 * it can just be made FT_UINT16, meaning the
		 * *maximum* length is 2 bytes, and be used
		 * for all lengths.)
		 */
		if (hfinfo->same_name_prev_id != -1)
			continue;
#endif
		vals   = NULL;
		vals64 = NULL;
		range  = NULL;
		tfs    = NULL;
		units  = NULL;

		if (hfinfo->strings != NULL) {
			if (FIELD_DISPLAY(hfinfo->display) != BASE_CUSTOM &&
			    (hfinfo->type == FT_CHAR  ||
			     hfinfo->type == FT_UINT8  ||
			     hfinfo->type == FT_UINT16 ||
			     hfinfo->type == FT_UINT24 ||
			     hfinfo->type == FT_UINT32 ||
			     hfinfo->type == FT_UINT40 ||
			     hfinfo->type == FT_UINT48 ||
			     hfinfo->type == FT_UINT56 ||
			     hfinfo->type == FT_UINT64 ||
			     hfinfo->type == FT_INT8   ||
			     hfinfo->type == FT_INT16  ||
			     hfinfo->type == FT_INT24  ||
			     hfinfo->type == FT_INT32  ||
			     hfinfo->type == FT_INT40  ||
			     hfinfo->type == FT_INT48  ||
			     hfinfo->type == FT_INT56  ||
			     hfinfo->type == FT_INT64  ||
			     hfinfo->type == FT_FLOAT  ||
			     hfinfo->type == FT_DOUBLE)) {

				if (hfinfo->display & BASE_RANGE_STRING) {
					range = (const range_string *)hfinfo->strings;
				} else if (hfinfo->display & BASE_EXT_STRING) {
					if (hfinfo->display & BASE_VAL64_STRING) {
						vals64 = VAL64_STRING_EXT_VS_P((const val64_string_ext *)hfinfo->strings);
					} else {
						vals = VALUE_STRING_EXT_VS_P((const value_string_ext *)hfinfo->strings);
					}
				} else if (hfinfo->display & BASE_VAL64_STRING) {
					vals64 = (const val64_string *)hfinfo->strings;
				} else if (hfinfo->display & BASE_UNIT_STRING) {
					units = (const unit_name_string *)hfinfo->strings;
				} else {
					vals = (const value_string *)hfinfo->strings;
				}
			}
			else if (hfinfo->type == FT_BOOLEAN) {
				tfs = (const struct true_false_string *)hfinfo->strings;
			}
		}

		/* Print value strings? */
		if (vals) {
			if (hfinfo->display & BASE_EXT_STRING) {
				if (hfinfo->display & BASE_VAL64_STRING) {
					val64_string_ext *vse_p = (val64_string_ext *)hfinfo->strings;
					if (!val64_string_ext_validate(vse_p)) {
						g_warning("Invalid val64_string_ext ptr for: %s", hfinfo->abbrev);
						continue;
					}
					try_val64_to_str_ext(0, vse_p); /* "prime" the extended val64_string */
					ws_debug_printf("E\t%s\t%u\t%s\t%s\n",
					       hfinfo->abbrev,
					       VAL64_STRING_EXT_VS_NUM_ENTRIES(vse_p),
					       VAL64_STRING_EXT_VS_NAME(vse_p),
					       val64_string_ext_match_type_str(vse_p));
				} else {
					value_string_ext *vse_p = (value_string_ext *)hfinfo->strings;
					if (!value_string_ext_validate(vse_p)) {
						g_warning("Invalid value_string_ext ptr for: %s", hfinfo->abbrev);
						continue;
					}
					try_val_to_str_ext(0, vse_p); /* "prime" the extended value_string */
					ws_debug_printf("E\t%s\t%u\t%s\t%s\n",
					       hfinfo->abbrev,
					       VALUE_STRING_EXT_VS_NUM_ENTRIES(vse_p),
					       VALUE_STRING_EXT_VS_NAME(vse_p),
					       value_string_ext_match_type_str(vse_p));
				}
			}
			vi = 0;
			while (vals[vi].strptr) {
				/* Print in the proper base */
				if (hfinfo->type == FT_CHAR) {
					if (g_ascii_isprint(vals[vi].value)) {
						ws_debug_printf("V\t%s\t'%c'\t%s\n",
						       hfinfo->abbrev,
						       vals[vi].value,
						       vals[vi].strptr);
					} else {
						if (hfinfo->display == BASE_HEX) {
							ws_debug_printf("V\t%s\t'\\x%02x'\t%s\n",
							       hfinfo->abbrev,
							       vals[vi].value,
							       vals[vi].strptr);
						}
						else {
							ws_debug_printf("V\t%s\t'\\%03o'\t%s\n",
							       hfinfo->abbrev,
							       vals[vi].value,
							       vals[vi].strptr);
						}
					}
				} else {
					if (hfinfo->display == BASE_HEX) {
						ws_debug_printf("V\t%s\t0x%x\t%s\n",
						       hfinfo->abbrev,
						       vals[vi].value,
						       vals[vi].strptr);
					}
					else {
						ws_debug_printf("V\t%s\t%u\t%s\n",
						       hfinfo->abbrev,
						       vals[vi].value,
						       vals[vi].strptr);
					}
				}
				vi++;
			}
		}
		else if (vals64) {
			vi = 0;
			while (vals64[vi].strptr) {
				ws_debug_printf("V64\t%s\t%" G_GINT64_MODIFIER "u\t%s\n",
				       hfinfo->abbrev,
				       vals64[vi].value,
				       vals64[vi].strptr);
				vi++;
			}
		}

		/* print range strings? */
		else if (range) {
			vi = 0;
			while (range[vi].strptr) {
				/* Print in the proper base */
				if (FIELD_DISPLAY(hfinfo->display) == BASE_HEX) {
					ws_debug_printf("R\t%s\t0x%x\t0x%x\t%s\n",
					       hfinfo->abbrev,
					       range[vi].value_min,
					       range[vi].value_max,
					       range[vi].strptr);
				}
				else {
					ws_debug_printf("R\t%s\t%u\t%u\t%s\n",
					       hfinfo->abbrev,
					       range[vi].value_min,
					       range[vi].value_max,
					       range[vi].strptr);
				}
				vi++;
			}
		}

		/* Print true/false strings? */
		else if (tfs) {
			ws_debug_printf("T\t%s\t%s\t%s\n", hfinfo->abbrev,
			       tfs->true_string, tfs->false_string);
		}
		/* Print unit strings? */
		else if (units) {
			ws_debug_printf("U\t%s\t%s\t%s\n", hfinfo->abbrev,
			       units->singular, units->plural ? units->plural : "(no plural)");
		}
	}
}