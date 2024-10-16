static void no_memory_bailout(size_t allocate_size, char *error)
{
	zend_accel_error(ACCEL_LOG_FATAL, "Unable to allocate shared memory segment of %ld bytes: %s: %s (%d)", allocate_size, error?error:"unknown", strerror(errno), errno );
}