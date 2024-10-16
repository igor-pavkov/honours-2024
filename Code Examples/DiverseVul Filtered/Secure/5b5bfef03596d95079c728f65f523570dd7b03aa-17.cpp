static void sig_window_print_info(WINDOW_REC *win)
{
	GUI_WINDOW_REC *gui;

	gui = WINDOW_GUI(win);
	if (gui->use_scroll) {
		printformat_window(win, MSGLEVEL_CLIENTCRAP,
				   TXT_WINDOW_INFO_SCROLL,
				   gui->scroll ? "yes" : "no");
	}

	if (WINDOW_MAIN(win)->sticky_windows)
                windows_print_sticky(win);
}