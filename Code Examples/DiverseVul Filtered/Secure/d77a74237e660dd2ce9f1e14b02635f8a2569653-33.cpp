int sys_setgroups(gid_t UNUSED(primary_gid), int setlen, gid_t *gidset)
{
#if !defined(HAVE_SETGROUPS)
	errno = ENOSYS;
	return -1;
#endif /* HAVE_SETGROUPS */

#if defined(USE_BSD_SETGROUPS)
	return sys_bsd_setgroups(primary_gid, setlen, gidset);
#elif defined(HAVE_BROKEN_GETGROUPS)
	return sys_broken_setgroups(setlen, gidset);
#else
	return samba_setgroups(setlen, gidset);
#endif
}