int vt_do_diacrit(unsigned int cmd, void __user *udp, int perm)
{
	unsigned long flags;
	int asize;
	int ret = 0;

	switch (cmd) {
	case KDGKBDIACR:
	{
		struct kbdiacrs __user *a = udp;
		struct kbdiacr *dia;
		int i;

		dia = kmalloc_array(MAX_DIACR, sizeof(struct kbdiacr),
								GFP_KERNEL);
		if (!dia)
			return -ENOMEM;

		/* Lock the diacriticals table, make a copy and then
		   copy it after we unlock */
		spin_lock_irqsave(&kbd_event_lock, flags);

		asize = accent_table_size;
		for (i = 0; i < asize; i++) {
			dia[i].diacr = conv_uni_to_8bit(
						accent_table[i].diacr);
			dia[i].base = conv_uni_to_8bit(
						accent_table[i].base);
			dia[i].result = conv_uni_to_8bit(
						accent_table[i].result);
		}
		spin_unlock_irqrestore(&kbd_event_lock, flags);

		if (put_user(asize, &a->kb_cnt))
			ret = -EFAULT;
		else  if (copy_to_user(a->kbdiacr, dia,
				asize * sizeof(struct kbdiacr)))
			ret = -EFAULT;
		kfree(dia);
		return ret;
	}
	case KDGKBDIACRUC:
	{
		struct kbdiacrsuc __user *a = udp;
		void *buf;

		buf = kmalloc_array(MAX_DIACR, sizeof(struct kbdiacruc),
								GFP_KERNEL);
		if (buf == NULL)
			return -ENOMEM;

		/* Lock the diacriticals table, make a copy and then
		   copy it after we unlock */
		spin_lock_irqsave(&kbd_event_lock, flags);

		asize = accent_table_size;
		memcpy(buf, accent_table, asize * sizeof(struct kbdiacruc));

		spin_unlock_irqrestore(&kbd_event_lock, flags);

		if (put_user(asize, &a->kb_cnt))
			ret = -EFAULT;
		else if (copy_to_user(a->kbdiacruc, buf,
				asize*sizeof(struct kbdiacruc)))
			ret = -EFAULT;
		kfree(buf);
		return ret;
	}

	case KDSKBDIACR:
	{
		struct kbdiacrs __user *a = udp;
		struct kbdiacr *dia = NULL;
		unsigned int ct;
		int i;

		if (!perm)
			return -EPERM;
		if (get_user(ct, &a->kb_cnt))
			return -EFAULT;
		if (ct >= MAX_DIACR)
			return -EINVAL;

		if (ct) {

			dia = memdup_user(a->kbdiacr,
					sizeof(struct kbdiacr) * ct);
			if (IS_ERR(dia))
				return PTR_ERR(dia);

		}

		spin_lock_irqsave(&kbd_event_lock, flags);
		accent_table_size = ct;
		for (i = 0; i < ct; i++) {
			accent_table[i].diacr =
					conv_8bit_to_uni(dia[i].diacr);
			accent_table[i].base =
					conv_8bit_to_uni(dia[i].base);
			accent_table[i].result =
					conv_8bit_to_uni(dia[i].result);
		}
		spin_unlock_irqrestore(&kbd_event_lock, flags);
		kfree(dia);
		return 0;
	}

	case KDSKBDIACRUC:
	{
		struct kbdiacrsuc __user *a = udp;
		unsigned int ct;
		void *buf = NULL;

		if (!perm)
			return -EPERM;

		if (get_user(ct, &a->kb_cnt))
			return -EFAULT;

		if (ct >= MAX_DIACR)
			return -EINVAL;

		if (ct) {
			buf = memdup_user(a->kbdiacruc,
					  ct * sizeof(struct kbdiacruc));
			if (IS_ERR(buf))
				return PTR_ERR(buf);
		} 
		spin_lock_irqsave(&kbd_event_lock, flags);
		if (ct)
			memcpy(accent_table, buf,
					ct * sizeof(struct kbdiacruc));
		accent_table_size = ct;
		spin_unlock_irqrestore(&kbd_event_lock, flags);
		kfree(buf);
		return 0;
	}
	}
	return ret;
}