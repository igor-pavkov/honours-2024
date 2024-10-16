TEST(UriSuite, TestAddBase) {
		// 5.4.1. Normal Examples
		ASSERT_TRUE(testAddBaseHelper(L"http://a/b/c/d;p?q", L"g:h", L"g:h"));
		ASSERT_TRUE(testAddBaseHelper(L"http://a/b/c/d;p?q", L"g", L"http://a/b/c/g"));
		ASSERT_TRUE(testAddBaseHelper(L"http://a/b/c/d;p?q", L"./g", L"http://a/b/c/g"));
		ASSERT_TRUE(testAddBaseHelper(L"http://a/b/c/d;p?q", L"g/", L"http://a/b/c/g/"));
		ASSERT_TRUE(testAddBaseHelper(L"http://a/b/c/d;p?q", L"/g", L"http://a/g"));
		ASSERT_TRUE(testAddBaseHelper(L"http://a/b/c/d;p?q", L"//g", L"http://g"));
		ASSERT_TRUE(testAddBaseHelper(L"http://a/b/c/d;p?q", L"?y", L"http://a/b/c/d;p?y"));
		ASSERT_TRUE(testAddBaseHelper(L"http://a/b/c/d;p?q", L"g?y", L"http://a/b/c/g?y"));
		ASSERT_TRUE(testAddBaseHelper(L"http://a/b/c/d;p?q", L"#s", L"http://a/b/c/d;p?q#s"));
		ASSERT_TRUE(testAddBaseHelper(L"http://a/b/c/d;p?q", L"g#s", L"http://a/b/c/g#s"));
		ASSERT_TRUE(testAddBaseHelper(L"http://a/b/c/d;p?q", L"g?y#s", L"http://a/b/c/g?y#s"));
		ASSERT_TRUE(testAddBaseHelper(L"http://a/b/c/d;p?q", L";x", L"http://a/b/c/;x"));
		ASSERT_TRUE(testAddBaseHelper(L"http://a/b/c/d;p?q", L"g;x", L"http://a/b/c/g;x"));
		ASSERT_TRUE(testAddBaseHelper(L"http://a/b/c/d;p?q", L"g;x?y#s", L"http://a/b/c/g;x?y#s"));
		ASSERT_TRUE(testAddBaseHelper(L"http://a/b/c/d;p?q", L"", L"http://a/b/c/d;p?q"));
		ASSERT_TRUE(testAddBaseHelper(L"http://a/b/c/d;p?q", L".", L"http://a/b/c/"));
		ASSERT_TRUE(testAddBaseHelper(L"http://a/b/c/d;p?q", L"./", L"http://a/b/c/"));
		ASSERT_TRUE(testAddBaseHelper(L"http://a/b/c/d;p?q", L"..", L"http://a/b/"));
		ASSERT_TRUE(testAddBaseHelper(L"http://a/b/c/d;p?q", L"../", L"http://a/b/"));
		ASSERT_TRUE(testAddBaseHelper(L"http://a/b/c/d;p?q", L"../g", L"http://a/b/g"));
		ASSERT_TRUE(testAddBaseHelper(L"http://a/b/c/d;p?q", L"../..", L"http://a/"));
		ASSERT_TRUE(testAddBaseHelper(L"http://a/b/c/d;p?q", L"../../", L"http://a/"));
		ASSERT_TRUE(testAddBaseHelper(L"http://a/b/c/d;p?q", L"../../g", L"http://a/g"));

		// 5.4.2. Abnormal Examples
		ASSERT_TRUE(testAddBaseHelper(L"http://a/b/c/d;p?q", L"../../../g", L"http://a/g"));
		ASSERT_TRUE(testAddBaseHelper(L"http://a/b/c/d;p?q", L"../../../../g", L"http://a/g"));
		ASSERT_TRUE(testAddBaseHelper(L"http://a/b/c/d;p?q", L"/./g", L"http://a/g"));
		ASSERT_TRUE(testAddBaseHelper(L"http://a/b/c/d;p?q", L"/../g", L"http://a/g"));
		ASSERT_TRUE(testAddBaseHelper(L"http://a/b/c/d;p?q", L"g.", L"http://a/b/c/g."));
		ASSERT_TRUE(testAddBaseHelper(L"http://a/b/c/d;p?q", L".g", L"http://a/b/c/.g"));
		ASSERT_TRUE(testAddBaseHelper(L"http://a/b/c/d;p?q", L"g..", L"http://a/b/c/g.."));
		ASSERT_TRUE(testAddBaseHelper(L"http://a/b/c/d;p?q", L"..g", L"http://a/b/c/..g"));
		ASSERT_TRUE(testAddBaseHelper(L"http://a/b/c/d;p?q", L"./../g", L"http://a/b/g"));
		ASSERT_TRUE(testAddBaseHelper(L"http://a/b/c/d;p?q", L"./g/.", L"http://a/b/c/g/"));
		ASSERT_TRUE(testAddBaseHelper(L"http://a/b/c/d;p?q", L"g/./h", L"http://a/b/c/g/h"));
		ASSERT_TRUE(testAddBaseHelper(L"http://a/b/c/d;p?q", L"g/../h", L"http://a/b/c/h"));
		ASSERT_TRUE(testAddBaseHelper(L"http://a/b/c/d;p?q", L"g;x=1/./y", L"http://a/b/c/g;x=1/y"));
		ASSERT_TRUE(testAddBaseHelper(L"http://a/b/c/d;p?q", L"g;x=1/../y", L"http://a/b/c/y"));
		ASSERT_TRUE(testAddBaseHelper(L"http://a/b/c/d;p?q", L"g?y/./x", L"http://a/b/c/g?y/./x"));
		ASSERT_TRUE(testAddBaseHelper(L"http://a/b/c/d;p?q", L"g?y/../x", L"http://a/b/c/g?y/../x"));
		ASSERT_TRUE(testAddBaseHelper(L"http://a/b/c/d;p?q", L"g#s/./x", L"http://a/b/c/g#s/./x"));
		ASSERT_TRUE(testAddBaseHelper(L"http://a/b/c/d;p?q", L"g#s/../x", L"http://a/b/c/g#s/../x"));
		ASSERT_TRUE(testAddBaseHelper(L"http://a/b/c/d;p?q", L"http:g", L"http:g"));

		// Backward compatibility (feature request #4, RFC3986 5.4.2)
		ASSERT_TRUE(testAddBaseHelper(L"http://a/b/c/d;p?q", L"http:g", L"http:g", false));
		ASSERT_TRUE(testAddBaseHelper(L"http://a/b/c/d;p?q", L"http:g", L"http://a/b/c/g", true));
		ASSERT_TRUE(testAddBaseHelper(L"http://a/b/c/d;p?q", L"http:g?q#f", L"http://a/b/c/g?q#f", true));
		ASSERT_TRUE(testAddBaseHelper(L"http://a/b/c/d;p?q", L"other:g?q#f", L"other:g?q#f", true));

		// Bug related to absolutePath flag set despite presence of host
		ASSERT_TRUE(testAddBaseHelper(L"http://a/b/c/d;p?q", L"/", L"http://a/"));
		ASSERT_TRUE(testAddBaseHelper(L"http://a/b/c/d;p?q", L"/g/", L"http://a/g/"));
}