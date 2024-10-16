static enum hrtimer_restart pit_timer_fn(struct hrtimer *data)
{
	struct kvm_kpit_state *ps = container_of(data, struct kvm_kpit_state, timer);
	struct kvm_pit *pt = ps->kvm->arch.vpit;

	if (ps->reinject || !atomic_read(&ps->pending)) {
		atomic_inc(&ps->pending);
		queue_kthread_work(&pt->worker, &pt->expired);
	}

	if (ps->is_periodic) {
		hrtimer_add_expires_ns(&ps->timer, ps->period);
		return HRTIMER_RESTART;
	} else
		return HRTIMER_NORESTART;
}