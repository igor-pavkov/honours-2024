static int read_index(struct iowarrior *dev)
{
	int intr_idx, read_idx;

	read_idx = atomic_read(&dev->read_idx);
	intr_idx = atomic_read(&dev->intr_idx);

	return (read_idx == intr_idx ? -1 : read_idx);
}