static ssize_t tun_chr_aio_read(struct kiocb *iocb, const struct iovec *iv,
			    unsigned long count, loff_t pos)
{
	struct file *file = iocb->ki_filp;
	struct tun_file *tfile = file->private_data;
	struct tun_struct *tun = __tun_get(tfile);
	DECLARE_WAITQUEUE(wait, current);
	struct sk_buff *skb;
	ssize_t len, ret = 0;

	if (!tun)
		return -EBADFD;

	DBG(KERN_INFO "%s: tun_chr_read\n", tun->dev->name);

	len = iov_length(iv, count);
	if (len < 0) {
		ret = -EINVAL;
		goto out;
	}

	add_wait_queue(&tun->socket.wait, &wait);
	while (len) {
		current->state = TASK_INTERRUPTIBLE;

		/* Read frames from the queue */
		if (!(skb=skb_dequeue(&tun->readq))) {
			if (file->f_flags & O_NONBLOCK) {
				ret = -EAGAIN;
				break;
			}
			if (signal_pending(current)) {
				ret = -ERESTARTSYS;
				break;
			}
			if (tun->dev->reg_state != NETREG_REGISTERED) {
				ret = -EIO;
				break;
			}

			/* Nothing to read, let's sleep */
			schedule();
			continue;
		}
		netif_wake_queue(tun->dev);

		ret = tun_put_user(tun, skb, iv, len);
		kfree_skb(skb);
		break;
	}

	current->state = TASK_RUNNING;
	remove_wait_queue(&tun->socket.wait, &wait);

out:
	tun_put(tun);
	return ret;
}