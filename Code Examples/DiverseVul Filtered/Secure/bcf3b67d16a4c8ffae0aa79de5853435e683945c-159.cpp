static __poll_t megasas_mgmt_poll(struct file *file, poll_table *wait)
{
	__poll_t mask;
	unsigned long flags;

	poll_wait(file, &megasas_poll_wait, wait);
	spin_lock_irqsave(&poll_aen_lock, flags);
	if (megasas_poll_wait_aen)
		mask = (EPOLLIN | EPOLLRDNORM);
	else
		mask = 0;
	megasas_poll_wait_aen = 0;
	spin_unlock_irqrestore(&poll_aen_lock, flags);
	return mask;
}