TEST(UriSuite, TestUriHostIpFour1) {
		UriParserStateA stateA;
		UriUriA uriA;
		stateA.uri = &uriA;
		//                          0   4  0  3  0      7  01  0 2
		const char * const input = "http" "://" "1.2.3.4" ":" "80";
		ASSERT_TRUE(0 == uriParseUriA(&stateA, input));

		ASSERT_TRUE(uriA.hostText.first == input + 4 + 3);
		ASSERT_TRUE(uriA.hostText.afterLast == input + 4 + 3 + 7);
		ASSERT_TRUE(uriA.hostData.ip4 != NULL);
		ASSERT_TRUE(uriA.hostData.ip6 == NULL);
		ASSERT_TRUE(uriA.hostData.ipFuture.first == NULL);
		ASSERT_TRUE(uriA.hostData.ipFuture.afterLast == NULL);
		uriFreeUriMembersA(&uriA);
}