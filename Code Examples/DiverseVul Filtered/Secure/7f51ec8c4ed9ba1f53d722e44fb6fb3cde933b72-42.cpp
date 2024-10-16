bool ldb_dn_validate(struct ldb_dn *dn)
{
	return ldb_dn_explode(dn);
}