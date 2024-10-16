    SG_Exception_Existential::SG_Exception_Existential(const char* container_name, const char* missing_name)
    {
        std::string cn_s(container_name);
        std::string mn_s(missing_name);

        this -> err_msg = "[" + cn_s + "] The property or the variable associated with "
                + mn_s
                + " is missing.";
    }