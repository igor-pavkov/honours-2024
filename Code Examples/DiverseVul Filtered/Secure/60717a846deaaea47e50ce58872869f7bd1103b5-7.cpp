static int NAttrFlag(ntfs_attr *na, FILE_ATTR_FLAGS flag)
{
	if (na->type == AT_DATA && na->name == AT_UNNAMED)
		return (na->ni->flags & flag);
	return 0;
}