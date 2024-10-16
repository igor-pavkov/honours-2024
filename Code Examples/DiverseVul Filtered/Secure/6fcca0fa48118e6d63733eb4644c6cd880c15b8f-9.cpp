static void window_reset(struct psi_window *win, u64 now, u64 value,
			 u64 prev_growth)
{
	win->start_time = now;
	win->start_value = value;
	win->prev_growth = prev_growth;
}