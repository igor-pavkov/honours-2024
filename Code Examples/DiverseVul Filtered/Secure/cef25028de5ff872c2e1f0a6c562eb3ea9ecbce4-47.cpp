TEST(UriSuite, TestUriComponentsBug20070701) {
		UriParserStateA stateA;
		UriUriA uriA;
		stateA.uri = &uriA;
		//                          01  01  01
		const char * const input = "a" ":" "b";
		ASSERT_TRUE(0 == uriParseUriA(&stateA, input));

		ASSERT_TRUE(uriA.scheme.first == input);
		ASSERT_TRUE(uriA.scheme.afterLast == input + 1);
		ASSERT_TRUE(uriA.userInfo.first == NULL);
		ASSERT_TRUE(uriA.userInfo.afterLast == NULL);
		ASSERT_TRUE(uriA.hostText.first == NULL);
		ASSERT_TRUE(uriA.hostText.afterLast == NULL);
		ASSERT_TRUE(uriA.hostData.ipFuture.first == NULL);
		ASSERT_TRUE(uriA.hostData.ipFuture.afterLast == NULL);
		ASSERT_TRUE(uriA.portText.first == NULL);
		ASSERT_TRUE(uriA.portText.afterLast == NULL);

		ASSERT_TRUE(uriA.pathHead->text.first == input + 1 + 1);
		ASSERT_TRUE(uriA.pathHead->text.afterLast == input + 1 + 1 + 1);
		ASSERT_TRUE(uriA.pathHead->next == NULL);
		ASSERT_TRUE(uriA.pathTail == uriA.pathHead);

		ASSERT_TRUE(uriA.query.first == NULL);
		ASSERT_TRUE(uriA.query.afterLast == NULL);
		ASSERT_TRUE(uriA.fragment.first == NULL);
		ASSERT_TRUE(uriA.fragment.afterLast == NULL);

		ASSERT_TRUE(!uriA.absolutePath);
		uriFreeUriMembersA(&uriA);
}