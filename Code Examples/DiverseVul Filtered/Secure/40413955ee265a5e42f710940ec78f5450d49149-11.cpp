int cipso_v4_doi_walk(u32 *skip_cnt,
		     int (*callback) (struct cipso_v4_doi *doi_def, void *arg),
		     void *cb_arg)
{
	int ret_val = -ENOENT;
	u32 doi_cnt = 0;
	struct cipso_v4_doi *iter_doi;

	rcu_read_lock();
	list_for_each_entry_rcu(iter_doi, &cipso_v4_doi_list, list)
		if (refcount_read(&iter_doi->refcount) > 0) {
			if (doi_cnt++ < *skip_cnt)
				continue;
			ret_val = callback(iter_doi, cb_arg);
			if (ret_val < 0) {
				doi_cnt--;
				goto doi_walk_return;
			}
		}

doi_walk_return:
	rcu_read_unlock();
	*skip_cnt = doi_cnt;
	return ret_val;
}