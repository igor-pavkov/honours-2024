static void add_assoc_image_info(zval *value, int sub_array, image_info_type *image_info, int section_index)
{
	char    buffer[64], *val, *name, uname[64];
	int     i, ap, l, b, idx=0, unknown=0;
#ifdef EXIF_DEBUG
	int     info_tag;
#endif
	image_info_value *info_value;
	image_info_data  *info_data;
	zval 			 tmpi, array;

#ifdef EXIF_DEBUG
/*		php_error_docref(NULL, E_NOTICE, "Adding %d infos from section %s", image_info->info_list[section_index].count, exif_get_sectionname(section_index));*/
#endif
	if (image_info->info_list[section_index].count) {
		if (sub_array) {
			array_init(&tmpi);
		} else {
			ZVAL_COPY_VALUE(&tmpi, value);
		}

		for(i=0; i<image_info->info_list[section_index].count; i++) {
			info_data  = &image_info->info_list[section_index].list[i];
#ifdef EXIF_DEBUG
			info_tag   = info_data->tag; /* conversion */
#endif
			info_value = &info_data->value;
			if (!(name = info_data->name)) {
				snprintf(uname, sizeof(uname), "%d", unknown++);
				name = uname;
			}
#ifdef EXIF_DEBUG
/*		php_error_docref(NULL, E_NOTICE, "Adding infos: tag(0x%04X,%12s,L=0x%04X): %s", info_tag, exif_get_tagname(info_tag, buffer, -12, exif_get_tag_table(section_index)), info_data->length, info_data->format==TAG_FMT_STRING?(info_value&&info_value->s?info_value->s:"<no data>"):exif_get_tagformat(info_data->format));*/
#endif
			if (info_data->length==0) {
				add_assoc_null(&tmpi, name);
			} else {
				switch (info_data->format) {
					default:
						/* Standard says more types possible but skip them...
						 * but allow users to handle data if they know how to
						 * So not return but use type UNDEFINED
						 * return;
						 */
					case TAG_FMT_BYTE:
					case TAG_FMT_SBYTE:
					case TAG_FMT_UNDEFINED:
						if (!info_value->s) {
							add_assoc_stringl(&tmpi, name, "", 0);
						} else {
							add_assoc_stringl(&tmpi, name, info_value->s, info_data->length);
						}
						break;

					case TAG_FMT_STRING:
						if (!(val = info_value->s)) {
							val = "";
						}
						if (section_index==SECTION_COMMENT) {
							add_index_string(&tmpi, idx++, val);
						} else {
							add_assoc_string(&tmpi, name, val);
						}
						break;

					case TAG_FMT_URATIONAL:
					case TAG_FMT_SRATIONAL:
					/*case TAG_FMT_BYTE:
					case TAG_FMT_SBYTE:*/
					case TAG_FMT_USHORT:
					case TAG_FMT_SSHORT:
					case TAG_FMT_SINGLE:
					case TAG_FMT_DOUBLE:
					case TAG_FMT_ULONG:
					case TAG_FMT_SLONG:
						/* now the rest, first see if it becomes an array */
						if ((l = info_data->length) > 1) {
							array_init(&array);
						}
						for(ap=0; ap<l; ap++) {
							if (l>1) {
								info_value = &info_data->value.list[ap];
							}
							switch (info_data->format) {
								case TAG_FMT_BYTE:
									if (l>1) {
										info_value = &info_data->value;
										for (b=0;b<l;b++) {
											add_index_long(&array, b, (int)(info_value->s[b]));
										}
										break;
									}
								case TAG_FMT_USHORT:
								case TAG_FMT_ULONG:
									if (l==1) {
										add_assoc_long(&tmpi, name, (int)info_value->u);
									} else {
										add_index_long(&array, ap, (int)info_value->u);
									}
									break;

								case TAG_FMT_URATIONAL:
									snprintf(buffer, sizeof(buffer), "%u/%u", info_value->ur.num, info_value->ur.den);
									if (l==1) {
										add_assoc_string(&tmpi, name, buffer);
									} else {
										add_index_string(&array, ap, buffer);
									}
									break;

								case TAG_FMT_SBYTE:
									if (l>1) {
										info_value = &info_data->value;
										for (b=0;b<l;b++) {
											add_index_long(&array, ap, (int)info_value->s[b]);
										}
										break;
									}
								case TAG_FMT_SSHORT:
								case TAG_FMT_SLONG:
									if (l==1) {
										add_assoc_long(&tmpi, name, info_value->i);
									} else {
										add_index_long(&array, ap, info_value->i);
									}
									break;

								case TAG_FMT_SRATIONAL:
									snprintf(buffer, sizeof(buffer), "%i/%i", info_value->sr.num, info_value->sr.den);
									if (l==1) {
										add_assoc_string(&tmpi, name, buffer);
									} else {
										add_index_string(&array, ap, buffer);
									}
									break;

								case TAG_FMT_SINGLE:
									if (l==1) {
										add_assoc_double(&tmpi, name, info_value->f);
									} else {
										add_index_double(&array, ap, info_value->f);
									}
									break;

								case TAG_FMT_DOUBLE:
									if (l==1) {
										add_assoc_double(&tmpi, name, info_value->d);
									} else {
										add_index_double(&array, ap, info_value->d);
									}
									break;
							}
							info_value = &info_data->value.list[ap];
						}
						if (l>1) {
							add_assoc_zval(&tmpi, name, &array);
						}
						break;
				}
			}
		}
		if (sub_array) {
			add_assoc_zval(value, exif_get_sectionname(section_index), &tmpi);
		}
	}
}