static void blkg_free(struct blkcg_gq *blkg)
{
	int i;

	if (!blkg)
		return;

	for (i = 0; i < BLKCG_MAX_POLS; i++)
		if (blkg->pd[i])
			blkcg_policy[i]->pd_free_fn(blkg->pd[i]);

	if (blkg->blkcg != &blkcg_root)
		blk_exit_rl(&blkg->rl);

	blkg_rwstat_exit(&blkg->stat_ios);
	blkg_rwstat_exit(&blkg->stat_bytes);
	kfree(blkg);
}