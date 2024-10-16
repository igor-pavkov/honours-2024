static int nft_register_flowtable_net_hooks(struct net *net,
					    struct nft_table *table,
					    struct list_head *hook_list,
					    struct nft_flowtable *flowtable)
{
	struct nft_hook *hook, *hook2, *next;
	struct nft_flowtable *ft;
	int err, i = 0;

	list_for_each_entry(hook, hook_list, list) {
		list_for_each_entry(ft, &table->flowtables, list) {
			if (!nft_is_active_next(net, ft))
				continue;

			list_for_each_entry(hook2, &ft->hook_list, list) {
				if (hook->ops.dev == hook2->ops.dev &&
				    hook->ops.pf == hook2->ops.pf) {
					err = -EEXIST;
					goto err_unregister_net_hooks;
				}
			}
		}

		err = flowtable->data.type->setup(&flowtable->data,
						  hook->ops.dev,
						  FLOW_BLOCK_BIND);
		if (err < 0)
			goto err_unregister_net_hooks;

		err = nf_register_net_hook(net, &hook->ops);
		if (err < 0) {
			flowtable->data.type->setup(&flowtable->data,
						    hook->ops.dev,
						    FLOW_BLOCK_UNBIND);
			goto err_unregister_net_hooks;
		}

		i++;
	}

	return 0;

err_unregister_net_hooks:
	list_for_each_entry_safe(hook, next, hook_list, list) {
		if (i-- <= 0)
			break;

		nft_unregister_flowtable_hook(net, flowtable, hook);
		list_del_rcu(&hook->list);
		kfree_rcu(hook, rcu);
	}

	return err;
}