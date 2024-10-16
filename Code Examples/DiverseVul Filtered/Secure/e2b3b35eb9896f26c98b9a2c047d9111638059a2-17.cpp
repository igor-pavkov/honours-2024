static bool vhost_can_busy_poll(struct vhost_dev *dev,
				unsigned long endtime)
{
	return likely(!need_resched()) &&
	       likely(!time_after(busy_clock(), endtime)) &&
	       likely(!signal_pending(current)) &&
	       !vhost_has_work(dev);
}