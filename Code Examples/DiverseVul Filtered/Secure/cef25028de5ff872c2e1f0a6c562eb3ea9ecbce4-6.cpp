TEST(UriSuite, TestUri) {
		UriParserStateA stateA;
		UriParserStateW stateW;
		UriUriA uriA;
		UriUriW uriW;

		stateA.uri = &uriA;
		stateW.uri = &uriW;

		// On/off for each
		ASSERT_TRUE(0 == uriParseUriA(&stateA, "//user:pass@[::1]:80/segment/index.html?query#frag"));
		uriFreeUriMembersA(&uriA);
		ASSERT_TRUE(0 == uriParseUriA(&stateA, "http://[::1]:80/segment/index.html?query#frag"));
		uriFreeUriMembersA(&uriA);
		ASSERT_TRUE(0 == uriParseUriA(&stateA, "http://user:pass@[::1]/segment/index.html?query#frag"));
		uriFreeUriMembersA(&uriA);
		ASSERT_TRUE(0 == uriParseUriA(&stateA, "http://user:pass@[::1]:80?query#frag"));
		uriFreeUriMembersA(&uriA);
		ASSERT_TRUE(0 == uriParseUriA(&stateA, "http://user:pass@[::1]:80/segment/index.html#frag"));
		uriFreeUriMembersA(&uriA);
		ASSERT_TRUE(0 == uriParseUriA(&stateA, "http://user:pass@[::1]:80/segment/index.html?query"));
		uriFreeUriMembersA(&uriA);

		// Schema, port, one segment
		ASSERT_TRUE(0 == uriParseUriA(&stateA, "ftp://host:21/gnu/"));
		uriFreeUriMembersA(&uriA);

		// Relative
		ASSERT_TRUE(0 == uriParseUriA(&stateA, "one/two/three"));
		ASSERT_TRUE(!uriA.absolutePath);
		uriFreeUriMembersA(&uriA);
		ASSERT_TRUE(0 == uriParseUriA(&stateA, "/one/two/three"));
		ASSERT_TRUE(uriA.absolutePath);
		uriFreeUriMembersA(&uriA);
		ASSERT_TRUE(0 == uriParseUriA(&stateA, "//user:pass@localhost/one/two/three"));
		uriFreeUriMembersA(&uriA);

		// ANSI and Unicode
		ASSERT_TRUE(0 == uriParseUriA(&stateA, "http://www.example.com/"));
		uriFreeUriMembersA(&uriA);
		ASSERT_TRUE(0 == uriParseUriW(&stateW, L"http://www.example.com/"));
		uriFreeUriMembersW(&uriW);

		// Real life examples
		ASSERT_TRUE(0 == uriParseUriA(&stateA, "http://sourceforge.net/projects/uriparser/"));
		uriFreeUriMembersA(&uriA);
		ASSERT_TRUE(0 == uriParseUriA(&stateA, "http://sourceforge.net/project/platformdownload.php?group_id=182840"));
		uriFreeUriMembersA(&uriA);
		ASSERT_TRUE(0 == uriParseUriA(&stateA, "mailto:test@example.com"));
		uriFreeUriMembersA(&uriA);
		ASSERT_TRUE(0 == uriParseUriA(&stateA, "../../"));
		uriFreeUriMembersA(&uriA);
		ASSERT_TRUE(0 == uriParseUriA(&stateA, "/"));
		ASSERT_TRUE(uriA.absolutePath);
		uriFreeUriMembersA(&uriA);
		ASSERT_TRUE(0 == uriParseUriA(&stateA, ""));
		ASSERT_TRUE(!uriA.absolutePath);
		uriFreeUriMembersA(&uriA);
		ASSERT_TRUE(0 == uriParseUriA(&stateA, "file:///bin/bash"));
		uriFreeUriMembersA(&uriA);

		// Percent encoding
		ASSERT_TRUE(0 == uriParseUriA(&stateA, "http://www.example.com/name%20with%20spaces/"));
		uriFreeUriMembersA(&uriA);
		ASSERT_TRUE(0 != uriParseUriA(&stateA, "http://www.example.com/name with spaces/"));
		uriFreeUriMembersA(&uriA);
}