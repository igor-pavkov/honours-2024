static void ep_user_copy_worker(struct work_struct *work)
{
	struct kiocb_priv *priv = container_of(work, struct kiocb_priv, work);
	struct mm_struct *mm = priv->mm;
	struct kiocb *iocb = priv->iocb;
	size_t ret;

	kthread_use_mm(mm);
	ret = copy_to_iter(priv->buf, priv->actual, &priv->to);
	kthread_unuse_mm(mm);
	if (!ret)
		ret = -EFAULT;

	/* completing the iocb can drop the ctx and mm, don't touch mm after */
	iocb->ki_complete(iocb, ret);

	kfree(priv->buf);
	kfree(priv->to_free);
	kfree(priv);
}