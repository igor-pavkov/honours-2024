static void exif_iif_add_buffer(image_info_type *image_info, int section_index, char *name, int length, char *value TSRMLS_DC)
{
	image_info_data  *info_data;
	image_info_data  *list;

	if (value) {
		list = safe_erealloc(image_info->info_list[section_index].list, (image_info->info_list[section_index].count+1), sizeof(image_info_data), 0);
		image_info->info_list[section_index].list = list;
		info_data  = &image_info->info_list[section_index].list[image_info->info_list[section_index].count];
		info_data->tag    = TAG_NONE;
		info_data->format = TAG_FMT_UNDEFINED;
		info_data->length = length;
		info_data->name   = estrdup(name);
		info_data->value.s = safe_emalloc(length, 1, 1);
		memcpy(info_data->value.s, value, length);
		info_data->value.s[length] = 0;
		image_info->sections_found |= 1<<section_index;
		image_info->info_list[section_index].count++;
	}
}