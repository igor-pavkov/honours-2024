    SG_Exception_General_Malformed
        ::SG_Exception_General_Malformed(const char * arg)
    {
        std::string arg1_s(arg);

        this -> err_msg = "Corruption or malformed formatting has been detected in: " + arg1_s;
    }