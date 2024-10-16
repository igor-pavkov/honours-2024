static int qdisc_change(struct Qdisc *sch, struct nlattr **tca)
{
	struct qdisc_size_table *stab = NULL;
	int err = 0;

	if (tca[TCA_OPTIONS]) {
		if (sch->ops->change == NULL)
			return -EINVAL;
		err = sch->ops->change(sch, tca[TCA_OPTIONS]);
		if (err)
			return err;
	}

	if (tca[TCA_STAB]) {
		stab = qdisc_get_stab(tca[TCA_STAB]);
		if (IS_ERR(stab))
			return PTR_ERR(stab);
	}

	qdisc_put_stab(sch->stab);
	sch->stab = stab;

	if (tca[TCA_RATE])
		/* NB: ignores errors from replace_estimator
		   because change can't be undone. */
		gen_replace_estimator(&sch->bstats, &sch->rate_est,
					    qdisc_root_sleeping_lock(sch),
					    tca[TCA_RATE]);

	return 0;
}