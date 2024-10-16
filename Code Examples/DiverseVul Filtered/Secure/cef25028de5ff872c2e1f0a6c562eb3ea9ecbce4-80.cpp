	void testQueryListPairHelper(const char * pair, const char * unescapedKey,
			const char * unescapedValue, const char * fixed = NULL) {
		int res;
		UriQueryListA * queryList;
		int itemCount;

		res = uriDissectQueryMallocA(&queryList, &itemCount, pair, pair + strlen(pair));
		ASSERT_TRUE(res == URI_SUCCESS);
		ASSERT_TRUE(queryList != NULL);
		ASSERT_TRUE(itemCount == 1);
		ASSERT_TRUE(!strcmp(queryList->key, unescapedKey));
		ASSERT_TRUE(!strcmp(queryList->value, unescapedValue));

		char * recomposed;
		res = uriComposeQueryMallocA(&recomposed, queryList);
		ASSERT_TRUE(res == URI_SUCCESS);
		ASSERT_TRUE(recomposed != NULL);
		ASSERT_TRUE(!strcmp(recomposed, (fixed != NULL) ? fixed : pair));
		free(recomposed);
		uriFreeQueryListA(queryList);
	}