void qdisc_put_stab(struct qdisc_size_table *tab)
{
	if (!tab)
		return;

	spin_lock(&qdisc_stab_lock);

	if (--tab->refcnt == 0) {
		list_del(&tab->list);
		kfree(tab);
	}

	spin_unlock(&qdisc_stab_lock);
}