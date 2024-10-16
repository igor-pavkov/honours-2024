    void Image::printStructure(std::ostream&, PrintStructureOption,int /*depth*/)
    {
        throw Error(kerUnsupportedImageType, io_->path());
    }