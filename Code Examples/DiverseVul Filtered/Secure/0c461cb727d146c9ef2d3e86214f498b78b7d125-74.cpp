static void selinux_inode_getsecid(struct inode *inode, u32 *secid)
{
	struct inode_security_struct *isec = inode_security_novalidate(inode);
	*secid = isec->sid;
}