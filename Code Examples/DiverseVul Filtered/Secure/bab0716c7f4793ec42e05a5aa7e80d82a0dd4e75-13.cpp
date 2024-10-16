static MXFPackage* mxf_resolve_source_package(MXFContext *mxf, UID package_ul, UID package_uid)
{
    MXFPackage *package = NULL;
    int i;

    for (i = 0; i < mxf->packages_count; i++) {
        package = mxf_resolve_strong_ref(mxf, &mxf->packages_refs[i], SourcePackage);
        if (!package)
            continue;

        if (!memcmp(package->package_ul, package_ul, 16) && !memcmp(package->package_uid, package_uid, 16))
            return package;
    }
    return NULL;
}