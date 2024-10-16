static void NAttrSetFlag(ntfs_attr *na, FILE_ATTR_FLAGS flag)
{
	if (na->type == AT_DATA && na->name == AT_UNNAMED)
		na->ni->flags |= flag;
	else
		ntfs_log_trace("Denied setting flag %d for not unnamed data "
			       "attribute\n", le32_to_cpu(flag));
}