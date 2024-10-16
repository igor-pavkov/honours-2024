static inline long ldsem_atomic_update(long delta, struct ld_semaphore *sem)
{
	return atomic_long_add_return(delta, (atomic_long_t *)&sem->count);
}