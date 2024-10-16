    SG_Exception_BadSum::SG_Exception_BadSum(const char* container_name, const char* arg1, const char* arg2)
    {
        std::string cn_s(container_name);
        std::string arg1_s(arg1);
        std::string arg2_s(arg2);

        this -> err_msg = "[" + cn_s + "]"
                + " The sum of all values in "
                + arg1_s
                + " and "
                + arg2_s
                + " do not match.";
    }