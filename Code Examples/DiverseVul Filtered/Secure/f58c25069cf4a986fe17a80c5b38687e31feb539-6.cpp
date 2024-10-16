UriBool URI_FUNC(RemoveDotSegmentsEx)(URI_TYPE(Uri) * uri,
		UriBool relative, UriBool pathOwned, UriMemoryManager * memory) {
	URI_TYPE(PathSegment) * walker;
	if ((uri == NULL) || (uri->pathHead == NULL)) {
		return URI_TRUE;
	}

	walker = uri->pathHead;
	walker->reserved = NULL; /* Prev pointer */
	do {
		UriBool removeSegment = URI_FALSE;
		int len = (int)(walker->text.afterLast - walker->text.first);
		switch (len) {
		case 1:
			if ((walker->text.first)[0] == _UT('.')) {
				/* "." segment -> remove if not essential */
				URI_TYPE(PathSegment) * const prev = walker->reserved;
				URI_TYPE(PathSegment) * const nextBackup = walker->next;

				/* Is this dot segment essential? */
				removeSegment = URI_TRUE;
				if (relative && (walker == uri->pathHead) && (walker->next != NULL)) {
					const URI_CHAR * ch = walker->next->text.first;
					for (; ch < walker->next->text.afterLast; ch++) {
						if (*ch == _UT(':')) {
							removeSegment = URI_FALSE;
							break;
						}
					}
				}

				if (removeSegment) {
					/* Last segment? */
					if (walker->next != NULL) {
						/* Not last segment */
						walker->next->reserved = prev;

						if (prev == NULL) {
							/* First but not last segment */
							uri->pathHead = walker->next;
						} else {
							/* Middle segment */
							prev->next = walker->next;
						}

						if (pathOwned && (walker->text.first != walker->text.afterLast)) {
							memory->free(memory, (URI_CHAR *)walker->text.first);
						}
						memory->free(memory, walker);
					} else {
						/* Last segment */
						if (pathOwned && (walker->text.first != walker->text.afterLast)) {
							memory->free(memory, (URI_CHAR *)walker->text.first);
						}

						if (prev == NULL) {
							/* Last and first */
							if (URI_FUNC(IsHostSet)(uri)) {
								/* Replace "." with empty segment to represent trailing slash */
								walker->text.first = URI_FUNC(SafeToPointTo);
								walker->text.afterLast = URI_FUNC(SafeToPointTo);
							} else {
								memory->free(memory, walker);

								uri->pathHead = NULL;
								uri->pathTail = NULL;
							}
						} else {
							/* Last but not first, replace "." with empty segment to represent trailing slash */
							walker->text.first = URI_FUNC(SafeToPointTo);
							walker->text.afterLast = URI_FUNC(SafeToPointTo);
						}
					}

					walker = nextBackup;
				}
			}
			break;

		case 2:
			if (((walker->text.first)[0] == _UT('.'))
					&& ((walker->text.first)[1] == _UT('.'))) {
				/* Path ".." -> remove this and the previous segment */
				URI_TYPE(PathSegment) * const prev = walker->reserved;
				URI_TYPE(PathSegment) * prevPrev;
				URI_TYPE(PathSegment) * const nextBackup = walker->next;

				removeSegment = URI_TRUE;
				if (relative) {
					if (prev == NULL) {
						removeSegment = URI_FALSE;
					} else if ((prev != NULL)
							&& ((prev->text.afterLast - prev->text.first) == 2)
							&& ((prev->text.first)[0] == _UT('.'))
							&& ((prev->text.first)[1] == _UT('.'))) {
						removeSegment = URI_FALSE;
					}
				}

				if (removeSegment) {
					if (prev != NULL) {
						/* Not first segment */
						prevPrev = prev->reserved;
						if (prevPrev != NULL) {
							/* Not even prev is the first one */
							prevPrev->next = walker->next;
							if (walker->next != NULL) {
								walker->next->reserved = prevPrev;
							} else {
								/* Last segment -> insert "" segment to represent trailing slash, update tail */
								URI_TYPE(PathSegment) * const segment = memory->calloc(memory, 1, sizeof(URI_TYPE(PathSegment)));
								if (segment == NULL) {
									if (pathOwned && (walker->text.first != walker->text.afterLast)) {
										memory->free(memory, (URI_CHAR *)walker->text.first);
									}
									memory->free(memory, walker);

									if (pathOwned && (prev->text.first != prev->text.afterLast)) {
										memory->free(memory, (URI_CHAR *)prev->text.first);
									}
									memory->free(memory, prev);

									return URI_FALSE; /* Raises malloc error */
								}
								segment->text.first = URI_FUNC(SafeToPointTo);
								segment->text.afterLast = URI_FUNC(SafeToPointTo);
								prevPrev->next = segment;
								uri->pathTail = segment;
							}

							if (pathOwned && (walker->text.first != walker->text.afterLast)) {
								memory->free(memory, (URI_CHAR *)walker->text.first);
							}
							memory->free(memory, walker);

							if (pathOwned && (prev->text.first != prev->text.afterLast)) {
								memory->free(memory, (URI_CHAR *)prev->text.first);
							}
							memory->free(memory, prev);

							walker = nextBackup;
						} else {
							/* Prev is the first segment */
							if (walker->next != NULL) {
								uri->pathHead = walker->next;
								walker->next->reserved = NULL;

								if (pathOwned && (walker->text.first != walker->text.afterLast)) {
									memory->free(memory, (URI_CHAR *)walker->text.first);
								}
								memory->free(memory, walker);
							} else {
								/* Re-use segment for "" path segment to represent trailing slash, update tail */
								URI_TYPE(PathSegment) * const segment = walker;
								if (pathOwned && (segment->text.first != segment->text.afterLast)) {
									memory->free(memory, (URI_CHAR *)segment->text.first);
								}
								segment->text.first = URI_FUNC(SafeToPointTo);
								segment->text.afterLast = URI_FUNC(SafeToPointTo);
								uri->pathHead = segment;
								uri->pathTail = segment;
							}

							if (pathOwned && (prev->text.first != prev->text.afterLast)) {
								memory->free(memory, (URI_CHAR *)prev->text.first);
							}
							memory->free(memory, prev);

							walker = nextBackup;
						}
					} else {
						URI_TYPE(PathSegment) * const anotherNextBackup = walker->next;
						/* First segment -> update head pointer */
						uri->pathHead = walker->next;
						if (walker->next != NULL) {
							walker->next->reserved = NULL;
						} else {
							/* Last segment -> update tail */
							uri->pathTail = NULL;
						}

						if (pathOwned && (walker->text.first != walker->text.afterLast)) {
							memory->free(memory, (URI_CHAR *)walker->text.first);
						}
						memory->free(memory, walker);

						walker = anotherNextBackup;
					}
				}
			}
			break;

		}

		if (!removeSegment) {
			if (walker->next != NULL) {
				walker->next->reserved = walker;
			} else {
				/* Last segment -> update tail */
				uri->pathTail = walker;
			}
			walker = walker->next;
		}
	} while (walker != NULL);

	return URI_TRUE;
}