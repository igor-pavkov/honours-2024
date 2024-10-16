int vt_do_kdgkb_ioctl(int cmd, struct kbsentry __user *user_kdgkb, int perm)
{
	struct kbsentry *kbs;
	char *p;
	u_char *q;
	u_char __user *up;
	int sz, fnw_sz;
	int delta;
	char *first_free, *fj, *fnw;
	int i, j, k;
	int ret;
	unsigned long flags;

	if (!capable(CAP_SYS_TTY_CONFIG))
		perm = 0;

	kbs = kmalloc(sizeof(*kbs), GFP_KERNEL);
	if (!kbs) {
		ret = -ENOMEM;
		goto reterr;
	}

	/* we mostly copy too much here (512bytes), but who cares ;) */
	if (copy_from_user(kbs, user_kdgkb, sizeof(struct kbsentry))) {
		ret = -EFAULT;
		goto reterr;
	}
	kbs->kb_string[sizeof(kbs->kb_string)-1] = '\0';
	i = array_index_nospec(kbs->kb_func, MAX_NR_FUNC);

	switch (cmd) {
	case KDGKBSENT:
		sz = sizeof(kbs->kb_string) - 1; /* sz should have been
						  a struct member */
		up = user_kdgkb->kb_string;
		p = func_table[i];
		if(p)
			for ( ; *p && sz; p++, sz--)
				if (put_user(*p, up++)) {
					ret = -EFAULT;
					goto reterr;
				}
		if (put_user('\0', up)) {
			ret = -EFAULT;
			goto reterr;
		}
		kfree(kbs);
		return ((p && *p) ? -EOVERFLOW : 0);
	case KDSKBSENT:
		if (!perm) {
			ret = -EPERM;
			goto reterr;
		}

		fnw = NULL;
		fnw_sz = 0;
		/* race aginst other writers */
		again:
		spin_lock_irqsave(&func_buf_lock, flags);
		q = func_table[i];

		/* fj pointer to next entry after 'q' */
		first_free = funcbufptr + (funcbufsize - funcbufleft);
		for (j = i+1; j < MAX_NR_FUNC && !func_table[j]; j++)
			;
		if (j < MAX_NR_FUNC)
			fj = func_table[j];
		else
			fj = first_free;
		/* buffer usage increase by new entry */
		delta = (q ? -strlen(q) : 1) + strlen(kbs->kb_string);

		if (delta <= funcbufleft) { 	/* it fits in current buf */
		    if (j < MAX_NR_FUNC) {
			/* make enough space for new entry at 'fj' */
			memmove(fj + delta, fj, first_free - fj);
			for (k = j; k < MAX_NR_FUNC; k++)
			    if (func_table[k])
				func_table[k] += delta;
		    }
		    if (!q)
		      func_table[i] = fj;
		    funcbufleft -= delta;
		} else {			/* allocate a larger buffer */
		    sz = 256;
		    while (sz < funcbufsize - funcbufleft + delta)
		      sz <<= 1;
		    if (fnw_sz != sz) {
		      spin_unlock_irqrestore(&func_buf_lock, flags);
		      kfree(fnw);
		      fnw = kmalloc(sz, GFP_KERNEL);
		      fnw_sz = sz;
		      if (!fnw) {
			ret = -ENOMEM;
			goto reterr;
		      }
		      goto again;
		    }

		    if (!q)
		      func_table[i] = fj;
		    /* copy data before insertion point to new location */
		    if (fj > funcbufptr)
			memmove(fnw, funcbufptr, fj - funcbufptr);
		    for (k = 0; k < j; k++)
		      if (func_table[k])
			func_table[k] = fnw + (func_table[k] - funcbufptr);

		    /* copy data after insertion point to new location */
		    if (first_free > fj) {
			memmove(fnw + (fj - funcbufptr) + delta, fj, first_free - fj);
			for (k = j; k < MAX_NR_FUNC; k++)
			  if (func_table[k])
			    func_table[k] = fnw + (func_table[k] - funcbufptr) + delta;
		    }
		    if (funcbufptr != func_buf)
		      kfree(funcbufptr);
		    funcbufptr = fnw;
		    funcbufleft = funcbufleft - delta + sz - funcbufsize;
		    funcbufsize = sz;
		}
		/* finally insert item itself */
		strcpy(func_table[i], kbs->kb_string);
		spin_unlock_irqrestore(&func_buf_lock, flags);
		break;
	}
	ret = 0;
reterr:
	kfree(kbs);
	return ret;
}