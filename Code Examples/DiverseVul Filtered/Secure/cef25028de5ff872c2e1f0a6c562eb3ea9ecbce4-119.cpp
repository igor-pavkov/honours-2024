	void helperTestQueryString(char const * uriString, int pairsExpected) {
		UriParserStateA state;
		UriUriA uri;
		state.uri = &uri;
		int res = uriParseUriA(&state, uriString);
		ASSERT_TRUE(res == URI_SUCCESS);

		UriQueryListA * queryList = NULL;
		int itemCount = 0;

		res = uriDissectQueryMallocA(&queryList, &itemCount,
				uri.query.first, uri.query.afterLast);
		ASSERT_TRUE(res == URI_SUCCESS);
		ASSERT_TRUE(queryList != NULL);
		ASSERT_TRUE(itemCount == pairsExpected);
		uriFreeQueryListA(queryList);
		uriFreeUriMembersA(&uri);
	}