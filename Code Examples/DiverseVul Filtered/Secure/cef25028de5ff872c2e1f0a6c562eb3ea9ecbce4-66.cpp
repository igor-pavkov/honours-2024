TEST(UriParseSingleSuite, ErrorNullAfterLastDetected) {
	UriUriA uri;

	EXPECT_EQ(uriParseSingleUriExA(&uri, "foo", NULL, NULL), URI_SUCCESS);

	uriFreeUriMembersA(&uri);
}