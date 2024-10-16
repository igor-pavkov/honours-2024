char gf_prompt_get_char()
{
	char ch;
	if (ch_peek != -1) {
		ch = ch_peek;
		ch_peek = -1;
		close_keyboard(1);
		return ch;
	}
	if (0==read(0,&ch,1))
		ch = 0;
	close_keyboard(1);
	return ch;
}