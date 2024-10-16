static void jpc_ppxstab_destroy(jpc_ppxstab_t *tab)
{
	int i;
	for (i = 0; i < tab->numents; ++i) {
		jpc_ppxstabent_destroy(tab->ents[i]);
	}
	if (tab->ents) {
		jas_free(tab->ents);
	}
	jas_free(tab);
}