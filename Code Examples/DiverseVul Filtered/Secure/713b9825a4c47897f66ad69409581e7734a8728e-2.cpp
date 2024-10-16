static bool io_flush_signals(void)
{
	if (unlikely(test_thread_flag(TIF_NOTIFY_SIGNAL))) {
		__set_current_state(TASK_RUNNING);
		tracehook_notify_signal();
		return true;
	}
	return false;
}