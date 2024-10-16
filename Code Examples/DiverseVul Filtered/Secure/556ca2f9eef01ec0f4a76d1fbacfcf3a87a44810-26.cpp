static void rebase_info1_free(RzDyldRebaseInfo1 *rebase_info) {
	if (!rebase_info) {
		return;
	}
	free(rebase_info->toc);
	free(rebase_info->entries);
	free(rebase_info);
}