char *ldb_dn_alloc_linearized(TALLOC_CTX *mem_ctx, struct ldb_dn *dn)
{
	return talloc_strdup(mem_ctx, ldb_dn_get_linearized(dn));
}