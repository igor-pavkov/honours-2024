proto_item_fill_display_label(field_info *finfo, gchar *display_label_str, const int label_str_size)
{
	header_field_info *hfinfo = finfo->hfinfo;
	int label_len = 0;
	char *tmp_str;
	guint8 *bytes;
	guint32 number;
	guint64 number64;
	const true_false_string  *tfstring;
	const char *hf_str_val;
	char number_buf[48];
	const char *number_out;
	address addr;
	ws_in4_addr ipv4;
	ws_in6_addr *ipv6;

	switch (hfinfo->type) {

		case FT_NONE:
		case FT_PROTOCOL:
			/* prevent multiple check marks by setting result directly */
			return protoo_strlcpy(display_label_str, UTF8_CHECK_MARK, label_str_size);
			break;

		case FT_UINT_BYTES:
		case FT_BYTES:
			tmp_str = hfinfo_format_bytes(NULL,
				hfinfo,
				(guint8 *)fvalue_get(&finfo->value),
				fvalue_length(&finfo->value));
			label_len = protoo_strlcpy(display_label_str, tmp_str, label_str_size);
			wmem_free(NULL, tmp_str);
			break;

		case FT_ABSOLUTE_TIME:
			tmp_str = abs_time_to_str(NULL, (const nstime_t *)fvalue_get(&finfo->value), (absolute_time_display_e)hfinfo->display, TRUE);
			label_len = protoo_strlcpy(display_label_str, tmp_str, label_str_size);
			wmem_free(NULL, tmp_str);
			break;

		case FT_RELATIVE_TIME:
			tmp_str = rel_time_to_secs_str(NULL, (const nstime_t *)fvalue_get(&finfo->value));
			label_len = protoo_strlcpy(display_label_str, tmp_str, label_str_size);
			wmem_free(NULL, tmp_str);
			break;

		case FT_BOOLEAN:
			number64 = fvalue_get_uinteger64(&finfo->value);
			tfstring = &tfs_true_false;
			if (hfinfo->strings) {
				tfstring = (const struct true_false_string*) hfinfo->strings;
			}
			label_len = protoo_strlcpy(display_label_str,
					tfs_get_string(!!number64, tfstring), label_str_size);
			break;

		case FT_CHAR:
			number = fvalue_get_uinteger(&finfo->value);

			if (FIELD_DISPLAY(hfinfo->display) == BASE_CUSTOM) {
				gchar tmp[ITEM_LABEL_LENGTH];
				custom_fmt_func_t fmtfunc = (custom_fmt_func_t)hfinfo->strings;

				DISSECTOR_ASSERT(fmtfunc);
				fmtfunc(tmp, number);

				label_len = protoo_strlcpy(display_label_str, tmp, label_str_size);

			} else if (hfinfo->strings) {
				number_out = hf_try_val_to_str(number, hfinfo);

				if (!number_out) {
					number_out = hfinfo_char_value_format_display(BASE_HEX, number_buf, number);
				}

				label_len = protoo_strlcpy(display_label_str, number_out, label_str_size);

			} else {
				number_out = hfinfo_char_value_format(hfinfo, number_buf, number);

				label_len = protoo_strlcpy(display_label_str, number_out, label_str_size);
			}

			break;

		/* XXX - make these just FT_NUMBER? */
		case FT_INT8:
		case FT_INT16:
		case FT_INT24:
		case FT_INT32:
		case FT_UINT8:
		case FT_UINT16:
		case FT_UINT24:
		case FT_UINT32:
		case FT_FRAMENUM:
			hf_str_val = NULL;
			number = IS_FT_INT(hfinfo->type) ?
				(guint32) fvalue_get_sinteger(&finfo->value) :
				fvalue_get_uinteger(&finfo->value);

			if (FIELD_DISPLAY(hfinfo->display) == BASE_CUSTOM) {
				gchar tmp[ITEM_LABEL_LENGTH];
				custom_fmt_func_t fmtfunc = (custom_fmt_func_t)hfinfo->strings;

				DISSECTOR_ASSERT(fmtfunc);
				fmtfunc(tmp, number);

				label_len = protoo_strlcpy(display_label_str, tmp, label_str_size);

			} else if (hfinfo->strings && hfinfo->type != FT_FRAMENUM) {
				if (hfinfo->display & BASE_UNIT_STRING) {
					number_out = hfinfo_numeric_value_format(hfinfo, number_buf, number);
					label_len = protoo_strlcpy(display_label_str, number_out, label_str_size);
					hf_str_val = hf_try_val_to_str(number, hfinfo);
					label_len += protoo_strlcpy(display_label_str+label_len, hf_str_val, label_str_size-label_len);
				} else {
					number_out = hf_str_val = hf_try_val_to_str(number, hfinfo);

					if (!number_out) {
						number_out = hfinfo_number_value_format_display(hfinfo, hfinfo->display, number_buf, number);
					}

					label_len = protoo_strlcpy(display_label_str, number_out, label_str_size);
				}
			} else {
				number_out = hfinfo_number_value_format(hfinfo, number_buf, number);

				label_len = protoo_strlcpy(display_label_str, number_out, label_str_size);
			}

			break;

		case FT_INT40:
		case FT_INT48:
		case FT_INT56:
		case FT_INT64:
		case FT_UINT40:
		case FT_UINT48:
		case FT_UINT56:
		case FT_UINT64:
			hf_str_val = NULL;
			number64 = IS_FT_INT(hfinfo->type) ?
				(guint64) fvalue_get_sinteger64(&finfo->value) :
				fvalue_get_uinteger64(&finfo->value);

			if (FIELD_DISPLAY(hfinfo->display) == BASE_CUSTOM) {
				gchar tmp[ITEM_LABEL_LENGTH];
				custom_fmt_func_64_t fmtfunc64 = (custom_fmt_func_64_t)hfinfo->strings;

				DISSECTOR_ASSERT(fmtfunc64);
				fmtfunc64(tmp, number64);

				label_len = protoo_strlcpy(display_label_str, tmp, label_str_size);
			} else if (hfinfo->strings) {
				if (hfinfo->display & BASE_UNIT_STRING) {
					number_out = hfinfo_numeric_value_format64(hfinfo, number_buf, number64);
					label_len = protoo_strlcpy(display_label_str, number_out, label_str_size);
					hf_str_val = hf_try_val64_to_str(number64, hfinfo);
					label_len += protoo_strlcpy(display_label_str+label_len, hf_str_val, label_str_size-label_len);
				} else {
					number_out = hf_str_val = hf_try_val64_to_str(number64, hfinfo);

					if (!number_out)
						number_out = hfinfo_number_value_format_display64(hfinfo, hfinfo->display, number_buf, number64);

					label_len = protoo_strlcpy(display_label_str, number_out, label_str_size);
				}
			} else {
				number_out = hfinfo_number_value_format64(hfinfo, number_buf, number64);

				label_len = protoo_strlcpy(display_label_str, number_out, label_str_size);
			}

			break;

		case FT_EUI64:
			tmp_str = eui64_to_str(NULL, fvalue_get_uinteger64(&finfo->value));
			label_len = protoo_strlcpy(display_label_str, tmp_str, label_str_size);
			wmem_free(NULL, tmp_str);
			break;

		case FT_IPv4:
			ipv4 = fvalue_get_uinteger(&finfo->value);
			set_address (&addr, AT_IPv4, 4, &ipv4);
			address_to_str_buf(&addr, display_label_str, label_str_size);
			label_len = (int)strlen(display_label_str);
			break;

		case FT_IPv6:
			ipv6 = (ws_in6_addr *)fvalue_get(&finfo->value);
			set_address (&addr, AT_IPv6, sizeof(ws_in6_addr), ipv6);
			address_to_str_buf(&addr, display_label_str, label_str_size);
			label_len = (int)strlen(display_label_str);
			break;

		case FT_FCWWN:
			set_address (&addr, AT_FCWWN, FCWWN_ADDR_LEN, fvalue_get(&finfo->value));
			address_to_str_buf(&addr, display_label_str, label_str_size);
			label_len = (int)strlen(display_label_str);
			break;

		case FT_ETHER:
			set_address (&addr, AT_ETHER, FT_ETHER_LEN, fvalue_get(&finfo->value));
			address_to_str_buf(&addr, display_label_str, label_str_size);
			label_len = (int)strlen(display_label_str);
			break;

		case FT_GUID:
			tmp_str = guid_to_str(NULL, (e_guid_t *)fvalue_get(&finfo->value));
			label_len = protoo_strlcpy(display_label_str, tmp_str, label_str_size);
			wmem_free(NULL, tmp_str);
			break;

		case FT_REL_OID:
			bytes = (guint8 *)fvalue_get(&finfo->value);
			tmp_str = rel_oid_resolved_from_encoded(NULL, bytes, fvalue_length(&finfo->value));
			label_len = protoo_strlcpy(display_label_str, tmp_str, label_str_size);
			wmem_free(NULL, tmp_str);
			break;

		case FT_OID:
			bytes = (guint8 *)fvalue_get(&finfo->value);
			tmp_str = oid_resolved_from_encoded(NULL, bytes, fvalue_length(&finfo->value));
			label_len = protoo_strlcpy(display_label_str, tmp_str, label_str_size);
			wmem_free(NULL, tmp_str);
			break;

		case FT_SYSTEM_ID:
			bytes = (guint8 *)fvalue_get(&finfo->value);
			tmp_str = print_system_id(NULL, bytes, fvalue_length(&finfo->value));
			label_len = protoo_strlcpy(display_label_str, tmp_str, label_str_size);
			wmem_free(NULL, tmp_str);
			break;

		case FT_FLOAT:
			if (hfinfo->display & BASE_UNIT_STRING) {
				double d_value = fvalue_get_floating(&finfo->value);
				g_snprintf(display_label_str, label_str_size,
						"%." G_STRINGIFY(FLT_DIG) "g%s", d_value,
						unit_name_string_get_double(d_value, (const unit_name_string*)hfinfo->strings));
			} else {
				g_snprintf(display_label_str, label_str_size,
						"%." G_STRINGIFY(FLT_DIG) "g", fvalue_get_floating(&finfo->value));
			}
			label_len = (int)strlen(display_label_str);
			break;

		case FT_DOUBLE:
			if (hfinfo->display & BASE_UNIT_STRING) {
				double d_value = fvalue_get_floating(&finfo->value);
				g_snprintf(display_label_str, label_str_size,
						"%." G_STRINGIFY(DBL_DIG) "g%s", d_value,
						unit_name_string_get_double(d_value, (const unit_name_string*)hfinfo->strings));
			} else {
				g_snprintf(display_label_str, label_str_size,
						"%." G_STRINGIFY(DBL_DIG) "g", fvalue_get_floating(&finfo->value));
			}
			label_len = (int)strlen(display_label_str);
			break;

		case FT_STRING:
		case FT_STRINGZ:
		case FT_UINT_STRING:
		case FT_STRINGZPAD:
		case FT_STRINGZTRUNC:
			bytes = (guint8 *)fvalue_get(&finfo->value);
			tmp_str = hfinfo_format_text(NULL, hfinfo, bytes);
			label_len = protoo_strlcpy(display_label_str, tmp_str, label_str_size);
			wmem_free(NULL, tmp_str);
			break;

		default:
			/* First try ftype string representation */
			tmp_str = fvalue_to_string_repr(NULL, &finfo->value, FTREPR_DISPLAY, hfinfo->display);
			if (!tmp_str) {
				/* Default to show as bytes */
				bytes = (guint8 *)fvalue_get(&finfo->value);
				tmp_str = bytes_to_str(NULL, bytes, fvalue_length(&finfo->value));
			}
			label_len = protoo_strlcpy(display_label_str, tmp_str, label_str_size);
			wmem_free(NULL, tmp_str);
			break;
	}
	return label_len;
}