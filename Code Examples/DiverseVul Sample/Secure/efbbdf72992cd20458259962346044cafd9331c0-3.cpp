struct dns_resolvers *find_resolvers_by_id(const char *id)
{
	struct dns_resolvers *res;

	list_for_each_entry(res, &dns_resolvers, list) {
		if (!strcmp(res->id, id))
			return res;
	}
	return NULL;
}