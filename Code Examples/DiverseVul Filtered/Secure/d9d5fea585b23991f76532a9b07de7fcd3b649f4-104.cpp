static int wsgi_find_path_info(const char *uri, const char *path_info)
{
    int lu = strlen(uri);
    int lp = strlen(path_info);

    while (lu-- && lp-- && uri[lu] == path_info[lp]) {
        if (path_info[lp] == '/') {
            while (lu && uri[lu-1] == '/') lu--;
        }
    }

    if (lu == -1) {
        lu = 0;
    }

    while (uri[lu] != '\0' && uri[lu] != '/') {
        lu++;
    }
    return lu;
}