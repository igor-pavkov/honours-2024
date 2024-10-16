static inline int check_deny(u32 mask, int isowner)
{
	if (mask & (NFS4_ACE_READ_ATTRIBUTES | NFS4_ACE_READ_ACL))
		return -EINVAL;
	if (!isowner)
		return 0;
	if (mask & (NFS4_ACE_WRITE_ATTRIBUTES | NFS4_ACE_WRITE_ACL))
		return -EINVAL;
	return 0;
}