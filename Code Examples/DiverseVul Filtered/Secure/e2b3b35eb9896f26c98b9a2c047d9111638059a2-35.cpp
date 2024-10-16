static struct ptr_ring *get_tap_ptr_ring(int fd)
{
	struct ptr_ring *ring;
	struct file *file = fget(fd);

	if (!file)
		return NULL;
	ring = tun_get_tx_ring(file);
	if (!IS_ERR(ring))
		goto out;
	ring = tap_get_ptr_ring(file);
	if (!IS_ERR(ring))
		goto out;
	ring = NULL;
out:
	fput(file);
	return ring;
}