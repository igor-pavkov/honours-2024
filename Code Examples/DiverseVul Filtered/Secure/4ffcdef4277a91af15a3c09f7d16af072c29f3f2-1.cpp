static XattrOperations *get_xattr_operations(XattrOperations **h,
                                             const char *name)
{
    XattrOperations *xops;
    for (xops = *(h)++; xops != NULL; xops = *(h)++) {
        if (!strncmp(name, xops->name, strlen(xops->name))) {
            return xops;
        }
    }
    return NULL;
}