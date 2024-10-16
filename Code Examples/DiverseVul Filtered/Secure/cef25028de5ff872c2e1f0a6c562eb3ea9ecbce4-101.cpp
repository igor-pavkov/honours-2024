TEST(UriSuite, TestUriUserInfoHostPort6) {
		// No user info, with port
		UriParserStateA stateA;
		UriUriA uriA;
		stateA.uri = &uriA;
		//                          0   4  0  3  0        9  01  0  3
		const char * const input = "http" "://" "localhost" ":" "123";
		ASSERT_TRUE(0 == uriParseUriA(&stateA, input));

		ASSERT_TRUE(uriA.userInfo.first == NULL);
		ASSERT_TRUE(uriA.userInfo.afterLast == NULL);
		ASSERT_TRUE(uriA.hostText.first == input + 4 + 3);
		ASSERT_TRUE(uriA.hostText.afterLast == input + 4 + 3 + 9);
		ASSERT_TRUE(uriA.portText.first == input + 4 + 3 + 9 + 1);
		ASSERT_TRUE(uriA.portText.afterLast == input + 4 + 3 + 9 + 1 + 3);
		uriFreeUriMembersA(&uriA);
}