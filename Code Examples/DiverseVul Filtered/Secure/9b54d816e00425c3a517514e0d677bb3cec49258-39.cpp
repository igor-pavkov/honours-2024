static int blkcg_print_stat(struct seq_file *sf, void *v)
{
	struct blkcg *blkcg = css_to_blkcg(seq_css(sf));
	struct blkcg_gq *blkg;

	rcu_read_lock();

	hlist_for_each_entry_rcu(blkg, &blkcg->blkg_list, blkcg_node) {
		const char *dname;
		struct blkg_rwstat rwstat;
		u64 rbytes, wbytes, rios, wios;

		dname = blkg_dev_name(blkg);
		if (!dname)
			continue;

		spin_lock_irq(blkg->q->queue_lock);

		rwstat = blkg_rwstat_recursive_sum(blkg, NULL,
					offsetof(struct blkcg_gq, stat_bytes));
		rbytes = atomic64_read(&rwstat.aux_cnt[BLKG_RWSTAT_READ]);
		wbytes = atomic64_read(&rwstat.aux_cnt[BLKG_RWSTAT_WRITE]);

		rwstat = blkg_rwstat_recursive_sum(blkg, NULL,
					offsetof(struct blkcg_gq, stat_ios));
		rios = atomic64_read(&rwstat.aux_cnt[BLKG_RWSTAT_READ]);
		wios = atomic64_read(&rwstat.aux_cnt[BLKG_RWSTAT_WRITE]);

		spin_unlock_irq(blkg->q->queue_lock);

		if (rbytes || wbytes || rios || wios)
			seq_printf(sf, "%s rbytes=%llu wbytes=%llu rios=%llu wios=%llu\n",
				   dname, rbytes, wbytes, rios, wios);
	}

	rcu_read_unlock();
	return 0;
}