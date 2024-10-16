static void v9fs_path_init_dirname(V9fsPath *path, const char *str)
{
    path->data = g_path_get_dirname(str);
    path->size = strlen(path->data) + 1;
}