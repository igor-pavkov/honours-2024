void set_effective_capability(enum smbd_capability capability)
{
#if defined(HAVE_POSIX_CAPABILITIES)
	set_process_capability(capability, True);
#endif /* HAVE_POSIX_CAPABILITIES */
}