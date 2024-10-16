	void testCompareRangeHelper(const char * a, const char * b, int expected, bool avoidNullRange = true) {
		UriTextRangeA ra;
		UriTextRangeA rb;

		if (a) {
			ra.first = a;
			ra.afterLast = a + strlen(a);
		} else {
			ra.first = NULL;
			ra.afterLast = NULL;
		}

		if (b) {
			rb.first = b;
			rb.afterLast = b + strlen(b);
		} else {
			rb.first = NULL;
			rb.afterLast = NULL;
		}

		const int received = uriCompareRangeA(
				((a == NULL) && avoidNullRange) ? NULL : &ra,
				((b == NULL) && avoidNullRange) ? NULL : &rb);
		if (received != expected) {
			printf("Comparing <%s> to <%s> yields %d, expected %d.\n",
					a, b, received, expected);
		}
		ASSERT_TRUE(received == expected);
	}