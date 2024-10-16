static int sys_bsd_setgroups(gid_t primary_gid, int setlen, const gid_t *gidset)
{
	gid_t *new_gidset = NULL;
	int max;
	int ret;

	/* setgroups(2) will fail with EINVAL if we pass too many groups. */
	max = groups_max();

	/* No group list, just make sure we are setting the efective GID. */
	if (setlen == 0) {
		return samba_setgroups(1, &primary_gid);
	}

	/* If the primary gid is not the first array element, grow the array
	 * and insert it at the front.
	 */
	if (gidset[0] != primary_gid) {
	        new_gidset = SMB_MALLOC_ARRAY(gid_t, setlen + 1);
	        if (new_gidset == NULL) {
			return -1;
	        }

		memcpy(new_gidset + 1, gidset, (setlen * sizeof(gid_t)));
		new_gidset[0] = primary_gid;
		setlen++;
	}

	if (setlen > max) {
		DEBUG(3, ("forced to truncate group list from %d to %d\n",
			setlen, max));
		setlen = max;
	}

#if defined(HAVE_BROKEN_GETGROUPS)
	ret = sys_broken_setgroups(setlen, new_gidset ? new_gidset : gidset);
#else
	ret = samba_setgroups(setlen, new_gidset ? new_gidset : gidset);
#endif

	if (new_gidset) {
		int errsav = errno;
		SAFE_FREE(new_gidset);
		errno = errsav;
	}

	return ret;
}