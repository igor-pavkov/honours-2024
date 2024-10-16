static inline bool io_should_retry_thread(long err)
{
	switch (err) {
	case -EAGAIN:
	case -ERESTARTSYS:
	case -ERESTARTNOINTR:
	case -ERESTARTNOHAND:
		return true;
	default:
		return false;
	}
}