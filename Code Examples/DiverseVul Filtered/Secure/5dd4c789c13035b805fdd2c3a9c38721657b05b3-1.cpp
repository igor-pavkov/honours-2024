static int compare_notify_change_events(const void *p1, const void *p2)
{
	const struct notify_change_event *e1 = p1;
	const struct notify_change_event *e2 = p2;

	return timespec_compare(&e1->when, &e2->when);
}