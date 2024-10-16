int con_copy_unimap(struct vc_data *dst_vc, struct vc_data *src_vc)
{
	struct uni_pagedir *q;

	if (!*src_vc->vc_uni_pagedir_loc)
		return -EINVAL;
	if (*dst_vc->vc_uni_pagedir_loc == *src_vc->vc_uni_pagedir_loc)
		return 0;
	con_free_unimap(dst_vc);
	q = *src_vc->vc_uni_pagedir_loc;
	q->refcount++;
	*dst_vc->vc_uni_pagedir_loc = q;
	return 0;
}