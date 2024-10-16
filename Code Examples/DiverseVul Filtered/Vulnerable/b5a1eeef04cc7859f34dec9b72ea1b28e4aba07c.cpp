static ssize_t bat_socket_read(struct file *file, char __user *buf,
			       size_t count, loff_t *ppos)
{
	struct socket_client *socket_client = file->private_data;
	struct socket_packet *socket_packet;
	size_t packet_len;
	int error;

	if ((file->f_flags & O_NONBLOCK) && (socket_client->queue_len == 0))
		return -EAGAIN;

	if ((!buf) || (count < sizeof(struct icmp_packet)))
		return -EINVAL;

	if (!access_ok(VERIFY_WRITE, buf, count))
		return -EFAULT;

	error = wait_event_interruptible(socket_client->queue_wait,
					 socket_client->queue_len);

	if (error)
		return error;

	spin_lock_bh(&socket_client->lock);

	socket_packet = list_first_entry(&socket_client->queue_list,
					 struct socket_packet, list);
	list_del(&socket_packet->list);
	socket_client->queue_len--;

	spin_unlock_bh(&socket_client->lock);

	error = copy_to_user(buf, &socket_packet->icmp_packet,
			     socket_packet->icmp_len);

	packet_len = socket_packet->icmp_len;
	kfree(socket_packet);

	if (error)
		return -EFAULT;

	return packet_len;
}