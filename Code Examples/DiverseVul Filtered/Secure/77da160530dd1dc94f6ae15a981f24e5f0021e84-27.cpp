static void disk_events_workfn(struct work_struct *work)
{
	struct delayed_work *dwork = to_delayed_work(work);
	struct disk_events *ev = container_of(dwork, struct disk_events, dwork);

	disk_check_events(ev, &ev->clearing);
}