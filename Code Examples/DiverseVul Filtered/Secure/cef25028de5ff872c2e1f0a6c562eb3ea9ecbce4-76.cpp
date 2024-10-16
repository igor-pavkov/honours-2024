TEST(UriSuite, TestNormalizeSyntaxComponents) {
		ASSERT_TRUE(testNormalizeSyntaxHelper(
				L"HTTP://%41@EXAMPLE.ORG/../a?%41#%41",
				L"http://%41@EXAMPLE.ORG/../a?%41#%41",
				URI_NORMALIZE_SCHEME));

		ASSERT_TRUE(testNormalizeSyntaxHelper(
				L"HTTP://%41@EXAMPLE.ORG/../a?%41#%41",
				L"HTTP://A@EXAMPLE.ORG/../a?%41#%41",
				URI_NORMALIZE_USER_INFO));

		ASSERT_TRUE(testNormalizeSyntaxHelper(
				L"HTTP://%41@EXAMPLE.ORG/../a?%41#%41",
				L"HTTP://%41@example.org/../a?%41#%41",
				URI_NORMALIZE_HOST));

		ASSERT_TRUE(testNormalizeSyntaxHelper(
				L"HTTP://%41@EXAMPLE.ORG/../a?%41#%41",
				L"HTTP://%41@EXAMPLE.ORG/a?%41#%41",
				URI_NORMALIZE_PATH));

		ASSERT_TRUE(testNormalizeSyntaxHelper(
				L"HTTP://%41@EXAMPLE.ORG/../a?%41#%41",
				L"HTTP://%41@EXAMPLE.ORG/../a?A#%41",
				URI_NORMALIZE_QUERY));

		ASSERT_TRUE(testNormalizeSyntaxHelper(
				L"HTTP://%41@EXAMPLE.ORG/../a?%41#%41",
				L"HTTP://%41@EXAMPLE.ORG/../a?%41#A",
				URI_NORMALIZE_FRAGMENT));
}