    static void lf(std::ostream& out,bool& bLF)
    {
        if ( bLF ) {
            out << std::endl;
            out.flush();
            bLF = false ;
        }
    }