int vt_do_kdsk_ioctl(int cmd, struct kbentry __user *user_kbe, int perm,
						int console)
{
	struct kbd_struct *kb = kbd_table + console;
	struct kbentry tmp;
	ushort *key_map, *new_map, val, ov;
	unsigned long flags;

	if (copy_from_user(&tmp, user_kbe, sizeof(struct kbentry)))
		return -EFAULT;

	if (!capable(CAP_SYS_TTY_CONFIG))
		perm = 0;

	switch (cmd) {
	case KDGKBENT:
		/* Ensure another thread doesn't free it under us */
		spin_lock_irqsave(&kbd_event_lock, flags);
		key_map = key_maps[s];
		if (key_map) {
		    val = U(key_map[i]);
		    if (kb->kbdmode != VC_UNICODE && KTYP(val) >= NR_TYPES)
			val = K_HOLE;
		} else
		    val = (i ? K_HOLE : K_NOSUCHMAP);
		spin_unlock_irqrestore(&kbd_event_lock, flags);
		return put_user(val, &user_kbe->kb_value);
	case KDSKBENT:
		if (!perm)
			return -EPERM;
		if (!i && v == K_NOSUCHMAP) {
			spin_lock_irqsave(&kbd_event_lock, flags);
			/* deallocate map */
			key_map = key_maps[s];
			if (s && key_map) {
			    key_maps[s] = NULL;
			    if (key_map[0] == U(K_ALLOCATED)) {
					kfree(key_map);
					keymap_count--;
			    }
			}
			spin_unlock_irqrestore(&kbd_event_lock, flags);
			break;
		}

		if (KTYP(v) < NR_TYPES) {
		    if (KVAL(v) > max_vals[KTYP(v)])
				return -EINVAL;
		} else
		    if (kb->kbdmode != VC_UNICODE)
				return -EINVAL;

		/* ++Geert: non-PC keyboards may generate keycode zero */
#if !defined(__mc68000__) && !defined(__powerpc__)
		/* assignment to entry 0 only tests validity of args */
		if (!i)
			break;
#endif

		new_map = kmalloc(sizeof(plain_map), GFP_KERNEL);
		if (!new_map)
			return -ENOMEM;
		spin_lock_irqsave(&kbd_event_lock, flags);
		key_map = key_maps[s];
		if (key_map == NULL) {
			int j;

			if (keymap_count >= MAX_NR_OF_USER_KEYMAPS &&
			    !capable(CAP_SYS_RESOURCE)) {
				spin_unlock_irqrestore(&kbd_event_lock, flags);
				kfree(new_map);
				return -EPERM;
			}
			key_maps[s] = new_map;
			key_map = new_map;
			key_map[0] = U(K_ALLOCATED);
			for (j = 1; j < NR_KEYS; j++)
				key_map[j] = U(K_HOLE);
			keymap_count++;
		} else
			kfree(new_map);

		ov = U(key_map[i]);
		if (v == ov)
			goto out;
		/*
		 * Attention Key.
		 */
		if (((ov == K_SAK) || (v == K_SAK)) && !capable(CAP_SYS_ADMIN)) {
			spin_unlock_irqrestore(&kbd_event_lock, flags);
			return -EPERM;
		}
		key_map[i] = U(v);
		if (!s && (KTYP(ov) == KT_SHIFT || KTYP(v) == KT_SHIFT))
			do_compute_shiftstate();
out:
		spin_unlock_irqrestore(&kbd_event_lock, flags);
		break;
	}
	return 0;
}