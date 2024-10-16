static void timeout_work(struct work_struct *work)
{
	struct tpm_chip *chip = container_of(work, struct tpm_chip, work);

	mutex_lock(&chip->buffer_mutex);
	atomic_set(&chip->data_pending, 0);
	memset(chip->data_buffer, 0, TPM_BUFSIZE);
	mutex_unlock(&chip->buffer_mutex);
}