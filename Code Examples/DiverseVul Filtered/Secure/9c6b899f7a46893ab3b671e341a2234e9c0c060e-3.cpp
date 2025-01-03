static int local_rename(FsContext *ctx, const char *oldpath,
                        const char *newpath)
{
    int err;
    char *oname = g_path_get_basename(oldpath);
    char *nname = g_path_get_basename(newpath);
    V9fsPath olddir, newdir;

    v9fs_path_init_dirname(&olddir, oldpath);
    v9fs_path_init_dirname(&newdir, newpath);

    err = local_renameat(ctx, &olddir, oname, &newdir, nname);

    v9fs_path_free(&newdir);
    v9fs_path_free(&olddir);
    g_free(nname);
    g_free(oname);

    return err;
}