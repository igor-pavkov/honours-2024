TEST(UriSuite, TestUriUserInfoHostPort23Bug3510198Two) {
		// User info with ":", with port, with escaped chars in user name and password
		UriParserStateA stateA;
		UriUriA uriA;
		stateA.uri = &uriA;

		int res;
		//                           0   4  0  3  0            13 01  0   4  01
		res = uriParseUriA(&stateA, "http" "://" "%2Fuser:%2F21" "@" "host" "/");
		ASSERT_TRUE(URI_SUCCESS == res);
		ASSERT_TRUE(!memcmp(uriA.userInfo.first, "%2Fuser:%2F21", 13 * sizeof(char)));
		ASSERT_TRUE(uriA.userInfo.afterLast - uriA.userInfo.first == 13);
		ASSERT_TRUE(!memcmp(uriA.hostText.first, "host", 4 * sizeof(char)));
		ASSERT_TRUE(uriA.hostText.afterLast - uriA.hostText.first == 4);
		ASSERT_TRUE(uriA.portText.first == NULL);
		ASSERT_TRUE(uriA.portText.afterLast == NULL);
		uriFreeUriMembersA(&uriA);
}