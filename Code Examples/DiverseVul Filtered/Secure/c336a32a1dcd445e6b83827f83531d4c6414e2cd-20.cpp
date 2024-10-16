static int Special(int c)
{
	switch (c) {
	case '\b':
		BackSpace();
		return 1;
	case '\r':
		Return();
		return 1;
	case '\n':
		if (curr->w_autoaka)
			FindAKA();
	case '\013':		/* Vertical tab is the same as Line Feed */
		LineFeed(0);
		return 1;
	case '\007':
		WBell(curr, visual_bell);
		return 1;
	case '\t':
		ForwardTab();
		return 1;
	case '\017':		/* SI */
		MapCharset(G0);
		return 1;
	case '\016':		/* SO */
		MapCharset(G1);
		return 1;
	}
	return 0;
}