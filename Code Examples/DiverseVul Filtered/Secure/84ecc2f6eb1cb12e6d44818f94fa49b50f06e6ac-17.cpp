int con_get_unimap(struct vc_data *vc, ushort ct, ushort __user *uct, struct unipair __user *list)
{
	int i, j, k, ret = 0;
	ushort ect;
	u16 **p1, *p2;
	struct uni_pagedir *p;
	struct unipair *unilist;

	unilist = kvmalloc_array(ct, sizeof(struct unipair), GFP_KERNEL);
	if (!unilist)
		return -ENOMEM;

	console_lock();

	ect = 0;
	if (*vc->vc_uni_pagedir_loc) {
		p = *vc->vc_uni_pagedir_loc;
		for (i = 0; i < 32; i++) {
		p1 = p->uni_pgdir[i];
		if (p1)
			for (j = 0; j < 32; j++) {
			p2 = *(p1++);
			if (p2)
				for (k = 0; k < 64; k++, p2++) {
					if (*p2 >= MAX_GLYPH)
						continue;
					if (ect < ct) {
						unilist[ect].unicode =
							(i<<11)+(j<<6)+k;
						unilist[ect].fontpos = *p2;
					}
					ect++;
				}
			}
		}
	}
	console_unlock();
	if (copy_to_user(list, unilist, min(ect, ct) * sizeof(struct unipair)))
		ret = -EFAULT;
	put_user(ect, uct);
	kvfree(unilist);
	return ret ? ret : (ect <= ct) ? 0 : -ENOMEM;
}