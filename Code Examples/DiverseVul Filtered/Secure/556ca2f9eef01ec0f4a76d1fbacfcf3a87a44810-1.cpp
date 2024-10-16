static void rebase_info3_free(RzDyldRebaseInfo3 *rebase_info) {
	if (!rebase_info) {
		return;
	}
	free(rebase_info->page_starts);
	free(rebase_info);
}