int vt_do_kdskled(int console, int cmd, unsigned long arg, int perm)
{
	struct kbd_struct *kb = kbd_table + console;
        unsigned long flags;
	unsigned char ucval;

        switch(cmd) {
	/* the ioctls below read/set the flags usually shown in the leds */
	/* don't use them - they will go away without warning */
	case KDGKBLED:
                spin_lock_irqsave(&kbd_event_lock, flags);
		ucval = kb->ledflagstate | (kb->default_ledflagstate << 4);
                spin_unlock_irqrestore(&kbd_event_lock, flags);
		return put_user(ucval, (char __user *)arg);

	case KDSKBLED:
		if (!perm)
			return -EPERM;
		if (arg & ~0x77)
			return -EINVAL;
                spin_lock_irqsave(&led_lock, flags);
		kb->ledflagstate = (arg & 7);
		kb->default_ledflagstate = ((arg >> 4) & 7);
		set_leds();
                spin_unlock_irqrestore(&led_lock, flags);
		return 0;

	/* the ioctls below only set the lights, not the functions */
	/* for those, see KDGKBLED and KDSKBLED above */
	case KDGETLED:
		ucval = getledstate();
		return put_user(ucval, (char __user *)arg);

	case KDSETLED:
		if (!perm)
			return -EPERM;
		setledstate(kb, arg);
		return 0;
        }
        return -ENOIOCTLCMD;
}