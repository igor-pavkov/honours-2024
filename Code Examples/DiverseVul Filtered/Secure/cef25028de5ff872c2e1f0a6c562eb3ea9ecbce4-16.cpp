TEST(UriSuite, TestUriHostIpSix2) {
		UriParserStateA stateA;
		UriUriA uriA;
		stateA.uri = &uriA;
		//                          0   4  0  3  01  45
		const char * const input = "http" "://" "[::1]";
		ASSERT_TRUE(0 == uriParseUriA(&stateA, input));

		ASSERT_TRUE(uriA.hostText.first == input + 4 + 3 + 1);
		ASSERT_TRUE(uriA.hostText.afterLast == input + 4 + 3 + 4);
		ASSERT_TRUE(uriA.hostData.ip4 == NULL);
		ASSERT_TRUE(uriA.hostData.ip6 != NULL);
		ASSERT_TRUE(uriA.hostData.ipFuture.first == NULL);
		ASSERT_TRUE(uriA.hostData.ipFuture.afterLast == NULL);
		uriFreeUriMembersA(&uriA);
}