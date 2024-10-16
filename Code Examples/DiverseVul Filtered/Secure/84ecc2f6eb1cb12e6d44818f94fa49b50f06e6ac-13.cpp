u16 inverse_translate(struct vc_data *conp, int glyph, int use_unicode)
{
	struct uni_pagedir *p;
	int m;
	if (glyph < 0 || glyph >= MAX_GLYPH)
		return 0;
	else {
		p = *conp->vc_uni_pagedir_loc;
		if (!p)
			return glyph;
		else if (use_unicode) {
			if (!p->inverse_trans_unicode)
				return glyph;
			else
				return p->inverse_trans_unicode[glyph];
			} else {
			m = inv_translate[conp->vc_num];
			if (!p->inverse_translations[m])
				return glyph;
			else
				return p->inverse_translations[m][glyph];
			}
	}
}