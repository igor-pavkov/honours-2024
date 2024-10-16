TEST(UriSuite, TestUriUserInfoHostPort23Bug3510198RelatedOneTwo) {
		// Empty user info
		UriParserStateA stateA;
		UriUriA uriA;
		stateA.uri = &uriA;

		int res;
		//                           0   4  0  3  0      7  01
		res = uriParseUriA(&stateA, "http" "://" "%2Fhost" "/");
		ASSERT_TRUE(URI_SUCCESS == res);
		ASSERT_TRUE(uriA.userInfo.afterLast == NULL);
		ASSERT_TRUE(uriA.userInfo.first == NULL);
		ASSERT_TRUE(!memcmp(uriA.hostText.first, "%2Fhost", 7 * sizeof(char)));
		ASSERT_TRUE(uriA.hostText.afterLast - uriA.hostText.first == 7);
		ASSERT_TRUE(uriA.portText.first == NULL);
		ASSERT_TRUE(uriA.portText.afterLast == NULL);
		uriFreeUriMembersA(&uriA);
}