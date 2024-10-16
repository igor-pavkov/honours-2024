static struct qdisc_size_table *qdisc_get_stab(struct nlattr *opt)
{
	struct nlattr *tb[TCA_STAB_MAX + 1];
	struct qdisc_size_table *stab;
	struct tc_sizespec *s;
	unsigned int tsize = 0;
	u16 *tab = NULL;
	int err;

	err = nla_parse_nested(tb, TCA_STAB_MAX, opt, stab_policy);
	if (err < 0)
		return ERR_PTR(err);
	if (!tb[TCA_STAB_BASE])
		return ERR_PTR(-EINVAL);

	s = nla_data(tb[TCA_STAB_BASE]);

	if (s->tsize > 0) {
		if (!tb[TCA_STAB_DATA])
			return ERR_PTR(-EINVAL);
		tab = nla_data(tb[TCA_STAB_DATA]);
		tsize = nla_len(tb[TCA_STAB_DATA]) / sizeof(u16);
	}

	if (!s || tsize != s->tsize || (!tab && tsize > 0))
		return ERR_PTR(-EINVAL);

	spin_lock(&qdisc_stab_lock);

	list_for_each_entry(stab, &qdisc_stab_list, list) {
		if (memcmp(&stab->szopts, s, sizeof(*s)))
			continue;
		if (tsize > 0 && memcmp(stab->data, tab, tsize * sizeof(u16)))
			continue;
		stab->refcnt++;
		spin_unlock(&qdisc_stab_lock);
		return stab;
	}

	spin_unlock(&qdisc_stab_lock);

	stab = kmalloc(sizeof(*stab) + tsize * sizeof(u16), GFP_KERNEL);
	if (!stab)
		return ERR_PTR(-ENOMEM);

	stab->refcnt = 1;
	stab->szopts = *s;
	if (tsize > 0)
		memcpy(stab->data, tab, tsize * sizeof(u16));

	spin_lock(&qdisc_stab_lock);
	list_add_tail(&stab->list, &qdisc_stab_list);
	spin_unlock(&qdisc_stab_lock);

	return stab;
}