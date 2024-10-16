void mainwindows_init(void)
{
	screen_width = term_width;
	screen_height = term_height;

	mainwindows = NULL;
	active_mainwin = NULL;
	clrtoeol_info = g_new0(MAIN_WINDOW_BORDER_REC, 1);
	screen_reserved_top = screen_reserved_bottom = 0;
	screen_reserved_left = screen_reserved_right = 0;

	command_bind("window grow", NULL, (SIGNAL_FUNC) cmd_window_grow);
	command_bind("window shrink", NULL, (SIGNAL_FUNC) cmd_window_shrink);
	command_bind("window size", NULL, (SIGNAL_FUNC) cmd_window_size);
	command_bind("window balance", NULL, (SIGNAL_FUNC) cmd_window_balance);
	command_bind("window hide", NULL, (SIGNAL_FUNC) cmd_window_hide);
	command_bind("window show", NULL, (SIGNAL_FUNC) cmd_window_show);
	command_bind("window up", NULL, (SIGNAL_FUNC) cmd_window_up);
	command_bind("window down", NULL, (SIGNAL_FUNC) cmd_window_down);
	command_bind("window left", NULL, (SIGNAL_FUNC) cmd_window_left);
	command_bind("window right", NULL, (SIGNAL_FUNC) cmd_window_right);
	command_bind("window dup", NULL, (SIGNAL_FUNC) cmd_window_dup);
	command_bind("window ddown", NULL, (SIGNAL_FUNC) cmd_window_ddown);
	command_bind("window dleft", NULL, (SIGNAL_FUNC) cmd_window_dleft);
	command_bind("window dright", NULL, (SIGNAL_FUNC) cmd_window_dright);
	command_bind("window stick", NULL, (SIGNAL_FUNC) cmd_window_stick);
	command_bind("window move left", NULL, (SIGNAL_FUNC) cmd_window_move_left);
	command_bind("window move right", NULL, (SIGNAL_FUNC) cmd_window_move_right);
	command_bind("window move up", NULL, (SIGNAL_FUNC) cmd_window_move_up);
	command_bind("window move down", NULL, (SIGNAL_FUNC) cmd_window_move_down);
	command_bind("window move dleft", NULL, (SIGNAL_FUNC) cmd_window_move_dleft);
	command_bind("window move dright", NULL, (SIGNAL_FUNC) cmd_window_move_dright);
	command_bind("window rgrow", NULL, (SIGNAL_FUNC) cmd_window_rgrow);
	command_bind("window rshrink", NULL, (SIGNAL_FUNC) cmd_window_rshrink);
	command_bind("window rsize", NULL, (SIGNAL_FUNC) cmd_window_rsize);
	command_bind("window rbalance", NULL, (SIGNAL_FUNC) cmd_window_rbalance);
	command_bind("window rshow", NULL, (SIGNAL_FUNC) cmd_window_rshow);
	signal_add("window print info", (SIGNAL_FUNC) sig_window_print_info);
}