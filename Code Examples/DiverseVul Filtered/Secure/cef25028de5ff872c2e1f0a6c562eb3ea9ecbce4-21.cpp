static const URI_CHAR * URI_FUNC(ParseIPv6address2)(
		URI_TYPE(ParserState) * state,
		const URI_CHAR * first, const URI_CHAR * afterLast,
		UriMemoryManager * memory) {
	int zipperEver = 0;
	int quadsDone = 0;
	int digitCount = 0;
	unsigned char digitHistory[4];
	int ip4OctetsDone = 0;

	unsigned char quadsAfterZipper[14];
	int quadsAfterZipperCount = 0;


	for (;;) {
		if (first >= afterLast) {
			URI_FUNC(StopSyntax)(state, first, memory);
			return NULL;
		}

		/* Inside IPv4 part? */
		if (ip4OctetsDone > 0) {
			/* Eat rest of IPv4 address */
			for (;;) {
				switch (*first) {
				case URI_SET_DIGIT:
					if (digitCount == 4) {
						URI_FUNC(StopSyntax)(state, first, memory);
						return NULL;
					}
					digitHistory[digitCount++] = (unsigned char)(9 + *first - _UT('9'));
					break;

				case _UT('.'):
					if ((ip4OctetsDone == 4) /* NOTE! */
							|| (digitCount == 0)
							|| (digitCount == 4)) {
						/* Invalid digit or octet count */
						URI_FUNC(StopSyntax)(state, first, memory);
						return NULL;
					} else if ((digitCount > 1)
							&& (digitHistory[0] == 0)) {
						/* Leading zero */
						URI_FUNC(StopSyntax)(state, first - digitCount, memory);
						return NULL;
					} else if ((digitCount > 2)
							&& (digitHistory[1] == 0)) {
						/* Leading zero */
						URI_FUNC(StopSyntax)(state, first - digitCount + 1, memory);
						return NULL;
					} else if ((digitCount == 3)
							&& (100 * digitHistory[0]
								+ 10 * digitHistory[1]
								+ digitHistory[2] > 255)) {
						/* Octet value too large */
						if (digitHistory[0] > 2) {
							URI_FUNC(StopSyntax)(state, first - 3, memory);
						} else if (digitHistory[1] > 5) {
							URI_FUNC(StopSyntax)(state, first - 2, memory);
						} else {
							URI_FUNC(StopSyntax)(state, first - 1, memory);
						}
						return NULL;
					}

					/* Copy IPv4 octet */
					state->uri->hostData.ip6->data[16 - 4 + ip4OctetsDone] = uriGetOctetValue(digitHistory, digitCount);
					digitCount = 0;
					ip4OctetsDone++;
					break;

				case _UT(']'):
					if ((ip4OctetsDone != 3) /* NOTE! */
							|| (digitCount == 0)
							|| (digitCount == 4)) {
						/* Invalid digit or octet count */
						URI_FUNC(StopSyntax)(state, first, memory);
						return NULL;
					} else if ((digitCount > 1)
							&& (digitHistory[0] == 0)) {
						/* Leading zero */
						URI_FUNC(StopSyntax)(state, first - digitCount, memory);
						return NULL;
					} else if ((digitCount > 2)
							&& (digitHistory[1] == 0)) {
						/* Leading zero */
						URI_FUNC(StopSyntax)(state, first - digitCount + 1, memory);
						return NULL;
					} else if ((digitCount == 3)
							&& (100 * digitHistory[0]
								+ 10 * digitHistory[1]
								+ digitHistory[2] > 255)) {
						/* Octet value too large */
						if (digitHistory[0] > 2) {
							URI_FUNC(StopSyntax)(state, first - 3, memory);
						} else if (digitHistory[1] > 5) {
							URI_FUNC(StopSyntax)(state, first - 2, memory);
						} else {
							URI_FUNC(StopSyntax)(state, first - 1, memory);
						}
						return NULL;
					}

					state->uri->hostText.afterLast = first; /* HOST END */

					/* Copy missing quads right before IPv4 */
					memcpy(state->uri->hostData.ip6->data + 16 - 4 - 2 * quadsAfterZipperCount,
								quadsAfterZipper, 2 * quadsAfterZipperCount);

					/* Copy last IPv4 octet */
					state->uri->hostData.ip6->data[16 - 4 + 3] = uriGetOctetValue(digitHistory, digitCount);

					return first + 1;

				default:
					URI_FUNC(StopSyntax)(state, first, memory);
					return NULL;
				}
				first++;

				if (first >= afterLast) {
					URI_FUNC(StopSyntax)(state, first, memory);
					return NULL;
				}
			}
		} else {
			/* Eat while no dot in sight */
			int letterAmong = 0;
			int walking = 1;
			do {
				switch (*first) {
				case URI_SET_HEX_LETTER_LOWER:
					letterAmong = 1;
					if (digitCount == 4) {
						URI_FUNC(StopSyntax)(state, first, memory);
						return NULL;
					}
					digitHistory[digitCount] = (unsigned char)(15 + *first - _UT('f'));
					digitCount++;
					break;

				case URI_SET_HEX_LETTER_UPPER:
					letterAmong = 1;
					if (digitCount == 4) {
						URI_FUNC(StopSyntax)(state, first, memory);
						return NULL;
					}
					digitHistory[digitCount] = (unsigned char)(15 + *first - _UT('F'));
					digitCount++;
					break;

				case URI_SET_DIGIT:
					if (digitCount == 4) {
						URI_FUNC(StopSyntax)(state, first, memory);
						return NULL;
					}
					digitHistory[digitCount] = (unsigned char)(9 + *first - _UT('9'));
					digitCount++;
					break;

				case _UT(':'):
					{
						int setZipper = 0;

						if (digitCount > 0) {
							if (zipperEver) {
								uriWriteQuadToDoubleByte(digitHistory, digitCount, quadsAfterZipper + 2 * quadsAfterZipperCount);
								quadsAfterZipperCount++;
							} else {
								uriWriteQuadToDoubleByte(digitHistory, digitCount, state->uri->hostData.ip6->data + 2 * quadsDone);
							}
							quadsDone++;
							digitCount = 0;
						}
						letterAmong = 0;

						/* Too many quads? */
						if (quadsDone >= 8 - zipperEver) {
							URI_FUNC(StopSyntax)(state, first, memory);
							return NULL;
						}

						/* "::"? */
						if (first + 1 >= afterLast) {
							URI_FUNC(StopSyntax)(state, first + 1, memory);
							return NULL;
						}
						if (first[1] == _UT(':')) {
							const int resetOffset = 2 * (quadsDone + (digitCount > 0));

							first++;
							if (zipperEver) {
								URI_FUNC(StopSyntax)(state, first, memory);
								return NULL; /* "::.+::" */
							}

							/* Zero everything after zipper */
							memset(state->uri->hostData.ip6->data + resetOffset, 0, 16 - resetOffset);
							setZipper = 1;

							/* ":::+"? */
							if (first + 1 >= afterLast) {
								URI_FUNC(StopSyntax)(state, first + 1, memory);
								return NULL; /* No ']' yet */
							}
							if (first[1] == _UT(':')) {
								URI_FUNC(StopSyntax)(state, first + 1, memory);
								return NULL; /* ":::+ "*/
							}
						}

						if (setZipper) {
							zipperEver = 1;
						}
					}
					break;

				case _UT('.'):
					if ((quadsDone > 6) /* NOTE */
							|| (!zipperEver && (quadsDone < 6))
							|| letterAmong
							|| (digitCount == 0)
							|| (digitCount == 4)) {
						/* Invalid octet before */
						URI_FUNC(StopSyntax)(state, first, memory);
						return NULL;
					} else if ((digitCount > 1)
							&& (digitHistory[0] == 0)) {
						/* Leading zero */
						URI_FUNC(StopSyntax)(state, first - digitCount, memory);
						return NULL;
					} else if ((digitCount > 2)
							&& (digitHistory[1] == 0)) {
						/* Leading zero */
						URI_FUNC(StopSyntax)(state, first - digitCount + 1, memory);
						return NULL;
					} else if ((digitCount == 3)
							&& (100 * digitHistory[0]
								+ 10 * digitHistory[1]
								+ digitHistory[2] > 255)) {
						/* Octet value too large */
						if (digitHistory[0] > 2) {
							URI_FUNC(StopSyntax)(state, first - 3, memory);
						} else if (digitHistory[1] > 5) {
							URI_FUNC(StopSyntax)(state, first - 2, memory);
						} else {
							URI_FUNC(StopSyntax)(state, first - 1, memory);
						}
						return NULL;
					}

					/* Copy first IPv4 octet */
					state->uri->hostData.ip6->data[16 - 4] = uriGetOctetValue(digitHistory, digitCount);
					digitCount = 0;

					/* Switch over to IPv4 loop */
					ip4OctetsDone = 1;
					walking = 0;
					break;

				case _UT(']'):
					/* Too little quads? */
					if (!zipperEver && !((quadsDone == 7) && (digitCount > 0))) {
						URI_FUNC(StopSyntax)(state, first, memory);
						return NULL;
					}

					if (digitCount > 0) {
						if (zipperEver) {
							uriWriteQuadToDoubleByte(digitHistory, digitCount, quadsAfterZipper + 2 * quadsAfterZipperCount);
							quadsAfterZipperCount++;
						} else {
							uriWriteQuadToDoubleByte(digitHistory, digitCount, state->uri->hostData.ip6->data + 2 * quadsDone);
						}
						/*
						quadsDone++;
						digitCount = 0;
						*/
					}

					/* Copy missing quads to the end */
					memcpy(state->uri->hostData.ip6->data + 16 - 2 * quadsAfterZipperCount,
								quadsAfterZipper, 2 * quadsAfterZipperCount);

					state->uri->hostText.afterLast = first; /* HOST END */
					return first + 1; /* Fine */

				default:
					URI_FUNC(StopSyntax)(state, first, memory);
					return NULL;
				}
				first++;

				if (first >= afterLast) {
					URI_FUNC(StopSyntax)(state, first, memory);
					return NULL; /* No ']' yet */
				}
			} while (walking);
		}
	}
}