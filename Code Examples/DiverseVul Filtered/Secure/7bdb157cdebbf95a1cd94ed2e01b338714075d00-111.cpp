static struct pmu *perf_init_event(struct perf_event *event)
{
	int idx, type, ret;
	struct pmu *pmu;

	idx = srcu_read_lock(&pmus_srcu);

	/* Try parent's PMU first: */
	if (event->parent && event->parent->pmu) {
		pmu = event->parent->pmu;
		ret = perf_try_init_event(pmu, event);
		if (!ret)
			goto unlock;
	}

	/*
	 * PERF_TYPE_HARDWARE and PERF_TYPE_HW_CACHE
	 * are often aliases for PERF_TYPE_RAW.
	 */
	type = event->attr.type;
	if (type == PERF_TYPE_HARDWARE || type == PERF_TYPE_HW_CACHE)
		type = PERF_TYPE_RAW;

again:
	rcu_read_lock();
	pmu = idr_find(&pmu_idr, type);
	rcu_read_unlock();
	if (pmu) {
		ret = perf_try_init_event(pmu, event);
		if (ret == -ENOENT && event->attr.type != type) {
			type = event->attr.type;
			goto again;
		}

		if (ret)
			pmu = ERR_PTR(ret);

		goto unlock;
	}

	list_for_each_entry_rcu(pmu, &pmus, entry, lockdep_is_held(&pmus_srcu)) {
		ret = perf_try_init_event(pmu, event);
		if (!ret)
			goto unlock;

		if (ret != -ENOENT) {
			pmu = ERR_PTR(ret);
			goto unlock;
		}
	}
	pmu = ERR_PTR(-ENOENT);
unlock:
	srcu_read_unlock(&pmus_srcu, idx);

	return pmu;
}