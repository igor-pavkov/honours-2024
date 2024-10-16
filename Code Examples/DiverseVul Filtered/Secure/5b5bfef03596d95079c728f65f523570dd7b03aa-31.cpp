static MAIN_WINDOW_REC *find_window_with_room_right(void)
{
	MAIN_WINDOW_REC *biggest_rec;
	GSList *tmp;
	int space, biggest;

	biggest = 0; biggest_rec = NULL;
	for (tmp = mainwindows; tmp != NULL; tmp = tmp->next) {
		MAIN_WINDOW_REC *rec = tmp->data;

		space = MAIN_WINDOW_TEXT_WIDTH(rec);
		if (space >= 2 * NEW_WINDOW_WIDTH && space > biggest) {
			biggest = space;
			biggest_rec = rec;
		}
	}

	return biggest_rec;
}