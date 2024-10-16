TEST(UriSuite, TestNormalizeSyntax) {
		ASSERT_TRUE(testNormalizeSyntaxHelper(
				L"eXAMPLE://a/./b/../b/%63/%7bfoo%7d",
				L"example://a/b/c/%7Bfoo%7D"));

		// Testcase by Adrian Manrique
		ASSERT_TRUE(testNormalizeSyntaxHelper(
				L"http://examp%4Ce.com/",
				L"http://example.com/"));

		// Testcase by Adrian Manrique
		ASSERT_TRUE(testNormalizeSyntaxHelper(
				L"http://example.com/a/b/%2E%2E/",
				L"http://example.com/a/"));

		// Reported by Adrian Manrique
		ASSERT_TRUE(testNormalizeSyntaxHelper(
				L"http://user:pass@SOMEHOST.COM:123",
				L"http://user:pass@somehost.com:123"));

		ASSERT_TRUE(testNormalizeSyntaxHelper(
				L"HTTP://a:b@HOST:123/./1/2/../%41?abc#def",
				L"http://a:b@host:123/1/A?abc#def"));

		ASSERT_TRUE(testNormalizeSyntaxHelper(
				L"../../abc",
				L"../../abc"));

		ASSERT_TRUE(testNormalizeSyntaxHelper(
				L"../../abc/..",
				L"../../"));

		ASSERT_TRUE(testNormalizeSyntaxHelper(
				L"../../abc/../def",
				L"../../def"));

		ASSERT_TRUE(testNormalizeSyntaxHelper(
				L"abc/..",
				L""));

		ASSERT_TRUE(testNormalizeSyntaxHelper(
				L"abc/../",
				L""));

		ASSERT_TRUE(testNormalizeSyntaxHelper(
				L"../../abc/./def",
				L"../../abc/def"));

		ASSERT_TRUE(testNormalizeSyntaxHelper(
				L"./def",
				L"def"));

		ASSERT_TRUE(testNormalizeSyntaxHelper(
				L"def/.",
				L"def/"));

		ASSERT_TRUE(testNormalizeSyntaxHelper(
				L"./abc:def",
				L"./abc:def"));
}