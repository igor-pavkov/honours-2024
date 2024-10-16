static void acm_tty_unthrottle(struct tty_struct *tty)
{
	struct acm *acm = tty->driver_data;
	unsigned int was_throttled;

	spin_lock_irq(&acm->read_lock);
	was_throttled = acm->throttled;
	acm->throttled = 0;
	acm->throttle_req = 0;
	spin_unlock_irq(&acm->read_lock);

	if (was_throttled)
		acm_submit_read_urbs(acm, GFP_KERNEL);
}