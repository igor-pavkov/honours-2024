static void rds_inc_addref(struct rds_incoming *inc)
{
	rdsdebug("addref inc %p ref %d\n", inc, atomic_read(&inc->i_refcount));
	atomic_inc(&inc->i_refcount);
}