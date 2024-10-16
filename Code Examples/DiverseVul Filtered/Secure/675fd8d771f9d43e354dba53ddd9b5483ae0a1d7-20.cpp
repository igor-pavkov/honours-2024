static void shadow_copy2_sort_data(vfs_handle_struct *handle,
				   struct shadow_copy_data *shadow_copy2_data)
{
	int (*cmpfunc)(const void *, const void *);
	const char *sort;
	struct shadow_copy2_config *config;

	SMB_VFS_HANDLE_GET_DATA(handle, config, struct shadow_copy2_config,
				return);

	sort = config->sort_order;
	if (sort == NULL) {
		return;
	}

	if (strcmp(sort, "asc") == 0) {
		cmpfunc = shadow_copy2_label_cmp_asc;
	} else if (strcmp(sort, "desc") == 0) {
		cmpfunc = shadow_copy2_label_cmp_desc;
	} else {
		return;
	}

	if (shadow_copy2_data && shadow_copy2_data->num_volumes > 0 &&
	    shadow_copy2_data->labels)
	{
		TYPESAFE_QSORT(shadow_copy2_data->labels,
			       shadow_copy2_data->num_volumes,
			       cmpfunc);
	}
}