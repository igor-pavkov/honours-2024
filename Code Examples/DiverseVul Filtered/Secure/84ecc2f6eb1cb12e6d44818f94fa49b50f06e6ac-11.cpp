static void set_inverse_trans_unicode(struct vc_data *conp,
				      struct uni_pagedir *p)
{
	int i, j, k, glyph;
	u16 **p1, *p2;
	u16 *q;

	if (!p) return;
	q = p->inverse_trans_unicode;
	if (!q) {
		q = p->inverse_trans_unicode =
			kmalloc_array(MAX_GLYPH, sizeof(u16), GFP_KERNEL);
		if (!q)
			return;
	}
	memset(q, 0, MAX_GLYPH * sizeof(u16));

	for (i = 0; i < 32; i++) {
		p1 = p->uni_pgdir[i];
		if (!p1)
			continue;
		for (j = 0; j < 32; j++) {
			p2 = p1[j];
			if (!p2)
				continue;
			for (k = 0; k < 64; k++) {
				glyph = p2[k];
				if (glyph >= 0 && glyph < MAX_GLYPH
					       && q[glyph] < 32)
		  			q[glyph] = (i << 11) + (j << 6) + k;
			}
		}
	}
}