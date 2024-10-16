static struct kioctx *lookup_ioctx(unsigned long ctx_id)
{
	struct mm_struct *mm = current->mm;
	struct kioctx *ctx, *ret = NULL;
	struct hlist_node *n;

	rcu_read_lock();

	hlist_for_each_entry_rcu(ctx, n, &mm->ioctx_list, list) {
		if (ctx->user_id == ctx_id && !ctx->dead) {
			get_ioctx(ctx);
			ret = ctx;
			break;
		}
	}

	rcu_read_unlock();
	return ret;
}