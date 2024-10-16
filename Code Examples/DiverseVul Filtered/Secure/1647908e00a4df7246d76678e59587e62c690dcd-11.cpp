                void printStructure(std::ostream& os, PrintStructureOption option, int depth)
                {
                    printIFD(os, option, header_.dirOffset(), depth - 1);
                }