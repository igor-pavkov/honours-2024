TEST(UriSuite, TestUriUserInfoHostPort22Bug1948038) {
		UriParserStateA stateA;
		UriUriA uriA;
		stateA.uri = &uriA;

		int res;

		res = uriParseUriA(&stateA, "http://user:21@host/");
		ASSERT_TRUE(URI_SUCCESS == res);
		ASSERT_TRUE(!memcmp(uriA.userInfo.first, "user:21", 7 * sizeof(char)));
		ASSERT_TRUE(uriA.userInfo.afterLast - uriA.userInfo.first == 7);
		ASSERT_TRUE(!memcmp(uriA.hostText.first, "host", 4 * sizeof(char)));
		ASSERT_TRUE(uriA.hostText.afterLast - uriA.hostText.first == 4);
		ASSERT_TRUE(uriA.portText.first == NULL);
		ASSERT_TRUE(uriA.portText.afterLast == NULL);
		uriFreeUriMembersA(&uriA);

		res = uriParseUriA(&stateA, "http://user:1234@192.168.0.1:1234/foo.com");
		ASSERT_TRUE(URI_SUCCESS == res);
		uriFreeUriMembersA(&uriA);

		res = uriParseUriA(&stateA, "http://moo:21@moo:21@moo/");
		ASSERT_TRUE(URI_ERROR_SYNTAX == res);
		uriFreeUriMembersA(&uriA);

		res = uriParseUriA(&stateA, "http://moo:21@moo:21@moo:21/");
		ASSERT_TRUE(URI_ERROR_SYNTAX == res);
		uriFreeUriMembersA(&uriA);
}