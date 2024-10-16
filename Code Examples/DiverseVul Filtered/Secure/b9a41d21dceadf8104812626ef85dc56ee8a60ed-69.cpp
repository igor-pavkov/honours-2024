struct dm_md_mempools *dm_alloc_md_mempools(struct mapped_device *md, enum dm_queue_mode type,
					    unsigned integrity, unsigned per_io_data_size)
{
	struct dm_md_mempools *pools = kzalloc_node(sizeof(*pools), GFP_KERNEL, md->numa_node_id);
	unsigned int pool_size = 0;
	unsigned int front_pad;

	if (!pools)
		return NULL;

	switch (type) {
	case DM_TYPE_BIO_BASED:
	case DM_TYPE_DAX_BIO_BASED:
		pool_size = dm_get_reserved_bio_based_ios();
		front_pad = roundup(per_io_data_size, __alignof__(struct dm_target_io)) + offsetof(struct dm_target_io, clone);
	
		pools->io_pool = mempool_create_slab_pool(pool_size, _io_cache);
		if (!pools->io_pool)
			goto out;
		break;
	case DM_TYPE_REQUEST_BASED:
	case DM_TYPE_MQ_REQUEST_BASED:
		pool_size = dm_get_reserved_rq_based_ios();
		front_pad = offsetof(struct dm_rq_clone_bio_info, clone);
		/* per_io_data_size is used for blk-mq pdu at queue allocation */
		break;
	default:
		BUG();
	}

	pools->bs = bioset_create(pool_size, front_pad, BIOSET_NEED_RESCUER);
	if (!pools->bs)
		goto out;

	if (integrity && bioset_integrity_create(pools->bs, pool_size))
		goto out;

	return pools;

out:
	dm_free_md_mempools(pools);

	return NULL;
}