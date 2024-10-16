int rxe_mem_init_fast(struct rxe_dev *rxe, struct rxe_pd *pd,
		      int max_pages, struct rxe_mem *mem)
{
	int err;

	rxe_mem_init(0, mem);

	/* In fastreg, we also set the rkey */
	mem->ibmr.rkey = mem->ibmr.lkey;

	err = rxe_mem_alloc(rxe, mem, max_pages);
	if (err)
		goto err1;

	mem->pd			= pd;
	mem->max_buf		= max_pages;
	mem->state		= RXE_MEM_STATE_FREE;
	mem->type		= RXE_MEM_TYPE_MR;

	return 0;

err1:
	return err;
}