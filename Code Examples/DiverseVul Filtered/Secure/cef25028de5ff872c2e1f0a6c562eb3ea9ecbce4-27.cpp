TEST(UriSuite, TestDistinction) {
		/*
============================================================================
Rule                                | Example | hostSet | absPath | emptySeg
------------------------------------|---------|---------|---------|---------
1) URI = scheme ":" hier-part ...   |         |         |         |
   1) "//" authority path-abempty   | "s://"  | true    |   false |   false
                                    | "s:///" | true    |   false | true
                                    | "s://a" | true    |   false |   false
                                    | "s://a/"| true    |   false | true
   2) path-absolute                 | "s:/"   |   false | true    |   false
   3) path-rootless                 | "s:a"   |   false |   false |   false
                                    | "s:a/"  |   false |   false | true
   4) path-empty                    | "s:"    |   false |   false |   false
------------------------------------|---------|---------|---------|---------
2) relative-ref = relative-part ... |         |         |         |
   1) "//" authority path-abempty   | "//"    | true    |   false |   false
                                    | "///"   | true    |   false | true
   2) path-absolute                 | "/"     |   false | true    |   false
   3) path-noscheme                 | "a"     |   false |   false |   false
                                    | "a/"    |   false |   false | true
   4) path-empty                    | ""      |   false |   false |   false
============================================================================
		*/
		ASSERT_TRUE(testDistinctionHelper("s://", true, false, false));
		ASSERT_TRUE(testDistinctionHelper("s:///", true, false, true));
		ASSERT_TRUE(testDistinctionHelper("s://a", true, false, false));
		ASSERT_TRUE(testDistinctionHelper("s://a/", true, false, true));
		ASSERT_TRUE(testDistinctionHelper("s:/", false, true, false));
		ASSERT_TRUE(testDistinctionHelper("s:a", false, false, false));
		ASSERT_TRUE(testDistinctionHelper("s:a/", false, false, true));
		ASSERT_TRUE(testDistinctionHelper("s:", false, false, false));

		ASSERT_TRUE(testDistinctionHelper("//", true, false, false));
		ASSERT_TRUE(testDistinctionHelper("///", true, false, true));
		ASSERT_TRUE(testDistinctionHelper("/", false, true, false));
		ASSERT_TRUE(testDistinctionHelper("a", false, false, false));
		ASSERT_TRUE(testDistinctionHelper("a/", false, false, true));
		ASSERT_TRUE(testDistinctionHelper("", false, false, false));
}