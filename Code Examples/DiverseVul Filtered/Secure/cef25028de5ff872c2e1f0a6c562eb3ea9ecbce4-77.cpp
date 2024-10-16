TEST(UriSuite, TestCrashFreeUriMembersBug20080116) {
		// Testcase by Adrian Manrique
		UriParserStateA state;
		UriUriA uri;
		state.uri = &uri;
		uriParseUriA(&state, "http://test/?");
		uriNormalizeSyntaxA(&uri);
		uriFreeUriMembersA(&uri);

		ASSERT_TRUE(true);
}