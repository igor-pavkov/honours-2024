int proto_register(struct proto *prot, int alloc_slab)
{
	if (alloc_slab) {
		prot->slab = kmem_cache_create(prot->name, prot->obj_size, 0,
					SLAB_HWCACHE_ALIGN | prot->slab_flags,
					NULL);

		if (prot->slab == NULL) {
			printk(KERN_CRIT "%s: Can't create sock SLAB cache!\n",
			       prot->name);
			goto out;
		}

		if (prot->rsk_prot != NULL) {
			static const char mask[] = "request_sock_%s";

			prot->rsk_prot->slab_name = kmalloc(strlen(prot->name) + sizeof(mask) - 1, GFP_KERNEL);
			if (prot->rsk_prot->slab_name == NULL)
				goto out_free_sock_slab;

			sprintf(prot->rsk_prot->slab_name, mask, prot->name);
			prot->rsk_prot->slab = kmem_cache_create(prot->rsk_prot->slab_name,
								 prot->rsk_prot->obj_size, 0,
								 SLAB_HWCACHE_ALIGN, NULL);

			if (prot->rsk_prot->slab == NULL) {
				printk(KERN_CRIT "%s: Can't create request sock SLAB cache!\n",
				       prot->name);
				goto out_free_request_sock_slab_name;
			}
		}

		if (prot->twsk_prot != NULL) {
			static const char mask[] = "tw_sock_%s";

			prot->twsk_prot->twsk_slab_name = kmalloc(strlen(prot->name) + sizeof(mask) - 1, GFP_KERNEL);

			if (prot->twsk_prot->twsk_slab_name == NULL)
				goto out_free_request_sock_slab;

			sprintf(prot->twsk_prot->twsk_slab_name, mask, prot->name);
			prot->twsk_prot->twsk_slab =
				kmem_cache_create(prot->twsk_prot->twsk_slab_name,
						  prot->twsk_prot->twsk_obj_size,
						  0,
						  SLAB_HWCACHE_ALIGN |
							prot->slab_flags,
						  NULL);
			if (prot->twsk_prot->twsk_slab == NULL)
				goto out_free_timewait_sock_slab_name;
		}
	}

	write_lock(&proto_list_lock);
	list_add(&prot->node, &proto_list);
	assign_proto_idx(prot);
	write_unlock(&proto_list_lock);
	return 0;

out_free_timewait_sock_slab_name:
	kfree(prot->twsk_prot->twsk_slab_name);
out_free_request_sock_slab:
	if (prot->rsk_prot && prot->rsk_prot->slab) {
		kmem_cache_destroy(prot->rsk_prot->slab);
		prot->rsk_prot->slab = NULL;
	}
out_free_request_sock_slab_name:
	kfree(prot->rsk_prot->slab_name);
out_free_sock_slab:
	kmem_cache_destroy(prot->slab);
	prot->slab = NULL;
out:
	return -ENOBUFS;
}