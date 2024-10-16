TEST(UriSuite, TestUriHostEmpty) {
		UriParserStateA stateA;
		UriUriA uriA;
		stateA.uri = &uriA;
		//                          0   4  0  3  01  0  3
		const char * const input = "http" "://" ":" "123";
		const int res = uriParseUriA(&stateA, input);
		ASSERT_TRUE(URI_SUCCESS == res);
		ASSERT_TRUE(uriA.userInfo.first == NULL);
		ASSERT_TRUE(uriA.userInfo.afterLast == NULL);
		ASSERT_TRUE(uriA.hostText.first != NULL);
		ASSERT_TRUE(uriA.hostText.afterLast != NULL);
		ASSERT_TRUE(uriA.hostText.afterLast - uriA.hostText.first == 0);
		ASSERT_TRUE(uriA.portText.first == input + 4 + 3 + 1);
		ASSERT_TRUE(uriA.portText.afterLast == input + 4 + 3 + 1 + 3);
		uriFreeUriMembersA(&uriA);
}