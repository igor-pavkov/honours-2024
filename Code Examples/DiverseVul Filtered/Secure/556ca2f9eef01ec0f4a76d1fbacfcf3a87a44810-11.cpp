static void rebase_info2_free(RzDyldRebaseInfo2 *rebase_info) {
	if (!rebase_info) {
		return;
	}
	free(rebase_info->page_starts);
	free(rebase_info->page_extras);
	free(rebase_info);
}