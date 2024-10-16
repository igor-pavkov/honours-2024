static void destroy_freelist(void)
{
	int i;
	Bigint *tmp;

	_THREAD_PRIVATE_MUTEX_LOCK(dtoa_mutex);
	for (i = 0; i <= Kmax; i++) {
		Bigint **listp = &freelist[i];
		while ((tmp = *listp) != NULL) {
			*listp = tmp->next;
			free(tmp);
		}
		freelist[i] = NULL;
	}
	_THREAD_PRIVATE_MUTEX_UNLOCK(dtoa_mutex);
	
}