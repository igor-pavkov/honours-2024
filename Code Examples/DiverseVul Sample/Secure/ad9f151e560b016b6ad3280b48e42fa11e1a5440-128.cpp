void nf_tables_chain_destroy(struct nft_ctx *ctx)
{
	struct nft_chain *chain = ctx->chain;
	struct nft_hook *hook, *next;

	if (WARN_ON(chain->use > 0))
		return;

	/* no concurrent access possible anymore */
	nf_tables_chain_free_chain_rules(chain);

	if (nft_is_base_chain(chain)) {
		struct nft_base_chain *basechain = nft_base_chain(chain);

		if (nft_base_chain_netdev(ctx->family, basechain->ops.hooknum)) {
			list_for_each_entry_safe(hook, next,
						 &basechain->hook_list, list) {
				list_del_rcu(&hook->list);
				kfree_rcu(hook, rcu);
			}
		}
		module_put(basechain->type->owner);
		if (rcu_access_pointer(basechain->stats)) {
			static_branch_dec(&nft_counters_enabled);
			free_percpu(rcu_dereference_raw(basechain->stats));
		}
		kfree(chain->name);
		kfree(chain->udata);
		kfree(basechain);
	} else {
		kfree(chain->name);
		kfree(chain->udata);
		kfree(chain);
	}
}