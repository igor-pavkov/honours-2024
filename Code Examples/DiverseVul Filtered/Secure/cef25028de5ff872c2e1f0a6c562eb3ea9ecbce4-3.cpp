TEST(UriSuite, TestQueryDissectionBug3590761) {
		int res;
		UriQueryListA * queryList;
		int itemCount;
		const char * const pair = "q=hello&x=&y=";

		res = uriDissectQueryMallocA(&queryList, &itemCount, pair, pair + strlen(pair));
		ASSERT_TRUE(res == URI_SUCCESS);
		ASSERT_TRUE(queryList != NULL);
		ASSERT_TRUE(itemCount == 3);

		ASSERT_TRUE(!strcmp(queryList->key, "q"));
		ASSERT_TRUE(!strcmp(queryList->value, "hello"));

		ASSERT_TRUE(!strcmp(queryList->next->key, "x"));
		ASSERT_TRUE(!strcmp(queryList->next->value, ""));

		ASSERT_TRUE(!strcmp(queryList->next->next->key, "y"));
		ASSERT_TRUE(!strcmp(queryList->next->next->value, ""));

		ASSERT_TRUE(! queryList->next->next->next);

		uriFreeQueryListA(queryList);
}