static void rose_destroy_timer(unsigned long data)
{
	rose_destroy_socket((struct sock *)data);
}