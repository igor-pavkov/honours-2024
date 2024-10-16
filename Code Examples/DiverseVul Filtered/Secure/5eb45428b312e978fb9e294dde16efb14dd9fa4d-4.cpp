bool cgns_supported(void)
{
	return file_exists("/proc/self/ns/cgroup");
}