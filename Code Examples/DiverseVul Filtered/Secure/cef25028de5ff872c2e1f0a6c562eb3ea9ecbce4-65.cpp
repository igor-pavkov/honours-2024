TEST(UriSuite, TestUriComponents) {
		UriParserStateA stateA;
		UriUriA uriA;
		stateA.uri = &uriA;
		//                          0   4  0  3  0              15 01  0      7  01
		const char * const input = "http" "://" "sourceforge.net" "/" "project" "/"
		//		 0                   20 01  0              15
				"platformdownload.php" "?" "group_id=182840";
		ASSERT_TRUE(0 == uriParseUriA(&stateA, input));

		ASSERT_TRUE(uriA.scheme.first == input);
		ASSERT_TRUE(uriA.scheme.afterLast == input + 4);
		ASSERT_TRUE(uriA.userInfo.first == NULL);
		ASSERT_TRUE(uriA.userInfo.afterLast == NULL);
		ASSERT_TRUE(uriA.hostText.first == input + 4 + 3);
		ASSERT_TRUE(uriA.hostText.afterLast == input + 4 + 3 + 15);
		ASSERT_TRUE(uriA.hostData.ipFuture.first == NULL);
		ASSERT_TRUE(uriA.hostData.ipFuture.afterLast == NULL);
		ASSERT_TRUE(uriA.portText.first == NULL);
		ASSERT_TRUE(uriA.portText.afterLast == NULL);

		ASSERT_TRUE(uriA.pathHead->text.first == input + 4 + 3 + 15 + 1);
		ASSERT_TRUE(uriA.pathHead->text.afterLast == input + 4 + 3 + 15 + 1 + 7);
		ASSERT_TRUE(uriA.pathHead->next->text.first == input + 4 + 3 + 15 + 1 + 7 + 1);
		ASSERT_TRUE(uriA.pathHead->next->text.afterLast == input + 4 + 3 + 15 + 1 + 7 + 1 + 20);
		ASSERT_TRUE(uriA.pathHead->next->next == NULL);
		ASSERT_TRUE(uriA.pathTail == uriA.pathHead->next);

		ASSERT_TRUE(uriA.query.first == input + 4 + 3 + 15 + 1 + 7 + 1 + 20 + 1);
		ASSERT_TRUE(uriA.query.afterLast == input + 4 + 3 + 15 + 1 + 7 + 1 + 20 + 1 + 15);
		ASSERT_TRUE(uriA.fragment.first == NULL);
		ASSERT_TRUE(uriA.fragment.afterLast == NULL);
		uriFreeUriMembersA(&uriA);
}