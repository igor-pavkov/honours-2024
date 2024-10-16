static bool marshall_posix_acl(connection_struct *conn, char *pdata, SMB_STRUCT_STAT *pst, SMB_ACL_T posix_acl)
{
	int entry_id = SMB_ACL_FIRST_ENTRY;
	SMB_ACL_ENTRY_T entry;

	while ( posix_acl && (sys_acl_get_entry(posix_acl, entry_id, &entry) == 1)) {
		SMB_ACL_TAG_T tagtype;
		SMB_ACL_PERMSET_T permset;
		unsigned char perms = 0;
		unsigned int own_grp;

		/* get_next... */
		if (entry_id == SMB_ACL_FIRST_ENTRY) {
			entry_id = SMB_ACL_NEXT_ENTRY;
		}

		if (sys_acl_get_tag_type(entry, &tagtype) == -1) {
			DEBUG(0,("marshall_posix_acl: SMB_VFS_SYS_ACL_GET_TAG_TYPE failed.\n"));
			return False;
		}

		if (sys_acl_get_permset(entry, &permset) == -1) {
			DEBUG(0,("marshall_posix_acl: SMB_VFS_SYS_ACL_GET_PERMSET failed.\n"));
			return False;
		}

		perms |= (sys_acl_get_perm(permset, SMB_ACL_READ) ? SMB_POSIX_ACL_READ : 0);
		perms |= (sys_acl_get_perm(permset, SMB_ACL_WRITE) ? SMB_POSIX_ACL_WRITE : 0);
		perms |= (sys_acl_get_perm(permset, SMB_ACL_EXECUTE) ? SMB_POSIX_ACL_EXECUTE : 0);

		SCVAL(pdata,1,perms);

		switch (tagtype) {
			case SMB_ACL_USER_OBJ:
				SCVAL(pdata,0,SMB_POSIX_ACL_USER_OBJ);
				own_grp = (unsigned int)pst->st_ex_uid;
				SIVAL(pdata,2,own_grp);
				SIVAL(pdata,6,0);
				break;
			case SMB_ACL_USER:
				{
					uid_t *puid = (uid_t *)sys_acl_get_qualifier(entry);
					if (!puid) {
						DEBUG(0,("marshall_posix_acl: SMB_VFS_SYS_ACL_GET_QUALIFIER failed.\n"));
						return False;
					}
					own_grp = (unsigned int)*puid;
					SCVAL(pdata,0,SMB_POSIX_ACL_USER);
					SIVAL(pdata,2,own_grp);
					SIVAL(pdata,6,0);
					break;
				}
			case SMB_ACL_GROUP_OBJ:
				SCVAL(pdata,0,SMB_POSIX_ACL_GROUP_OBJ);
				own_grp = (unsigned int)pst->st_ex_gid;
				SIVAL(pdata,2,own_grp);
				SIVAL(pdata,6,0);
				break;
			case SMB_ACL_GROUP:
				{
					gid_t *pgid= (gid_t *)sys_acl_get_qualifier(entry);
					if (!pgid) {
						DEBUG(0,("marshall_posix_acl: SMB_VFS_SYS_ACL_GET_QUALIFIER failed.\n"));
						return False;
					}
					own_grp = (unsigned int)*pgid;
					SCVAL(pdata,0,SMB_POSIX_ACL_GROUP);
					SIVAL(pdata,2,own_grp);
					SIVAL(pdata,6,0);
					break;
				}
			case SMB_ACL_MASK:
				SCVAL(pdata,0,SMB_POSIX_ACL_MASK);
				SIVAL(pdata,2,0xFFFFFFFF);
				SIVAL(pdata,6,0xFFFFFFFF);
				break;
			case SMB_ACL_OTHER:
				SCVAL(pdata,0,SMB_POSIX_ACL_OTHER);
				SIVAL(pdata,2,0xFFFFFFFF);
				SIVAL(pdata,6,0xFFFFFFFF);
				break;
			default:
				DEBUG(0,("marshall_posix_acl: unknown tagtype.\n"));
				return False;
		}
		pdata += SMB_POSIX_ACL_ENTRY_SIZE;
	}

	return True;
}