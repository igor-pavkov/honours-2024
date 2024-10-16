static void __ldsem_wake(struct ld_semaphore *sem)
{
	if (!list_empty(&sem->write_wait))
		__ldsem_wake_writer(sem);
	else if (!list_empty(&sem->read_wait))
		__ldsem_wake_readers(sem);
}