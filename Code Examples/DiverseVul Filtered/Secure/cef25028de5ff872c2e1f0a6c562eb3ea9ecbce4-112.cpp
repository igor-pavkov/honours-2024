TEST(UriSuite, TestToStringBug1950126) {
		UriParserStateW state;
		UriUriW uriOne;
		UriUriW uriTwo;
		const wchar_t * const uriOneString = L"http://e.com/";
		const wchar_t * const uriTwoString = L"http://e.com";
		state.uri = &uriOne;
		ASSERT_TRUE(URI_SUCCESS == uriParseUriW(&state, uriOneString));
		state.uri = &uriTwo;
		ASSERT_TRUE(URI_SUCCESS == uriParseUriW(&state, uriTwoString));
		ASSERT_TRUE(URI_FALSE == uriEqualsUriW(&uriOne, &uriTwo));
		uriFreeUriMembersW(&uriOne);
		uriFreeUriMembersW(&uriTwo);

		ASSERT_TRUE(testToStringHelper(uriOneString));
		ASSERT_TRUE(testToStringHelper(uriTwoString));
}