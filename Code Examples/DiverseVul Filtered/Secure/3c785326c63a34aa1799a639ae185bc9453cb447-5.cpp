HttpFile::HttpFile(std::shared_ptr<HttpFileImpl> &&implPtr)
    : implPtr_(std::move(implPtr))
{
}