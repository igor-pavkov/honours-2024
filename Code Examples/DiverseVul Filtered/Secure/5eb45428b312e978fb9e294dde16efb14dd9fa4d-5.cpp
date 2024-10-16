static int get_mtu(char *name)
{
	int idx;

	idx = if_nametoindex(name);
	if (idx < 0)
		return -1;
	return netdev_get_mtu(idx);
}