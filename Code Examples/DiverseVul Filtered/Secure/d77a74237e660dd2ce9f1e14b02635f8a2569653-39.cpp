static int sys_broken_setgroups(int setlen, gid_t *gidset)
{
	GID_T *group_list;
	int i ; 

	if (setlen == 0)
		return 0 ;

	if (setlen < 0 || setlen > groups_max()) {
		errno = EINVAL; 
		return -1;   
	}

	/*
	 * Broken case. We need to allocate a
	 * GID_T array of size setlen.
	 */

	if((group_list = SMB_MALLOC_ARRAY(GID_T, setlen)) == NULL) {
		DEBUG(0,("sys_setgroups: Malloc fail.\n"));
		return -1;    
	}

	for(i = 0; i < setlen; i++) 
		group_list[i] = (GID_T) gidset[i]; 

	if(samba_setgroups(setlen, group_list) != 0) {
		int saved_errno = errno;
		SAFE_FREE(group_list);
		errno = saved_errno;
		return -1;
	}

	SAFE_FREE(group_list);
	return 0 ;
}