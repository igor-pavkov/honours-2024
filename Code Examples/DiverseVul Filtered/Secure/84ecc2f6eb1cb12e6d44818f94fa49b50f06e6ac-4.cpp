static int con_do_clear_unimap(struct vc_data *vc)
{
	struct uni_pagedir *p, *q;

	p = *vc->vc_uni_pagedir_loc;
	if (!p || --p->refcount) {
		q = kzalloc(sizeof(*p), GFP_KERNEL);
		if (!q) {
			if (p)
				p->refcount++;
			return -ENOMEM;
		}
		q->refcount=1;
		*vc->vc_uni_pagedir_loc = q;
	} else {
		if (p == dflt) dflt = NULL;
		p->refcount++;
		p->sum = 0;
		con_release_unimap(p);
	}
	return 0;
}