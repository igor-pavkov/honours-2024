static void jpc_ppxstabent_destroy(jpc_ppxstabent_t *ent)
{
	if (ent->data) {
		jas_free(ent->data);
	}
	jas_free(ent);
}