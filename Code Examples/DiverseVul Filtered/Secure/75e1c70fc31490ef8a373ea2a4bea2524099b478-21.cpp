SYSCALL_DEFINE3(io_cancel, aio_context_t, ctx_id, struct iocb __user *, iocb,
		struct io_event __user *, result)
{
	int (*cancel)(struct kiocb *iocb, struct io_event *res);
	struct kioctx *ctx;
	struct kiocb *kiocb;
	u32 key;
	int ret;

	ret = get_user(key, &iocb->aio_key);
	if (unlikely(ret))
		return -EFAULT;

	ctx = lookup_ioctx(ctx_id);
	if (unlikely(!ctx))
		return -EINVAL;

	spin_lock_irq(&ctx->ctx_lock);
	ret = -EAGAIN;
	kiocb = lookup_kiocb(ctx, iocb, key);
	if (kiocb && kiocb->ki_cancel) {
		cancel = kiocb->ki_cancel;
		kiocb->ki_users ++;
		kiocbSetCancelled(kiocb);
	} else
		cancel = NULL;
	spin_unlock_irq(&ctx->ctx_lock);

	if (NULL != cancel) {
		struct io_event tmp;
		pr_debug("calling cancel\n");
		memset(&tmp, 0, sizeof(tmp));
		tmp.obj = (u64)(unsigned long)kiocb->ki_obj.user;
		tmp.data = kiocb->ki_user_data;
		ret = cancel(kiocb, &tmp);
		if (!ret) {
			/* Cancellation succeeded -- copy the result
			 * into the user's buffer.
			 */
			if (copy_to_user(result, &tmp, sizeof(tmp)))
				ret = -EFAULT;
		}
	} else
		ret = -EINVAL;

	put_ioctx(ctx);

	return ret;
}