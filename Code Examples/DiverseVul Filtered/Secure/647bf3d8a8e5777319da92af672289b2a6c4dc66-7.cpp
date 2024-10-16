void rxe_mem_cleanup(void *arg)
{
	struct rxe_mem *mem = arg;
	int i;

	if (mem->umem)
		ib_umem_release(mem->umem);

	if (mem->map) {
		for (i = 0; i < mem->num_map; i++)
			kfree(mem->map[i]);

		kfree(mem->map);
	}
}