TEST(UriSuite, TestNormalizeSyntaxMaskRequired) {
		ASSERT_TRUE(testNormalizeMaskHelper(L"http://localhost/", URI_NORMALIZED));
		ASSERT_TRUE(testNormalizeMaskHelper(L"httP://localhost/", URI_NORMALIZE_SCHEME));
		ASSERT_TRUE(testNormalizeMaskHelper(L"http://%0d@localhost/", URI_NORMALIZE_USER_INFO));
		ASSERT_TRUE(testNormalizeMaskHelper(L"http://localhosT/", URI_NORMALIZE_HOST));
		ASSERT_TRUE(testNormalizeMaskHelper(L"http://localhost/./abc", URI_NORMALIZE_PATH));
		ASSERT_TRUE(testNormalizeMaskHelper(L"http://localhost/?AB%43", URI_NORMALIZE_QUERY));
		ASSERT_TRUE(testNormalizeMaskHelper(L"http://localhost/#AB%43", URI_NORMALIZE_FRAGMENT));
}