static void blkg_destroy_all(struct request_queue *q)
{
	struct blkcg_gq *blkg, *n;

	lockdep_assert_held(q->queue_lock);

	list_for_each_entry_safe(blkg, n, &q->blkg_list, q_node) {
		struct blkcg *blkcg = blkg->blkcg;

		spin_lock(&blkcg->lock);
		blkg_destroy(blkg);
		spin_unlock(&blkcg->lock);
	}

	q->root_blkg = NULL;
	q->root_rl.blkg = NULL;
}