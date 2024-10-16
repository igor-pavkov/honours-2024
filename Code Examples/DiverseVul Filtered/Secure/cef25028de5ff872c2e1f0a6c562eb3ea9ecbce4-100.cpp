TEST(UriSuite, TestUriUserInfoHostPort5) {
		// No user info, no port
		UriParserStateA stateA;
		UriUriA uriA;
		stateA.uri = &uriA;
		//                          0   4  0  3  0        9
		const char * const input = "http" "://" "localhost";
		ASSERT_TRUE(0 == uriParseUriA(&stateA, input));

		ASSERT_TRUE(uriA.userInfo.first == NULL);
		ASSERT_TRUE(uriA.userInfo.afterLast == NULL);
		ASSERT_TRUE(uriA.hostText.first == input + 4 + 3);
		ASSERT_TRUE(uriA.hostText.afterLast == input + 4 + 3 + 9);
		ASSERT_TRUE(uriA.portText.first == NULL);
		ASSERT_TRUE(uriA.portText.afterLast == NULL);
		uriFreeUriMembersA(&uriA);
}