void URI_FUNC(FreeUriMembers)(URI_TYPE(Uri) * uri) {
	URI_FUNC(FreeUriMembersMm)(uri, NULL);
}