TEST(UriSuite, TestNormalizeCrashBug20080224) {
		UriParserStateW stateW;
		int res;
		UriUriW testUri;
		stateW.uri = &testUri;

		res = uriParseUriW(&stateW, L"http://example.org/abc//../def");
		ASSERT_TRUE(res == 0);

		// First call will make us owner of copied memory
		res = uriNormalizeSyntaxExW(&testUri, URI_NORMALIZE_SCHEME);
		ASSERT_TRUE(res == 0);
		res = uriNormalizeSyntaxExW(&testUri, URI_NORMALIZE_HOST);
		ASSERT_TRUE(res == 0);

		// Frees empty path segment -> crash
		res = uriNormalizeSyntaxW(&testUri);
		ASSERT_TRUE(res == 0);

		uriFreeUriMembersW(&testUri);
}