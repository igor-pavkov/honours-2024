static void Bfree(Bigint *v)
{
	if (v) {
		_THREAD_PRIVATE_MUTEX_LOCK(dtoa_mutex);
		v->next = freelist[v->k];
		freelist[v->k] = v;
		_THREAD_PRIVATE_MUTEX_UNLOCK(dtoa_mutex);
	}
}