static inline int tnc_init(struct sixpack *sp)
{
	unsigned char inbyte = 0xe8;

	tnc_set_sync_state(sp, TNC_UNSYNC_STARTUP);

	sp->tty->ops->write(sp->tty, &inbyte, 1);

	mod_timer(&sp->resync_t, jiffies + SIXP_RESYNC_TIMEOUT);

	return 0;
}