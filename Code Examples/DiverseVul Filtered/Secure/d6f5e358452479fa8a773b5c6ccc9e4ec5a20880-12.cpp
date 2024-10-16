static int compare_iface(const void *ia, const void *ib)
{
	const struct cifs_server_iface *a = (struct cifs_server_iface *)ia;
	const struct cifs_server_iface *b = (struct cifs_server_iface *)ib;

	return a->speed == b->speed ? 0 : (a->speed > b->speed ? -1 : 1);
}