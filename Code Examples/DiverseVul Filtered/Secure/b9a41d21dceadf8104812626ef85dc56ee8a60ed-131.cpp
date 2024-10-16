static void start_io_acct(struct dm_io *io)
{
	struct mapped_device *md = io->md;
	struct bio *bio = io->bio;
	int cpu;
	int rw = bio_data_dir(bio);

	io->start_time = jiffies;

	cpu = part_stat_lock();
	part_round_stats(md->queue, cpu, &dm_disk(md)->part0);
	part_stat_unlock();
	atomic_set(&dm_disk(md)->part0.in_flight[rw],
		atomic_inc_return(&md->pending[rw]));

	if (unlikely(dm_stats_used(&md->stats)))
		dm_stats_account_io(&md->stats, bio_data_dir(bio),
				    bio->bi_iter.bi_sector, bio_sectors(bio),
				    false, 0, &io->stats_aux);
}