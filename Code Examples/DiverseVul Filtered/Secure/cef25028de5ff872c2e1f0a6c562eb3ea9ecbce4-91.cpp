TEST(UriSuite, TestQueryStringEndingInEqualSignNonBug32) {
		const char * queryString = "firstname=sdsd&lastname=";

		UriQueryListA * queryList = NULL;
		int itemCount = 0;
		const int res = uriDissectQueryMallocA(&queryList, &itemCount,
				queryString, queryString + strlen(queryString));

		ASSERT_TRUE(res == URI_SUCCESS);
		ASSERT_TRUE(itemCount == 2);
		ASSERT_TRUE(queryList != NULL);
		ASSERT_TRUE(strcmp(queryList->key, "firstname") == 0);
		ASSERT_TRUE(strcmp(queryList->value, "sdsd") == 0);
		ASSERT_TRUE(strcmp(queryList->next->key, "lastname") == 0);
		ASSERT_TRUE(strcmp(queryList->next->value, "") == 0);
		ASSERT_TRUE(queryList->next->next == NULL);

		uriFreeQueryListA(queryList);
}