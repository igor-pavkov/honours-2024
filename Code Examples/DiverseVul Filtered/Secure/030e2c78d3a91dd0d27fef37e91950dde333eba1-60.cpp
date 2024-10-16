static int snd_seq_ioctl_set_port_info(struct snd_seq_client *client,
				       void __user *arg)
{
	struct snd_seq_client_port *port;
	struct snd_seq_port_info info;

	if (copy_from_user(&info, arg, sizeof(info)))
		return -EFAULT;

	if (info.addr.client != client->number) /* only set our own ports ! */
		return -EPERM;
	port = snd_seq_port_use_ptr(client, info.addr.port);
	if (port) {
		snd_seq_set_port_info(port, &info);
		snd_seq_port_unlock(port);
	}
	return 0;
}