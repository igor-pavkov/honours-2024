int URI_FUNC(FreeUriMembersMm)(URI_TYPE(Uri) * uri, UriMemoryManager * memory) {
	if (uri == NULL) {
		return URI_ERROR_NULL;
	}

	URI_CHECK_MEMORY_MANAGER(memory);  /* may return */

	if (uri->owner) {
		/* Scheme */
		if (uri->scheme.first != NULL) {
			if (uri->scheme.first != uri->scheme.afterLast) {
				memory->free(memory, (URI_CHAR *)uri->scheme.first);
			}
			uri->scheme.first = NULL;
			uri->scheme.afterLast = NULL;
		}

		/* User info */
		if (uri->userInfo.first != NULL) {
			if (uri->userInfo.first != uri->userInfo.afterLast) {
				memory->free(memory, (URI_CHAR *)uri->userInfo.first);
			}
			uri->userInfo.first = NULL;
			uri->userInfo.afterLast = NULL;
		}

		/* Host data - IPvFuture */
		if (uri->hostData.ipFuture.first != NULL) {
			if (uri->hostData.ipFuture.first != uri->hostData.ipFuture.afterLast) {
				memory->free(memory, (URI_CHAR *)uri->hostData.ipFuture.first);
			}
			uri->hostData.ipFuture.first = NULL;
			uri->hostData.ipFuture.afterLast = NULL;
			uri->hostText.first = NULL;
			uri->hostText.afterLast = NULL;
		}

		/* Host text (if regname, after IPvFuture!) */
		if ((uri->hostText.first != NULL)
				&& (uri->hostData.ip4 == NULL)
				&& (uri->hostData.ip6 == NULL)) {
			/* Real regname */
			if (uri->hostText.first != uri->hostText.afterLast) {
				memory->free(memory, (URI_CHAR *)uri->hostText.first);
			}
			uri->hostText.first = NULL;
			uri->hostText.afterLast = NULL;
		}
	}

	/* Host data - IPv4 */
	if (uri->hostData.ip4 != NULL) {
		memory->free(memory, uri->hostData.ip4);
		uri->hostData.ip4 = NULL;
	}

	/* Host data - IPv6 */
	if (uri->hostData.ip6 != NULL) {
		memory->free(memory, uri->hostData.ip6);
		uri->hostData.ip6 = NULL;
	}

	/* Port text */
	if (uri->owner && (uri->portText.first != NULL)) {
		if (uri->portText.first != uri->portText.afterLast) {
			memory->free(memory, (URI_CHAR *)uri->portText.first);
		}
		uri->portText.first = NULL;
		uri->portText.afterLast = NULL;
	}

	/* Path */
	if (uri->pathHead != NULL) {
		URI_TYPE(PathSegment) * segWalk = uri->pathHead;
		while (segWalk != NULL) {
			URI_TYPE(PathSegment) * const next = segWalk->next;
			if (uri->owner && (segWalk->text.first != NULL)
					&& (segWalk->text.first < segWalk->text.afterLast)) {
				memory->free(memory, (URI_CHAR *)segWalk->text.first);
			}
			memory->free(memory, segWalk);
			segWalk = next;
		}
		uri->pathHead = NULL;
		uri->pathTail = NULL;
	}

	if (uri->owner) {
		/* Query */
		if (uri->query.first != NULL) {
			if (uri->query.first != uri->query.afterLast) {
				memory->free(memory, (URI_CHAR *)uri->query.first);
			}
			uri->query.first = NULL;
			uri->query.afterLast = NULL;
		}

		/* Fragment */
		if (uri->fragment.first != NULL) {
			if (uri->fragment.first != uri->fragment.afterLast) {
				memory->free(memory, (URI_CHAR *)uri->fragment.first);
			}
			uri->fragment.first = NULL;
			uri->fragment.afterLast = NULL;
		}
	}

	return URI_SUCCESS;
}