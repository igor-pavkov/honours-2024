static u64 window_update(struct psi_window *win, u64 now, u64 value)
{
	u64 elapsed;
	u64 growth;

	elapsed = now - win->start_time;
	growth = value - win->start_value;
	/*
	 * After each tracking window passes win->start_value and
	 * win->start_time get reset and win->prev_growth stores
	 * the average per-window growth of the previous window.
	 * win->prev_growth is then used to interpolate additional
	 * growth from the previous window assuming it was linear.
	 */
	if (elapsed > win->size)
		window_reset(win, now, value, growth);
	else {
		u32 remaining;

		remaining = win->size - elapsed;
		growth += div64_u64(win->prev_growth * remaining, win->size);
	}

	return growth;
}