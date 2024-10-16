void proto_free_field_strings (ftenum_t field_type, unsigned int field_display, const void *field_strings)
{
	if (field_strings == NULL) {
		return;
	}

	switch (field_type) {
		case FT_FRAMENUM:
			/* This is just an integer represented as a pointer */
			break;
		case FT_PROTOCOL: {
			protocol_t *protocol = (protocol_t *)field_strings;
			g_free((gchar *)protocol->short_name);
			break;
		}
		case FT_BOOLEAN: {
			true_false_string *tf = (true_false_string *)field_strings;
			g_free((gchar *)tf->true_string);
			g_free((gchar *)tf->false_string);
			break;
		}
		case FT_UINT40:
		case FT_INT40:
		case FT_UINT48:
		case FT_INT48:
		case FT_UINT56:
		case FT_INT56:
		case FT_UINT64:
		case FT_INT64: {
			/*
			 * XXX - if it's BASE_RANGE_STRING, or
			 * BASE_EXT_STRING, should we free it?
			 */
			if (field_display & BASE_UNIT_STRING) {
				unit_name_string *unit = (unit_name_string *)field_strings;
				g_free((gchar *)unit->singular);
				g_free((gchar *)unit->plural);
			} else {
				val64_string *vs64 = (val64_string *)field_strings;
				while (vs64->strptr) {
					g_free((gchar *)vs64->strptr);
					vs64++;
				}
			}
			break;
		}
		case FT_CHAR:
		case FT_UINT8:
		case FT_INT8:
		case FT_UINT16:
		case FT_INT16:
		case FT_UINT24:
		case FT_INT24:
		case FT_UINT32:
		case FT_INT32:
		case FT_FLOAT:
		case FT_DOUBLE: {
			/*
			 * XXX - if it's BASE_RANGE_STRING, or
			 * BASE_EXT_STRING, should we free it?
			 */
			if (field_display & BASE_UNIT_STRING) {
				unit_name_string *unit = (unit_name_string *)field_strings;
				g_free((gchar *)unit->singular);
				g_free((gchar *)unit->plural);
			} else if (field_display & BASE_RANGE_STRING) {
				range_string *rs = (range_string *)field_strings;
				while (rs->strptr) {
					g_free((gchar *)rs->strptr);
					rs++;
				}
			} else {
				value_string *vs = (value_string *)field_strings;
				while (vs->strptr) {
					g_free((gchar *)vs->strptr);
					vs++;
				}
			}
			break;
		default:
			break;
		}
	}

	if (field_type != FT_FRAMENUM) {
		g_free((void *)field_strings);
	}
}