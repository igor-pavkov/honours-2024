struct gendisk *alloc_disk_node(int minors, int node_id)
{
	struct gendisk *disk;

	disk = kzalloc_node(sizeof(struct gendisk), GFP_KERNEL, node_id);
	if (disk) {
		if (!init_part_stats(&disk->part0)) {
			kfree(disk);
			return NULL;
		}
		disk->node_id = node_id;
		if (disk_expand_part_tbl(disk, 0)) {
			free_part_stats(&disk->part0);
			kfree(disk);
			return NULL;
		}
		disk->part_tbl->part[0] = &disk->part0;

		/*
		 * set_capacity() and get_capacity() currently don't use
		 * seqcounter to read/update the part0->nr_sects. Still init
		 * the counter as we can read the sectors in IO submission
		 * patch using seqence counters.
		 *
		 * TODO: Ideally set_capacity() and get_capacity() should be
		 * converted to make use of bd_mutex and sequence counters.
		 */
		seqcount_init(&disk->part0.nr_sects_seq);
		if (hd_ref_init(&disk->part0)) {
			hd_free_part(&disk->part0);
			kfree(disk);
			return NULL;
		}

		disk->minors = minors;
		rand_initialize_disk(disk);
		disk_to_dev(disk)->class = &block_class;
		disk_to_dev(disk)->type = &disk_type;
		device_initialize(disk_to_dev(disk));
	}
	return disk;
}