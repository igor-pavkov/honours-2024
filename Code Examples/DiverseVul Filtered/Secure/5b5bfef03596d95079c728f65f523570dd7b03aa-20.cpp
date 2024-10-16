static int mainwindows_compare_reverse(MAIN_WINDOW_REC *w1, MAIN_WINDOW_REC *w2)
{
	return    w1->first_line   < w2->first_line   ?  1
		: w1->first_line   > w2->first_line   ? -1
		: w1->first_column < w2->first_column ?  1
		: w1->first_column > w2->first_column ? -1
		: 0;
}