void con_free_unimap(struct vc_data *vc)
{
	struct uni_pagedir *p;

	p = *vc->vc_uni_pagedir_loc;
	if (!p)
		return;
	*vc->vc_uni_pagedir_loc = NULL;
	if (--p->refcount)
		return;
	con_release_unimap(p);
	kfree(p);
}