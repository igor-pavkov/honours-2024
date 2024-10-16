TEST(UriSuite, TestUriUserInfoHostPort23Bug3510198RelatedOne) {
		// Empty user info
		UriParserStateA stateA;
		UriUriA uriA;
		stateA.uri = &uriA;

		int res;
		//                           0   4  0  3  01  0   4  01
		res = uriParseUriA(&stateA, "http" "://" "@" "host" "/");
		ASSERT_TRUE(URI_SUCCESS == res);
		ASSERT_TRUE(uriA.userInfo.afterLast != NULL);
		ASSERT_TRUE(uriA.userInfo.first != NULL);
		ASSERT_TRUE(uriA.userInfo.afterLast - uriA.userInfo.first == 0);
		ASSERT_TRUE(!memcmp(uriA.hostText.first, "host", 4 * sizeof(char)));
		ASSERT_TRUE(uriA.hostText.afterLast - uriA.hostText.first == 4);
		ASSERT_TRUE(uriA.portText.first == NULL);
		ASSERT_TRUE(uriA.portText.afterLast == NULL);
		uriFreeUriMembersA(&uriA);
}