static void rebase_info_free(RzDyldRebaseInfo *rebase_info) {
	if (!rebase_info) {
		return;
	}

	RZ_FREE(rebase_info->one_page_buf);

	ut8 version = rebase_info->version;

	if (version == 1) {
		rebase_info1_free((RzDyldRebaseInfo1 *)rebase_info);
	} else if (version == 2 || version == 4) {
		rebase_info2_free((RzDyldRebaseInfo2 *)rebase_info);
	} else if (version == 3) {
		rebase_info3_free((RzDyldRebaseInfo3 *)rebase_info);
	} else {
		free(rebase_info);
	}
}