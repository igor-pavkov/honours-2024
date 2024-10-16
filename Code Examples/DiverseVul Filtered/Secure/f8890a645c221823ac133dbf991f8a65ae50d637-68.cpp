static BOOL update_read_synchronize(rdpUpdate* update, wStream* s)
{
	WINPR_UNUSED(update);
	return Stream_SafeSeek(s, 2); /* pad2Octets (2 bytes) */
	                              /**
	                               * The Synchronize Update is an artifact from the
	                               * T.128 protocol and should be ignored.
	                               */
}