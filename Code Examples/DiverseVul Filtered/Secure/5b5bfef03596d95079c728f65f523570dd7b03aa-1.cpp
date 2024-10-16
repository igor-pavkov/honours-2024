static MAIN_WINDOW_REC *find_window_with_room()
{
	MAIN_WINDOW_REC *biggest_rec;
	GSList *tmp;
	int space, biggest;

	biggest = 0; biggest_rec = NULL;
	for (tmp = mainwindows; tmp != NULL; tmp = tmp->next) {
		MAIN_WINDOW_REC *rec = tmp->data;

		space = MAIN_WINDOW_TEXT_HEIGHT(rec);
		if (space >= WINDOW_MIN_SIZE+NEW_WINDOW_SIZE && space > biggest) {
			biggest = space;
			biggest_rec = rec;
		}
	}

	return biggest_rec;
}