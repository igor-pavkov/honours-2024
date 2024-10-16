void ldsem_up_read(struct ld_semaphore *sem)
{
	long count;

	lockdep_release(sem, 1, _RET_IP_);

	count = ldsem_atomic_update(-LDSEM_READ_BIAS, sem);
	if (count < 0 && (count & LDSEM_ACTIVE_MASK) == 0)
		ldsem_wake(sem);
}