static void free_filters_list(struct list_head *filters)
{
	struct perf_addr_filter *filter, *iter;

	list_for_each_entry_safe(filter, iter, filters, entry) {
		path_put(&filter->path);
		list_del(&filter->entry);
		kfree(filter);
	}
}