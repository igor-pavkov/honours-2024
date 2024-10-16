void drop_effective_capability(enum smbd_capability capability)
{
#if defined(HAVE_POSIX_CAPABILITIES)
	set_process_capability(capability, False);
#endif /* HAVE_POSIX_CAPABILITIES */
}