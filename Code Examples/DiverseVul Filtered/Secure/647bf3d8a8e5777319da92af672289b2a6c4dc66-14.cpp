static void rxe_mem_init(int access, struct rxe_mem *mem)
{
	u32 lkey = mem->pelem.index << 8 | rxe_get_key();
	u32 rkey = (access & IB_ACCESS_REMOTE) ? lkey : 0;

	if (mem->pelem.pool->type == RXE_TYPE_MR) {
		mem->ibmr.lkey		= lkey;
		mem->ibmr.rkey		= rkey;
	}

	mem->lkey		= lkey;
	mem->rkey		= rkey;
	mem->state		= RXE_MEM_STATE_INVALID;
	mem->type		= RXE_MEM_TYPE_NONE;
	mem->map_shift		= ilog2(RXE_BUF_PER_MAP);
}