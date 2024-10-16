int con_clear_unimap(struct vc_data *vc)
{
	int ret;
	console_lock();
	ret = con_do_clear_unimap(vc);
	console_unlock();
	return ret;
}