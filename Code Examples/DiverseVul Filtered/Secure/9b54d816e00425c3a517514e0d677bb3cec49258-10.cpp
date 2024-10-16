	__releases(ctx->disk->queue->queue_lock) __releases(rcu)
{
	struct module *owner;

	spin_unlock_irq(ctx->disk->queue->queue_lock);
	rcu_read_unlock();
	owner = ctx->disk->fops->owner;
	put_disk(ctx->disk);
	module_put(owner);
}