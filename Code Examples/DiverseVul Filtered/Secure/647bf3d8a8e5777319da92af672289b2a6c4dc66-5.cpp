struct rxe_mem *lookup_mem(struct rxe_pd *pd, int access, u32 key,
			   enum lookup_type type)
{
	struct rxe_mem *mem;
	struct rxe_dev *rxe = to_rdev(pd->ibpd.device);
	int index = key >> 8;

	if (index >= RXE_MIN_MR_INDEX && index <= RXE_MAX_MR_INDEX) {
		mem = rxe_pool_get_index(&rxe->mr_pool, index);
		if (!mem)
			goto err1;
	} else {
		goto err1;
	}

	if ((type == lookup_local && mem->lkey != key) ||
	    (type == lookup_remote && mem->rkey != key))
		goto err2;

	if (mem->pd != pd)
		goto err2;

	if (access && !(access & mem->access))
		goto err2;

	if (mem->state != RXE_MEM_STATE_VALID)
		goto err2;

	return mem;

err2:
	rxe_drop_ref(mem);
err1:
	return NULL;
}