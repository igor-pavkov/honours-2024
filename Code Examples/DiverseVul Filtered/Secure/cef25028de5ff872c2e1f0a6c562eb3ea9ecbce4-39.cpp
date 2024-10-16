	void testEqualsHelper(const char * uri_to_test) {
		UriParserStateA state;
		UriUriA uriOne;
		UriUriA uriTwo;
		state.uri = &uriOne;
		ASSERT_TRUE(URI_SUCCESS == uriParseUriA(&state, uri_to_test));
		state.uri = &uriTwo;
		ASSERT_TRUE(URI_SUCCESS == uriParseUriA(&state, uri_to_test));
		ASSERT_TRUE(URI_TRUE == uriEqualsUriA(&uriOne, &uriTwo));
		uriFreeUriMembersA(&uriOne);
		uriFreeUriMembersA(&uriTwo);
	}