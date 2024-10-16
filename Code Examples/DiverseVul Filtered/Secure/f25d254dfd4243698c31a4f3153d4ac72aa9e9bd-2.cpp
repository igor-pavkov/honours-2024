static void huft_free(huft_t *p)
{
	huft_t *q;

	/*
	 * If 'p' has the error bit set we have to clear it, otherwise we might run
	 * into a segmentation fault or an invalid pointer to free(p)
	 */
	if (BAD_HUFT(p)) {
		p = (huft_t*)((uintptr_t)(p) ^ (uintptr_t)(ERR_RET));
	}

	/* Go through linked list, freeing from the malloced (t[-1]) address. */
	while (p) {
		q = (--p)->v.t;
		free(p);
		p = q;
	}
}