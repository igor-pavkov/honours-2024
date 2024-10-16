int HttpFileImpl::save() const
{
    return save(HttpAppFrameworkImpl::instance().getUploadPath());
}