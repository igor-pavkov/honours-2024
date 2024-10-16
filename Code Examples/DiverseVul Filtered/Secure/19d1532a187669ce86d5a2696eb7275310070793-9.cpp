static void sp_put(struct sixpack *sp)
{
	if (refcount_dec_and_test(&sp->refcnt))
		complete(&sp->dead);
}