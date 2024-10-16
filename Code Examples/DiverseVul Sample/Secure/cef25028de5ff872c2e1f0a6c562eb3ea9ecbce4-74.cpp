TEST(UriSuite, TestInvalidInputBug16) {
		UriParserStateA stateA;
		UriUriA uriA;
		stateA.uri = &uriA;
		const char * const input = "A>B";

		const int res = uriParseUriA(&stateA, input);

		ASSERT_TRUE(res == URI_ERROR_SYNTAX);
		ASSERT_TRUE(stateA.errorPos == input + 1);
		ASSERT_TRUE(stateA.errorCode == URI_ERROR_SYNTAX);  /* failed previously */

		uriFreeUriMembersA(&uriA);
}