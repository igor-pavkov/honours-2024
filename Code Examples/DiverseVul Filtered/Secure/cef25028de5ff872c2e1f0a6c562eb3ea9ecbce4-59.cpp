TEST(UriSuite, TestTrailingSlash) {
		UriParserStateA stateA;
		UriUriA uriA;
		stateA.uri = &uriA;
		//                          0  3  01
		const char * const input = "abc" "/";
		ASSERT_TRUE(0 == uriParseUriA(&stateA, input));

		ASSERT_TRUE(uriA.pathHead->text.first == input);
		ASSERT_TRUE(uriA.pathHead->text.afterLast == input + 3);
		ASSERT_TRUE(uriA.pathHead->next->text.first == uriA.pathHead->next->text.afterLast);
		ASSERT_TRUE(uriA.pathHead->next->next == NULL);
		ASSERT_TRUE(uriA.pathTail == uriA.pathHead->next);
		uriFreeUriMembersA(&uriA);
}