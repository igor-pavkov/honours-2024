TEST(UriParseSingleSuite, Success) {
	UriUriA uri;

	EXPECT_EQ(uriParseSingleUriA(&uri, "file:///home/user/song.mp3", NULL),
			URI_SUCCESS);

	uriFreeUriMembersA(&uri);
}