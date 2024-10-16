int URI_FUNC(CompareRange)(
		const URI_TYPE(TextRange) * a,
		const URI_TYPE(TextRange) * b) {
	int diff;

	/* NOTE: Both NULL means equal! */
	if ((a == NULL) || (b == NULL)) {
		return ((a == NULL) ? 0 : 1) - ((b == NULL) ? 0 : 1);
	}

	/* NOTE: Both NULL means equal! */
	if ((a->first == NULL) || (b->first == NULL)) {
		return ((a->first == NULL) ? 0 : 1) - ((b->first == NULL) ? 0 : 1);
	}

	diff = ((int)(a->afterLast - a->first) - (int)(b->afterLast - b->first));
	if (diff > 0) {
		return 1;
	} else if (diff < 0) {
		return -1;
	}

	diff = URI_STRNCMP(a->first, b->first, (a->afterLast - a->first));

	if (diff > 0) {
		return 1;
	} else if (diff < 0) {
		return -1;
	}

	return diff;
}