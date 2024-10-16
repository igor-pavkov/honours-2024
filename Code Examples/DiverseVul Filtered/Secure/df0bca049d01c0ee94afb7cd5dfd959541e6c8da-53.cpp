void proto_unregister(struct proto *prot)
{
	write_lock(&proto_list_lock);
	release_proto_idx(prot);
	list_del(&prot->node);
	write_unlock(&proto_list_lock);

	if (prot->slab != NULL) {
		kmem_cache_destroy(prot->slab);
		prot->slab = NULL;
	}

	if (prot->rsk_prot != NULL && prot->rsk_prot->slab != NULL) {
		kmem_cache_destroy(prot->rsk_prot->slab);
		kfree(prot->rsk_prot->slab_name);
		prot->rsk_prot->slab = NULL;
	}

	if (prot->twsk_prot != NULL && prot->twsk_prot->twsk_slab != NULL) {
		kmem_cache_destroy(prot->twsk_prot->twsk_slab);
		kfree(prot->twsk_prot->twsk_slab_name);
		prot->twsk_prot->twsk_slab = NULL;
	}
}