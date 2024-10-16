Bool gf_prompt_has_input()
{
	u8 ch;
	s32 nread;
	pid_t fg = tcgetpgrp(STDIN_FILENO);

	//we are not foreground nor piped (used for IDEs), can't read stdin
	if ((fg!=-1) && (fg != getpgrp())) {
		return 0;
	}
	init_keyboard();
	if (ch_peek != -1) return 1;
	t_new.c_cc[VMIN]=0;
	tcsetattr(0, TCSANOW, &t_new);
	nread = (s32) read(0, &ch, 1);
	t_new.c_cc[VMIN]=1;
	tcsetattr(0, TCSANOW, &t_new);
	if(nread == 1) {
		ch_peek = ch;
		return 1;
	}
	close_keyboard(0);
	return 0;
}