static void StringChar(int c)
{
	if (curr->w_stringp >= curr->w_string + MAXSTR - 1)
		curr->w_state = LIT;
	else
		*(curr->w_stringp)++ = c;
}