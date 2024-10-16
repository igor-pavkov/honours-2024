static void namespace_unlock(void)
{
	struct hlist_head head;

	hlist_move_list(&unmounted, &head);

	up_write(&namespace_sem);

	if (likely(hlist_empty(&head)))
		return;

	synchronize_rcu();

	group_pin_kill(&head);
}