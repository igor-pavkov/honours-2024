void zend_accel_shared_protect(int mode)
{
#ifdef HAVE_MPROTECT
	int i;

	if (mode) {
		mode = PROT_READ;
	} else {
		mode = PROT_READ|PROT_WRITE;
	}

	for (i = 0; i < ZSMMG(shared_segments_count); i++) {
		mprotect(ZSMMG(shared_segments)[i]->p, ZSMMG(shared_segments)[i]->size, mode);
	}
#endif
}