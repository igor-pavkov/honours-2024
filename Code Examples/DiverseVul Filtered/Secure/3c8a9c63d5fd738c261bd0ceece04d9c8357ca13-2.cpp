static struct tun_struct *tun_get(struct file *file)
{
	return __tun_get(file->private_data);
}