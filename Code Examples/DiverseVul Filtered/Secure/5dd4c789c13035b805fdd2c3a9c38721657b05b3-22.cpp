static bool notify_change_record_identical(struct notify_change_event *c1,
					   struct notify_change_event *c2)
{
	/* Note this is deliberately case sensitive. */
	if (c1->action == c2->action &&
			strcmp(c1->name, c2->name) == 0) {
		return True;
	}
	return False;
}