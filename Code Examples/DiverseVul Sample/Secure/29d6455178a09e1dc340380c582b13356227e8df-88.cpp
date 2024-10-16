void preempt_notifier_register(struct preempt_notifier *notifier)
{
	if (!static_key_false(&preempt_notifier_key))
		WARN(1, "registering preempt_notifier while notifiers disabled\n");

	hlist_add_head(&notifier->link, &current->preempt_notifiers);
}