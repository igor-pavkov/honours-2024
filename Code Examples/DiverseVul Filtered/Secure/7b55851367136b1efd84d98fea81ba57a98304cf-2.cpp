static void mmdrop_async_fn(struct work_struct *work)
{
	struct mm_struct *mm;

	mm = container_of(work, struct mm_struct, async_put_work);
	__mmdrop(mm);
}