static void nft_obj_del(struct nft_object *obj)
{
	rhltable_remove(&nft_objname_ht, &obj->rhlhead, nft_objname_ht_params);
	list_del_rcu(&obj->list);
}