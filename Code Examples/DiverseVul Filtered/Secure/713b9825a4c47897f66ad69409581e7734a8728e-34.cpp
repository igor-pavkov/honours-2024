static inline unsigned int io_get_work_hash(struct io_wq_work *work)
{
	return work->flags >> IO_WQ_HASH_SHIFT;
}