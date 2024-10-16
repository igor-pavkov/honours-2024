std::string HttpFileImpl::getMd5() const
{
    return utils::getMd5(fileContent_.data(), fileContent_.size());
}