static void mkiss_put(struct mkiss *ax)
{
	if (refcount_dec_and_test(&ax->refcnt))
		complete(&ax->dead);
}