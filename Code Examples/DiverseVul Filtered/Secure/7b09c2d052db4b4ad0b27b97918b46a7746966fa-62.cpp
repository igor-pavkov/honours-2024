void fib6_metric_set(struct fib6_info *f6i, int metric, u32 val)
{
	if (!f6i)
		return;

	if (f6i->fib6_metrics == &dst_default_metrics) {
		struct dst_metrics *p = kzalloc(sizeof(*p), GFP_ATOMIC);

		if (!p)
			return;

		refcount_set(&p->refcnt, 1);
		f6i->fib6_metrics = p;
	}

	f6i->fib6_metrics->metrics[metric - 1] = val;
}