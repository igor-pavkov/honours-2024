static void btrfs_wait_for_no_snapshoting_writes(struct btrfs_root *root)
{
	s64 writers;
	DEFINE_WAIT(wait);

	do {
		prepare_to_wait(&root->subv_writers->wait, &wait,
				TASK_UNINTERRUPTIBLE);

		writers = percpu_counter_sum(&root->subv_writers->counter);
		if (writers)
			schedule();

		finish_wait(&root->subv_writers->wait, &wait);
	} while (writers);
}