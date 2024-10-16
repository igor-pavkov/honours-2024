TEST(UriSuite, TestCrashMakeOwnerBug20080207) {
		// Testcase by Adrian Manrique
		UriParserStateA state;
		UriUriA sourceUri;
		state.uri = &sourceUri;
		const char * const sourceUriString = "http://user:pass@somehost.com:80/";
		if (uriParseUriA(&state, sourceUriString) != 0) {
			ASSERT_TRUE(false);
		}
		if (uriNormalizeSyntaxA(&sourceUri) != 0) {
			ASSERT_TRUE(false);
		}
		uriFreeUriMembersA(&sourceUri);
		ASSERT_TRUE(true);
}