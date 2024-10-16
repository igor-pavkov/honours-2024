bool lxc_nic_exists(char *nic)
{
#define __LXC_SYS_CLASS_NET_LEN 15 + IFNAMSIZ + 1
	char path[__LXC_SYS_CLASS_NET_LEN];
	int ret;
	struct stat sb;

	if (!strcmp(nic, "none"))
		return true;

	ret = snprintf(path, __LXC_SYS_CLASS_NET_LEN, "/sys/class/net/%s", nic);
	if (ret < 0 || (size_t)ret >= __LXC_SYS_CLASS_NET_LEN)
		return false;

	ret = stat(path, &sb);
	if (ret < 0)
		return false;

	return true;
}