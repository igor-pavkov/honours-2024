static void PrintChar(int c)
{
	if (curr->w_stringp >= curr->w_string + MAXSTR - 1)
		PrintFlush();
	*(curr->w_stringp)++ = c;
}