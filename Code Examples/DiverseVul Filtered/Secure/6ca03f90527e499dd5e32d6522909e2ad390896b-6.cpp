static void k_brl(struct vc_data *vc, unsigned char value, char up_flag)
{
	static unsigned pressed, committing;
	static unsigned long releasestart;

	if (kbd->kbdmode != VC_UNICODE) {
		if (!up_flag)
			pr_warn("keyboard mode must be unicode for braille patterns\n");
		return;
	}

	if (!value) {
		k_unicode(vc, BRL_UC_ROW, up_flag);
		return;
	}

	if (value > 8)
		return;

	if (!up_flag) {
		pressed |= 1 << (value - 1);
		if (!brl_timeout)
			committing = pressed;
	} else if (brl_timeout) {
		if (!committing ||
		    time_after(jiffies,
			       releasestart + msecs_to_jiffies(brl_timeout))) {
			committing = pressed;
			releasestart = jiffies;
		}
		pressed &= ~(1 << (value - 1));
		if (!pressed && committing) {
			k_brlcommit(vc, committing, 0);
			committing = 0;
		}
	} else {
		if (committing) {
			k_brlcommit(vc, committing, 0);
			committing = 0;
		}
		pressed &= ~(1 << (value - 1));
	}
}