bool cgroup1_ssid_disabled(int ssid)
{
	return cgroup_no_v1_mask & (1 << ssid);
}