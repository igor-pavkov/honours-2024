struct dns_srvrq *find_srvrq_by_name(const char *name, struct proxy *px)
{
	struct dns_srvrq *srvrq;

	list_for_each_entry(srvrq, &dns_srvrq_list, list) {
		if (srvrq->proxy == px && !strcmp(srvrq->name, name))
			return srvrq;
	}
	return NULL;
}