static __poll_t snd_disconnect_poll(struct file * file, poll_table * wait)
{
	return EPOLLERR | EPOLLNVAL;
}