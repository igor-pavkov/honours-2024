static void update_user_maps(void)
{
	int i;
	struct uni_pagedir *p, *q = NULL;
	
	for (i = 0; i < MAX_NR_CONSOLES; i++) {
		if (!vc_cons_allocated(i))
			continue;
		p = *vc_cons[i].d->vc_uni_pagedir_loc;
		if (p && p != q) {
			set_inverse_transl(vc_cons[i].d, p, USER_MAP);
			set_inverse_trans_unicode(vc_cons[i].d, p);
			q = p;
		}
	}
}