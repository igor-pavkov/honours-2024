int rtnl_is_locked(void)
{
	return mutex_is_locked(&rtnl_mutex);
}