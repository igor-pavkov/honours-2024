static void NAttrClearFlag(ntfs_attr *na, FILE_ATTR_FLAGS flag)
{
	if (na->type == AT_DATA && na->name == AT_UNNAMED)
		na->ni->flags &= ~flag;
}