    SG_Exception_Dep::SG_Exception_Dep(const char* container_name, const char* arg1, const char* arg2)
    {
        std::string cn_s(container_name);
        std::string arg1_s(arg1);
        std::string arg2_s(arg2);

        this -> err_msg = "[" + cn_s + "] The attribute "
                + arg1_s
                + " may not exist without the attribute "
                + arg2_s
                + " existing.";
    }