int con_set_default_unimap(struct vc_data *vc)
{
	int i, j, err = 0, err1;
	u16 *q;
	struct uni_pagedir *p;

	if (dflt) {
		p = *vc->vc_uni_pagedir_loc;
		if (p == dflt)
			return 0;

		dflt->refcount++;
		*vc->vc_uni_pagedir_loc = dflt;
		if (p && !--p->refcount) {
			con_release_unimap(p);
			kfree(p);
		}
		return 0;
	}
	
	/* The default font is always 256 characters */

	err = con_do_clear_unimap(vc);
	if (err)
		return err;
    
	p = *vc->vc_uni_pagedir_loc;
	q = dfont_unitable;
	
	for (i = 0; i < 256; i++)
		for (j = dfont_unicount[i]; j; j--) {
			err1 = con_insert_unipair(p, *(q++), i);
			if (err1)
				err = err1;
		}
			
	if (con_unify_unimap(vc, p)) {
		dflt = *vc->vc_uni_pagedir_loc;
		return err;
	}

	for (i = 0; i <= 3; i++)
		set_inverse_transl(vc, p, i);	/* Update all inverse translations */
	set_inverse_trans_unicode(vc, p);
	dflt = p;
	return err;
}