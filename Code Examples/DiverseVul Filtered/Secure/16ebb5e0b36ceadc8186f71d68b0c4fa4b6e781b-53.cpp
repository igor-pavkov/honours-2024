struct qdisc_rate_table *qdisc_get_rtab(struct tc_ratespec *r, struct nlattr *tab)
{
	struct qdisc_rate_table *rtab;

	for (rtab = qdisc_rtab_list; rtab; rtab = rtab->next) {
		if (memcmp(&rtab->rate, r, sizeof(struct tc_ratespec)) == 0) {
			rtab->refcnt++;
			return rtab;
		}
	}

	if (tab == NULL || r->rate == 0 || r->cell_log == 0 ||
	    nla_len(tab) != TC_RTAB_SIZE)
		return NULL;

	rtab = kmalloc(sizeof(*rtab), GFP_KERNEL);
	if (rtab) {
		rtab->rate = *r;
		rtab->refcnt = 1;
		memcpy(rtab->data, nla_data(tab), 1024);
		rtab->next = qdisc_rtab_list;
		qdisc_rtab_list = rtab;
	}
	return rtab;
}