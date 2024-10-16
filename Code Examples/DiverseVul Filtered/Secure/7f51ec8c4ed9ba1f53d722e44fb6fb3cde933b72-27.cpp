struct ldb_dn *ldb_dn_new(TALLOC_CTX *mem_ctx,
			  struct ldb_context *ldb,
			  const char *strdn)
{
	struct ldb_val blob;
	blob.data = discard_const_p(uint8_t, strdn);
	blob.length = strdn ? strlen(strdn) : 0;
	return ldb_dn_from_ldb_val(mem_ctx, ldb, &blob);
}