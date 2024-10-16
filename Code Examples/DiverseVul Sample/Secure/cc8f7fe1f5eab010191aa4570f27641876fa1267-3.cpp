static struct bio_map_data *bio_alloc_map_data(struct iov_iter *data,
					       gfp_t gfp_mask)
{
	struct bio_map_data *bmd;

	if (data->nr_segs > UIO_MAXIOV)
		return NULL;

	bmd = kmalloc(struct_size(bmd, iov, data->nr_segs), gfp_mask);
	if (!bmd)
		return NULL;
	memcpy(bmd->iov, data->iov, sizeof(struct iovec) * data->nr_segs);
	bmd->iter = *data;
	bmd->iter.iov = bmd->iov;
	return bmd;
}