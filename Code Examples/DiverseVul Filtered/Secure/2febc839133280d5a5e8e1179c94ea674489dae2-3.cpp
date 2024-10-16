static void destroy_pit_timer(struct kvm_pit *pit)
{
	hrtimer_cancel(&pit->pit_state.timer);
	flush_kthread_work(&pit->expired);
}