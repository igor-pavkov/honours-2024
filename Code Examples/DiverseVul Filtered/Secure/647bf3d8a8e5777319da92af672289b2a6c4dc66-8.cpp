int rxe_mem_init_dma(struct rxe_dev *rxe, struct rxe_pd *pd,
		     int access, struct rxe_mem *mem)
{
	rxe_mem_init(access, mem);

	mem->pd			= pd;
	mem->access		= access;
	mem->state		= RXE_MEM_STATE_VALID;
	mem->type		= RXE_MEM_TYPE_DMA;

	return 0;
}