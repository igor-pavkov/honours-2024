static void flush_current_bio_list(struct blk_plug_cb *cb, bool from_schedule)
{
	struct dm_offload *o = container_of(cb, struct dm_offload, cb);
	struct bio_list list;
	struct bio *bio;
	int i;

	INIT_LIST_HEAD(&o->cb.list);

	if (unlikely(!current->bio_list))
		return;

	for (i = 0; i < 2; i++) {
		list = current->bio_list[i];
		bio_list_init(&current->bio_list[i]);

		while ((bio = bio_list_pop(&list))) {
			struct bio_set *bs = bio->bi_pool;
			if (unlikely(!bs) || bs == fs_bio_set ||
			    !bs->rescue_workqueue) {
				bio_list_add(&current->bio_list[i], bio);
				continue;
			}

			spin_lock(&bs->rescue_lock);
			bio_list_add(&bs->rescue_list, bio);
			queue_work(bs->rescue_workqueue, &bs->rescue_work);
			spin_unlock(&bs->rescue_lock);
		}
	}
}