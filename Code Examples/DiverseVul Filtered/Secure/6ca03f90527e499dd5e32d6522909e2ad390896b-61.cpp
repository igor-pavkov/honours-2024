void compute_shiftstate(void)
{
	unsigned long flags;
	spin_lock_irqsave(&kbd_event_lock, flags);
	do_compute_shiftstate();
	spin_unlock_irqrestore(&kbd_event_lock, flags);
}