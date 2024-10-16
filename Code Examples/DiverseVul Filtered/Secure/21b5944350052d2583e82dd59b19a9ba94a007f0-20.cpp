void net_ns_barrier(void)
{
	mutex_lock(&net_mutex);
	mutex_unlock(&net_mutex);
}