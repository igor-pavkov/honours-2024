static void swap_ptr(void *l, void *r)
{
	void **lp = l, **rp = r;

	swap(*lp, *rp);
}