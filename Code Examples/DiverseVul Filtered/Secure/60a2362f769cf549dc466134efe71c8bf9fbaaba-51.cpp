static int _notifier_call_chain(struct regulator_dev *rdev,
				  unsigned long event, void *data)
{
	/* call rdev chain first */
	return blocking_notifier_call_chain(&rdev->notifier, event, data);
}