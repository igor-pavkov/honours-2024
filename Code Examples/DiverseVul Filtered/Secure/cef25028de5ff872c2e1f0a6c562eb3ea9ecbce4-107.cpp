TEST(UriSuite, TestRangeComparison) {
		testCompareRangeHelper("", "", 0);
		testCompareRangeHelper("a", "", 1);
		testCompareRangeHelper("", "a", -1);

		testCompareRangeHelper("a", "a", 0);
		testCompareRangeHelper("a", "b", -1);
		testCompareRangeHelper("b", "a", 1);

		testCompareRangeHelper("a", "aa", -1);
		testCompareRangeHelper("aa", "a", 1);

		// Fixed with 0.8.1:
		testCompareRangeHelper(NULL, "a", -1);
		testCompareRangeHelper("a", NULL, 1);
		testCompareRangeHelper(NULL, NULL, 0);

		// Fixed with 0.8.3
		const bool KEEP_NULL_RANGE = false;
		const bool AVOID_NULL_RANGE = true;
		testCompareRangeHelper(NULL, "", -1, AVOID_NULL_RANGE);
		testCompareRangeHelper(NULL, "", -1, KEEP_NULL_RANGE);
		testCompareRangeHelper("", NULL, 1, AVOID_NULL_RANGE);
		testCompareRangeHelper("", NULL, 1, KEEP_NULL_RANGE);
}