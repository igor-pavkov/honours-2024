UriBool URI_FUNC(IsHostSet)(const URI_TYPE(Uri) * uri) {
	return (uri != NULL)
			&& ((uri->hostText.first != NULL)
				|| (uri->hostData.ip4 != NULL)
				|| (uri->hostData.ip6 != NULL)
				|| (uri->hostData.ipFuture.first != NULL)
			);
}