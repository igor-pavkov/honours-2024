TEST(FreeUriMembersSuite, MultiFreeWorksFine) {
	UriUriA uri;

	EXPECT_EQ(uriParseSingleUriA(&uri, "file:///home/user/song.mp3", NULL),
			URI_SUCCESS);

	UriUriA uriBackup = uri;
	EXPECT_EQ(memcmp(&uriBackup, &uri, sizeof(UriUriA)), 0);

	uriFreeUriMembersA(&uri);

	// Did some pointers change (to NULL)?
	EXPECT_NE(memcmp(&uriBackup, &uri, sizeof(UriUriA)), 0);

	uriFreeUriMembersA(&uri);  // second time
}