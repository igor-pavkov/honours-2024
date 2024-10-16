static int snd_seq_do_ioctl(struct snd_seq_client *client, unsigned int cmd,
			    void __user *arg)
{
	struct seq_ioctl_table *p;

	switch (cmd) {
	case SNDRV_SEQ_IOCTL_PVERSION:
		/* return sequencer version number */
		return put_user(SNDRV_SEQ_VERSION, (int __user *)arg) ? -EFAULT : 0;
	case SNDRV_SEQ_IOCTL_CLIENT_ID:
		/* return the id of this client */
		return put_user(client->number, (int __user *)arg) ? -EFAULT : 0;
	}

	if (! arg)
		return -EFAULT;
	for (p = ioctl_tables; p->cmd; p++) {
		if (p->cmd == cmd)
			return p->func(client, arg);
	}
	pr_debug("ALSA: seq unknown ioctl() 0x%x (type='%c', number=0x%02x)\n",
		   cmd, _IOC_TYPE(cmd), _IOC_NR(cmd));
	return -ENOTTY;
}