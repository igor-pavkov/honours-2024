static int bat_socket_open(struct inode *inode, struct file *file)
{
	unsigned int i;
	struct socket_client *socket_client;

	nonseekable_open(inode, file);

	socket_client = kmalloc(sizeof(*socket_client), GFP_KERNEL);

	if (!socket_client)
		return -ENOMEM;

	for (i = 0; i < ARRAY_SIZE(socket_client_hash); i++) {
		if (!socket_client_hash[i]) {
			socket_client_hash[i] = socket_client;
			break;
		}
	}

	if (i == ARRAY_SIZE(socket_client_hash)) {
		pr_err("Error - can't add another packet client: "
		       "maximum number of clients reached\n");
		kfree(socket_client);
		return -EXFULL;
	}

	INIT_LIST_HEAD(&socket_client->queue_list);
	socket_client->queue_len = 0;
	socket_client->index = i;
	socket_client->bat_priv = inode->i_private;
	spin_lock_init(&socket_client->lock);
	init_waitqueue_head(&socket_client->queue_wait);

	file->private_data = socket_client;

	inc_module_count();
	return 0;
}