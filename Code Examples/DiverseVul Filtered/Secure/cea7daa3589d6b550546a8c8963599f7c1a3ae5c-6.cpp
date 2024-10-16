key_ref_t keyring_search(key_ref_t keyring,
			 struct key_type *type,
			 const char *description)
{
	if (!type->match)
		return ERR_PTR(-ENOKEY);

	return keyring_search_aux(keyring, current->cred,
				  type, description, type->match);

} /* end keyring_search() */