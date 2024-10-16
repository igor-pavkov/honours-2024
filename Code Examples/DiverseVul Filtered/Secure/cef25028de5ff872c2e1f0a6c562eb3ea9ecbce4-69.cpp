TEST(UriSuite, TestUriUserInfoHostPort2) {
		// User info with ":", with port
		UriParserStateA stateA;
		UriUriA uriA;
		stateA.uri = &uriA;
		//                          0   4  0  3  0      7  01  0        9
		const char * const input = "http" "://" "abc:def" "@" "localhost"
		//		01   0  3
				":" "123";
		ASSERT_TRUE(0 == uriParseUriA(&stateA, input));

		ASSERT_TRUE(uriA.userInfo.first == input + 4 + 3);
		ASSERT_TRUE(uriA.userInfo.afterLast == input + 4 + 3 + 7);
		ASSERT_TRUE(uriA.hostText.first == input + 4 + 3 + 7 + 1);
		ASSERT_TRUE(uriA.hostText.afterLast == input + 4 + 3 + 7 + 1 + 9);
		ASSERT_TRUE(uriA.portText.first == input + 4 + 3 + 7 + 1 + 9 + 1);
		ASSERT_TRUE(uriA.portText.afterLast == input + 4 + 3 + 7 + 1 + 9 + 1 + 3);
		uriFreeUriMembersA(&uriA);
}