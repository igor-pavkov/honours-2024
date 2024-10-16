flatpak_dir_pull_untrusted_local (FlatpakDir          *self,
                                  const char          *src_path,
                                  const char          *remote_name,
                                  const char          *ref,
                                  const char         **subpaths,
                                  FlatpakProgress     *progress,
                                  GCancellable        *cancellable,
                                  GError             **error)
{
  g_autoptr(GFile) path_file = g_file_new_for_path (src_path);
  g_autofree char *url = g_file_get_uri (path_file);
  g_autofree char *checksum = NULL;
  g_autofree char *current_checksum = NULL;
  gboolean gpg_verify_summary;
  gboolean gpg_verify;
  g_autoptr(OstreeGpgVerifyResult) gpg_result = NULL;
  g_autoptr(GVariant) old_commit = NULL;
  g_autoptr(OstreeRepo) src_repo = NULL;
  g_autoptr(GVariant) new_commit = NULL;
  g_autoptr(GVariant) new_commit_metadata = NULL;
  g_autoptr(GVariant) extra_data_sources = NULL;
  g_autoptr(GPtrArray) subdirs_arg = NULL;
  g_auto(GLnxLockFile) lock = { 0, };
  gboolean ret = FALSE;
  g_autofree const char **ref_bindings = NULL;

  if (!flatpak_dir_ensure_repo (self, cancellable, error))
    return FALSE;

  /* Keep a shared repo lock to avoid prunes removing objects we're relying on
   * while we do the pull. There are two cases we protect against. 1) objects we
   * need but that we already decided are locally available could be removed,
   * and 2) during the transaction commit objects that don't yet have a ref to
   * them could be considered unreachable.
   */
  if (!flatpak_dir_repo_lock (self, &lock, LOCK_SH, cancellable, error))
    return FALSE;

  if (!ostree_repo_remote_get_gpg_verify_summary (self->repo, remote_name,
                                                  &gpg_verify_summary, error))
    return FALSE;

  if (!ostree_repo_remote_get_gpg_verify (self->repo, remote_name,
                                          &gpg_verify, error))
    return FALSE;

  /* This was verified in the client, but lets do it here too */
  if (!gpg_verify_summary || !gpg_verify)
    return flatpak_fail_error (error, FLATPAK_ERROR_UNTRUSTED, _("Can't pull from untrusted non-gpg verified remote"));

  if (!flatpak_repo_resolve_rev (self->repo, NULL, remote_name, ref, TRUE,
                                 &current_checksum, NULL, error))
    return FALSE;

  if (current_checksum != NULL &&
      !ostree_repo_load_commit (self->repo, current_checksum, &old_commit, NULL, error))
    return FALSE;

  src_repo = ostree_repo_new (path_file);
  if (!ostree_repo_open (src_repo, cancellable, error))
    return FALSE;

  if (!flatpak_repo_resolve_rev (src_repo, NULL, remote_name, ref, FALSE, &checksum, NULL, error))
    return FALSE;

  if (gpg_verify)
    {
      gpg_result = ostree_repo_verify_commit_for_remote (src_repo, checksum, remote_name, cancellable, error);
      if (gpg_result == NULL)
        return FALSE;

      if (ostree_gpg_verify_result_count_valid (gpg_result) == 0)
        return flatpak_fail_error (error, FLATPAK_ERROR_UNTRUSTED, _("GPG signatures found, but none are in trusted keyring"));
    }

  g_clear_object (&gpg_result);

  if (!ostree_repo_load_commit (src_repo, checksum, &new_commit, NULL, error))
    return FALSE;

  /* Here we check that there is actually a ref binding, otherwise we
     could allow installing a ref as another app, because both would
     pass gpg validation. Note that ostree pull actually also verifies
     the ref-bindings, but only if they exist. We could do only the
     ref-binding existence check, but if we got something weird might as
     well stop handling it early. */

  new_commit_metadata = g_variant_get_child_value (new_commit, 0);
  if (!g_variant_lookup (new_commit_metadata, OSTREE_COMMIT_META_KEY_REF_BINDING, "^a&s", &ref_bindings))
    return flatpak_fail_error (error, FLATPAK_ERROR_INVALID_DATA, _("Commit for ‘%s’ has no ref binding"),  ref);

  if (!g_strv_contains ((const char *const *) ref_bindings, ref))
    {
      g_autofree char *as_string = g_strjoinv (", ", (char **)ref_bindings);
      return flatpak_fail_error (error, FLATPAK_ERROR_INVALID_DATA, _("Commit for ‘%s’ is not in expected bound refs: %s"),  ref, as_string);
    }

  if (old_commit)
    {
      guint64 old_timestamp;
      guint64 new_timestamp;

      old_timestamp = ostree_commit_get_timestamp (old_commit);
      new_timestamp = ostree_commit_get_timestamp (new_commit);

      if (new_timestamp < old_timestamp)
        return flatpak_fail_error (error, FLATPAK_ERROR_DOWNGRADE, "Not allowed to downgrade %s (old_commit: %s/%" G_GINT64_FORMAT " new_commit: %s/%" G_GINT64_FORMAT ")",
                                   ref, current_checksum, old_timestamp, checksum, new_timestamp);
    }

  if (subpaths != NULL && subpaths[0] != NULL)
    {
      subdirs_arg = g_ptr_array_new_with_free_func (g_free);
      int i;
      g_ptr_array_add (subdirs_arg, g_strdup ("/metadata"));
      for (i = 0; subpaths[i] != NULL; i++)
        g_ptr_array_add (subdirs_arg,
                         g_build_filename ("/files", subpaths[i], NULL));
      g_ptr_array_add (subdirs_arg, NULL);
    }

  if (!ostree_repo_prepare_transaction (self->repo, NULL, cancellable, error))
    goto out;

  /* Past this we must use goto out, so we abort the transaction on error */

  if (!repo_pull_local_untrusted (self, self->repo, remote_name, url,
                                  subdirs_arg ? (const char **) subdirs_arg->pdata : NULL,
                                  ref, checksum, progress,
                                  cancellable, error))
    {
      g_prefix_error (error, _("While pulling %s from remote %s: "), ref, remote_name);
      goto out;
    }

  /* Get the out of bands extra-data required due to an ostree pull
     commitmeta size limit */
  extra_data_sources = flatpak_commit_get_extra_data_sources (new_commit, NULL);
  if (extra_data_sources)
    {
      GFile *dir = ostree_repo_get_path (src_repo);
      g_autoptr(GFile) file = NULL;
      g_autofree char *filename = NULL;
      g_autofree char *commitmeta = NULL;
      gsize commitmeta_size;
      g_autoptr(GVariant) new_metadata = NULL;

      filename = g_strconcat (checksum, ".commitmeta", NULL);
      file = g_file_get_child (dir, filename);
      if (!g_file_load_contents (file, cancellable,
                                 &commitmeta, &commitmeta_size,
                                 NULL, error))
        goto out;

      new_metadata = g_variant_ref_sink (g_variant_new_from_data (G_VARIANT_TYPE ("a{sv}"),
                                                                  commitmeta, commitmeta_size,
                                                                  FALSE,
                                                                  g_free, commitmeta));
      g_steal_pointer (&commitmeta); /* steal into the variant */

      if (!ostree_repo_write_commit_detached_metadata (self->repo, checksum, new_metadata, cancellable, error))
        goto out;
    }

  if (!ostree_repo_commit_transaction (self->repo, NULL, cancellable, error))
    goto out;

  ret = TRUE;

  flatpak_dir_log (self, "pull local", src_path, ref, checksum, current_checksum, NULL,
                   "Pulled %s from %s", ref, src_path);
out:
  if (!ret)
    ostree_repo_abort_transaction (self->repo, cancellable, NULL);

  return ret;
}