static void r_rebase_info_free(RRebaseInfo *info) {
	if (!info) {
		return;
	}

	if (info->ranges) {
		R_FREE (info->ranges);
		info->ranges = NULL;
	}

	R_FREE (info);
}