	void testRemoveBaseUriHelper(const char * expected,
								const char * absSourceStr,
								const char * absBaseStr) {
		UriParserStateA state;
		UriUriA absSource;
		UriUriA absBase;
		UriUriA dest;

		state.uri = &absSource;
		ASSERT_TRUE(uriParseUriA(&state, absSourceStr) == URI_SUCCESS);

		state.uri = &absBase;
		ASSERT_TRUE(uriParseUriA(&state, absBaseStr) == URI_SUCCESS);

		ASSERT_TRUE(uriRemoveBaseUriA(&dest, &absSource, &absBase, URI_FALSE)
				== URI_SUCCESS);

		int size = 0;
		ASSERT_TRUE(uriToStringCharsRequiredA(&dest, &size) == URI_SUCCESS);
		char * const buffer = (char *)malloc(size + 1);
		ASSERT_TRUE(buffer);
		ASSERT_TRUE(uriToStringA(buffer, &dest, size + 1, &size)
															== URI_SUCCESS);
		if (strcmp(buffer, expected)) {
			printf("Expected \"%s\" but got \"%s\"\n", expected, buffer);
			ASSERT_TRUE(0);
		}
		free(buffer);

		uriFreeUriMembersA(&absSource);
		uriFreeUriMembersA(&absBase);
		uriFreeUriMembersA(&dest);
	}