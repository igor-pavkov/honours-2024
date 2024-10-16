void vt_reset_unicode(int console)
{
	unsigned long flags;

	spin_lock_irqsave(&kbd_event_lock, flags);
	kbd_table[console].kbdmode = default_utf8 ? VC_UNICODE : VC_XLATE;
	spin_unlock_irqrestore(&kbd_event_lock, flags);
}