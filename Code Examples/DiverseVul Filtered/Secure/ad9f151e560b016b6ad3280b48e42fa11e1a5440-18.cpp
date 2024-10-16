struct nft_object *nft_obj_lookup(const struct net *net,
				  const struct nft_table *table,
				  const struct nlattr *nla, u32 objtype,
				  u8 genmask)
{
	struct nft_object_hash_key k = { .table = table };
	char search[NFT_OBJ_MAXNAMELEN];
	struct rhlist_head *tmp, *list;
	struct nft_object *obj;

	nla_strscpy(search, nla, sizeof(search));
	k.name = search;

	WARN_ON_ONCE(!rcu_read_lock_held() &&
		     !lockdep_commit_lock_is_held(net));

	rcu_read_lock();
	list = rhltable_lookup(&nft_objname_ht, &k, nft_objname_ht_params);
	if (!list)
		goto out;

	rhl_for_each_entry_rcu(obj, tmp, list, rhlhead) {
		if (objtype == obj->ops->type->type &&
		    nft_active_genmask(obj, genmask)) {
			rcu_read_unlock();
			return obj;
		}
	}
out:
	rcu_read_unlock();
	return ERR_PTR(-ENOENT);
}