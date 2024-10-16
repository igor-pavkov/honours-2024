void mainwindows_deinit(void)
{
	while (mainwindows != NULL)
		mainwindow_destroy(mainwindows->data);
	g_free(clrtoeol_info);

	command_unbind("window grow", (SIGNAL_FUNC) cmd_window_grow);
	command_unbind("window shrink", (SIGNAL_FUNC) cmd_window_shrink);
	command_unbind("window size", (SIGNAL_FUNC) cmd_window_size);
	command_unbind("window balance", (SIGNAL_FUNC) cmd_window_balance);
	command_unbind("window hide", (SIGNAL_FUNC) cmd_window_hide);
	command_unbind("window show", (SIGNAL_FUNC) cmd_window_show);
	command_unbind("window up", (SIGNAL_FUNC) cmd_window_up);
	command_unbind("window down", (SIGNAL_FUNC) cmd_window_down);
	command_unbind("window left", (SIGNAL_FUNC) cmd_window_left);
	command_unbind("window right", (SIGNAL_FUNC) cmd_window_right);
	command_unbind("window dup", (SIGNAL_FUNC) cmd_window_dup);
	command_unbind("window ddown", (SIGNAL_FUNC) cmd_window_ddown);
	command_unbind("window dleft", (SIGNAL_FUNC) cmd_window_dleft);
	command_unbind("window dright", (SIGNAL_FUNC) cmd_window_dright);
	command_unbind("window stick", (SIGNAL_FUNC) cmd_window_stick);
	command_unbind("window move left", (SIGNAL_FUNC) cmd_window_move_left);
	command_unbind("window move right", (SIGNAL_FUNC) cmd_window_move_right);
	command_unbind("window move up", (SIGNAL_FUNC) cmd_window_move_up);
	command_unbind("window move down", (SIGNAL_FUNC) cmd_window_move_down);
	command_unbind("window move dleft", (SIGNAL_FUNC) cmd_window_move_dleft);
	command_unbind("window move dright", (SIGNAL_FUNC) cmd_window_move_dright);
	command_unbind("window rgrow", (SIGNAL_FUNC) cmd_window_rgrow);
	command_unbind("window rshrink", (SIGNAL_FUNC) cmd_window_rshrink);
	command_unbind("window rsize", (SIGNAL_FUNC) cmd_window_rsize);
	command_unbind("window rbalance", (SIGNAL_FUNC) cmd_window_rbalance);
	command_unbind("window rshow", (SIGNAL_FUNC) cmd_window_rshow);
	signal_remove("window print info", (SIGNAL_FUNC) sig_window_print_info);
}