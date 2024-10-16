static struct cipso_v4_doi *cipso_v4_doi_search(u32 doi)
{
	struct cipso_v4_doi *iter;

	list_for_each_entry_rcu(iter, &cipso_v4_doi_list, list)
		if (iter->doi == doi && refcount_read(&iter->refcount))
			return iter;
	return NULL;
}