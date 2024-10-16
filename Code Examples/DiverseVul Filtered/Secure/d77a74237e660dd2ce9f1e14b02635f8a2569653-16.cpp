int sys_getgroups(int setlen, gid_t *gidset)
{
#if defined(HAVE_BROKEN_GETGROUPS)
	return sys_broken_getgroups(setlen, gidset);
#else
	return getgroups(setlen, gidset);
#endif
}