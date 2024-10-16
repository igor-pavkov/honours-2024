TEST(UriParseSingleSuite, ErrorNullMemoryManagerDetected) {
	UriUriA uri;
	const char * errorPos;
	const char * const uriString = "somethingwellformed";

	EXPECT_EQ(uriParseSingleUriExMmA(&uri,
			uriString,
			uriString + strlen(uriString),
			&errorPos, NULL), URI_SUCCESS);

	EXPECT_EQ(uriFreeUriMembersMmA(&uri, NULL), URI_SUCCESS);
}