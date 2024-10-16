void *iova_to_vaddr(struct rxe_mem *mem, u64 iova, int length)
{
	size_t offset;
	int m, n;
	void *addr;

	if (mem->state != RXE_MEM_STATE_VALID) {
		pr_warn("mem not in valid state\n");
		addr = NULL;
		goto out;
	}

	if (!mem->map) {
		addr = (void *)(uintptr_t)iova;
		goto out;
	}

	if (mem_check_range(mem, iova, length)) {
		pr_warn("range violation\n");
		addr = NULL;
		goto out;
	}

	lookup_iova(mem, iova, &m, &n, &offset);

	if (offset + length > mem->map[m]->buf[n].size) {
		pr_warn("crosses page boundary\n");
		addr = NULL;
		goto out;
	}

	addr = (void *)(uintptr_t)mem->map[m]->buf[n].addr + offset;

out:
	return addr;
}