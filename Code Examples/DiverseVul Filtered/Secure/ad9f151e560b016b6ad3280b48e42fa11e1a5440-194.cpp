static void nf_tables_flowtable_destroy(struct nft_flowtable *flowtable)
{
	struct nft_hook *hook, *next;

	flowtable->data.type->free(&flowtable->data);
	list_for_each_entry_safe(hook, next, &flowtable->hook_list, list) {
		flowtable->data.type->setup(&flowtable->data, hook->ops.dev,
					    FLOW_BLOCK_UNBIND);
		list_del_rcu(&hook->list);
		kfree(hook);
	}
	kfree(flowtable->name);
	module_put(flowtable->data.type->owner);
	kfree(flowtable);
}