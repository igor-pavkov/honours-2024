pk_transaction_search_details (PkTransaction *transaction,
			       GVariant *params,
			       GDBusMethodInvocation *context)
{
	gboolean ret;
	PkBitfield filter;
	g_autofree gchar **values = NULL;
	g_autoptr(GError) error = NULL;

	g_return_if_fail (PK_IS_TRANSACTION (transaction));
	g_return_if_fail (transaction->priv->tid != NULL);

	g_variant_get (params, "(t^a&s)",
		       &filter,
		       &values);

	g_debug ("SearchDetails method called: %" G_GUINT64_FORMAT ", %s",
		 filter, values[0]);

	/* not implemented yet */
	if (!pk_backend_is_implemented (transaction->priv->backend,
					PK_ROLE_ENUM_SEARCH_DETAILS)) {
		g_set_error (&error,
			     PK_TRANSACTION_ERROR,
			     PK_TRANSACTION_ERROR_NOT_SUPPORTED,
			     "SearchDetails not supported by backend");
		pk_transaction_set_state (transaction, PK_TRANSACTION_STATE_ERROR);
		goto out;
	}

	/* check the search term */
	ret = pk_transaction_search_check (values, &error);
	if (!ret) {
		pk_transaction_set_state (transaction, PK_TRANSACTION_STATE_ERROR);
		goto out;
	}

	/* save so we can run later */
	transaction->priv->cached_filters = filter;
	transaction->priv->cached_values = g_strdupv (values);
	pk_transaction_set_role (transaction, PK_ROLE_ENUM_SEARCH_DETAILS);
	pk_transaction_set_state (transaction, PK_TRANSACTION_STATE_READY);
out:
	pk_transaction_dbus_return (context, error);
}