TEST(UriSuite, TestUriUserInfoHostPort23Bug3510198Four) {
		// User info with ":", with port, with escaped chars in user name and password
		UriParserStateA stateA;
		UriUriA uriA;
		stateA.uri = &uriA;

		int res;
		//                           0   4  0  3  0                   20 01  0   4  01
		res = uriParseUriA(&stateA, "http" "://" "!$&'()*+,;=:password" "@" "host" "/");
		ASSERT_TRUE(URI_SUCCESS == res);
		ASSERT_TRUE(!memcmp(uriA.userInfo.first, "!$&'()*+,;=:password", 20 * sizeof(char)));
		ASSERT_TRUE(uriA.userInfo.afterLast - uriA.userInfo.first == 20);
		ASSERT_TRUE(!memcmp(uriA.hostText.first, "host", 4 * sizeof(char)));
		ASSERT_TRUE(uriA.hostText.afterLast - uriA.hostText.first == 4);
		ASSERT_TRUE(uriA.portText.first == NULL);
		ASSERT_TRUE(uriA.portText.afterLast == NULL);
		uriFreeUriMembersA(&uriA);
}