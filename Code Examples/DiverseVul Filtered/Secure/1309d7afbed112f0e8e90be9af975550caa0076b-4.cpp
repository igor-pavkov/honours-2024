int tpm_release(struct inode *inode, struct file *file)
{
	struct tpm_chip *chip = file->private_data;

	del_singleshot_timer_sync(&chip->user_read_timer);
	flush_work_sync(&chip->work);
	file->private_data = NULL;
	atomic_set(&chip->data_pending, 0);
	kfree(chip->data_buffer);
	clear_bit(0, &chip->is_open);
	put_device(chip->dev);
	return 0;
}