int HttpFileImpl::save(const std::string &path) const
{
    assert(!path.empty());
    if (fileName_.empty())
        return -1;
    filesystem::path fsUploadDir(utils::toNativePath(path));

    if (!fsUploadDir.is_absolute() && (!fsUploadDir.has_parent_path() ||
                                       (fsUploadDir.begin()->string() != "." &&
                                        fsUploadDir.begin()->string() != "..")))
    {
        fsUploadDir = utils::toNativePath(
                          HttpAppFrameworkImpl::instance().getUploadPath()) /
                      fsUploadDir;
    }

    fsUploadDir = filesystem::weakly_canonical(fsUploadDir);

    if (!filesystem::exists(fsUploadDir))
    {
        LOG_TRACE << "create path:" << fsUploadDir;
        drogon::error_code err;
        filesystem::create_directories(fsUploadDir, err);
        if (err)
        {
            LOG_SYSERR;
            return -1;
        }
    }

    filesystem::path fsSaveToPath(filesystem::weakly_canonical(
        fsUploadDir / utils::toNativePath(fileName_)));

    if (!std::equal(fsUploadDir.begin(),
                    fsUploadDir.end(),
                    fsSaveToPath.begin()))
    {
        LOG_ERROR
            << "Attempt writing outside of upload directory detected. Path: "
            << fileName_;
        return -1;
    }

    return saveTo(fsSaveToPath);
}