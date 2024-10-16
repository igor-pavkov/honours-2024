static void set_inverse_transl(struct vc_data *conp, struct uni_pagedir *p, int i)
{
	int j, glyph;
	unsigned short *t = translations[i];
	unsigned char *q;
	
	if (!p) return;
	q = p->inverse_translations[i];

	if (!q) {
		q = p->inverse_translations[i] = kmalloc(MAX_GLYPH, GFP_KERNEL);
		if (!q) return;
	}
	memset(q, 0, MAX_GLYPH);

	for (j = 0; j < E_TABSZ; j++) {
		glyph = conv_uni_to_pc(conp, t[j]);
		if (glyph >= 0 && glyph < MAX_GLYPH && q[glyph] < 32) {
			/* prefer '-' above SHY etc. */
		  	q[glyph] = j;
		}
	}
}