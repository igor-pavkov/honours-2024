static void con_release_unimap(struct uni_pagedir *p)
{
	u16 **p1;
	int i, j;

	if (p == dflt) dflt = NULL;  
	for (i = 0; i < 32; i++) {
		p1 = p->uni_pgdir[i];
		if (p1 != NULL) {
			for (j = 0; j < 32; j++)
				kfree(p1[j]);
			kfree(p1);
		}
		p->uni_pgdir[i] = NULL;
	}
	for (i = 0; i < 4; i++) {
		kfree(p->inverse_translations[i]);
		p->inverse_translations[i] = NULL;
	}
	kfree(p->inverse_trans_unicode);
	p->inverse_trans_unicode = NULL;
}