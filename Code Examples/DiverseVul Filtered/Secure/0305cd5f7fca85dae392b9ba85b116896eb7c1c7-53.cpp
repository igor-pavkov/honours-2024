static int wait_snapshoting_atomic_t(atomic_t *a)
{
	schedule();
	return 0;
}