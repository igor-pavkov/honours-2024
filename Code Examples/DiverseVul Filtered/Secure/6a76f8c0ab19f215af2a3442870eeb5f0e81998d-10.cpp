unsigned long ftrace_location(unsigned long ip)
{
	return ftrace_location_range(ip, ip);
}