TEST(UriSuite, QueryList) {
		testQueryListHelper(L"one=ONE&two=TWO", 2);
		testQueryListHelper(L"one=ONE&two=&three=THREE", 3);
		testQueryListHelper(L"one=ONE&two&three=THREE", 3);
		testQueryListHelper(L"one=ONE", 1);
		testQueryListHelper(L"one", 1);
		testQueryListHelper(L"", 0);
}