static int rxe_mem_alloc(struct rxe_dev *rxe, struct rxe_mem *mem, int num_buf)
{
	int i;
	int num_map;
	struct rxe_map **map = mem->map;

	num_map = (num_buf + RXE_BUF_PER_MAP - 1) / RXE_BUF_PER_MAP;

	mem->map = kmalloc_array(num_map, sizeof(*map), GFP_KERNEL);
	if (!mem->map)
		goto err1;

	for (i = 0; i < num_map; i++) {
		mem->map[i] = kmalloc(sizeof(**map), GFP_KERNEL);
		if (!mem->map[i])
			goto err2;
	}

	WARN_ON(!is_power_of_2(RXE_BUF_PER_MAP));

	mem->map_shift	= ilog2(RXE_BUF_PER_MAP);
	mem->map_mask	= RXE_BUF_PER_MAP - 1;

	mem->num_buf = num_buf;
	mem->num_map = num_map;
	mem->max_buf = num_map * RXE_BUF_PER_MAP;

	return 0;

err2:
	for (i--; i >= 0; i--)
		kfree(mem->map[i]);

	kfree(mem->map);
err1:
	return -ENOMEM;
}