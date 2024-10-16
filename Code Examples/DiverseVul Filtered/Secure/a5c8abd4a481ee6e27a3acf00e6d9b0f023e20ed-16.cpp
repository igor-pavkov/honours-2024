void set_bitmaps(struct activity *act[], uint64_t *flags)
{
	int p;

	if (!USE_OPTION_P(*flags)) {
		/* Force -P ALL */
		p = get_activity_position(act, A_CPU, EXIT_IF_NOT_FOUND);
		memset(act[p]->bitmap->b_array, ~0,
		       BITMAP_SIZE(act[p]->bitmap->b_size));
	}

	if (!USE_OPTION_I(*flags)) {
		/* Force -I ALL */
		p = get_activity_position(act, A_IRQ, EXIT_IF_NOT_FOUND);
		memset(act[p]->bitmap->b_array, ~0,
		       BITMAP_SIZE(act[p]->bitmap->b_size));
	}
}