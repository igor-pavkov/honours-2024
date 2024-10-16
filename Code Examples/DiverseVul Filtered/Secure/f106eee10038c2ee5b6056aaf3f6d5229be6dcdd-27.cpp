static void mm_init_aio(struct mm_struct *mm)
{
#ifdef CONFIG_AIO
	spin_lock_init(&mm->ioctx_lock);
	INIT_HLIST_HEAD(&mm->ioctx_list);
#endif
}