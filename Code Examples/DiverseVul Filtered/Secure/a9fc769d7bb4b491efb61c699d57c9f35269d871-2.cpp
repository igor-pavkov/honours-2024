proto_registrar_dump_fields(void)
{
	header_field_info *hfinfo, *parent_hfinfo;
	int		   i, len;
	const char	  *enum_name;
	const char	  *base_name;
	const char	  *blurb;
	char		   width[5];

	len = gpa_hfinfo.len;
	for (i = 0; i < len ; i++) {
		if (gpa_hfinfo.hfi[i] == NULL)
			continue; /* This is a deregistered protocol or header field */

		PROTO_REGISTRAR_GET_NTH(i, hfinfo);

		/*
		 * Skip the pseudo-field for "proto_tree_add_text()" since
		 * we don't want it in the list of filterable fields.
		 */
		if (hfinfo->id == hf_text_only)
			continue;

		/* format for protocols */
		if (proto_registrar_is_protocol(i)) {
			ws_debug_printf("P\t%s\t%s\n", hfinfo->name, hfinfo->abbrev);
		}
		/* format for header fields */
		else {
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

			PROTO_REGISTRAR_GET_NTH(hfinfo->parent, parent_hfinfo);

			enum_name = ftype_name(hfinfo->type);
			base_name = "";

			if (hfinfo->type == FT_CHAR  ||
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
			    hfinfo->type == FT_INT40 ||
			    hfinfo->type == FT_INT48 ||
			    hfinfo->type == FT_INT56 ||
			    hfinfo->type == FT_INT64) {

				switch (FIELD_DISPLAY(hfinfo->display)) {
					case BASE_NONE:
					case BASE_DEC:
					case BASE_HEX:
					case BASE_OCT:
					case BASE_DEC_HEX:
					case BASE_HEX_DEC:
					case BASE_CUSTOM:
					case BASE_PT_UDP:
					case BASE_PT_TCP:
					case BASE_PT_DCCP:
					case BASE_PT_SCTP:
					case BASE_OUI:
						base_name = val_to_str_const(FIELD_DISPLAY(hfinfo->display), hf_display, "????");
						break;
					default:
						base_name = "????";
						break;
				}
			} else if (hfinfo->type == FT_BOOLEAN) {
				/* For FT_BOOLEAN: 'display' can be "parent bitfield width" */
				g_snprintf(width, sizeof(width), "%d", hfinfo->display);
				base_name = width;
			}

			blurb = hfinfo->blurb;
			if (blurb == NULL)
				blurb = "";
			else if (strlen(blurb) == 0)
				blurb = "\"\"";

			ws_debug_printf("F\t%s\t%s\t%s\t%s\t%s\t0x%" G_GINT64_MODIFIER "x\t%s\n",
				hfinfo->name, hfinfo->abbrev, enum_name,
				parent_hfinfo->abbrev, base_name,
				hfinfo->bitmask, blurb);
		}
	}
}