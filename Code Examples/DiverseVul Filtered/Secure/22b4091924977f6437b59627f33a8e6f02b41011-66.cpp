unsigned char *create_volume_objectid(connection_struct *conn, unsigned char objid[16])
{
	const struct loadparm_substitution *lp_sub =
		loadparm_s3_global_substitution();

	E_md4hash(lp_servicename(talloc_tos(), lp_sub, SNUM(conn)),objid);
	return objid;
}