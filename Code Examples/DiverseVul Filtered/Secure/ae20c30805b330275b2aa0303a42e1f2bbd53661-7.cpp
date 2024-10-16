static void flushBuffer(const char* buffer,size_t start,int& end,std::string& file)
{
    file += std::string(buffer+start,end-start) ;
    end = 0 ;
}