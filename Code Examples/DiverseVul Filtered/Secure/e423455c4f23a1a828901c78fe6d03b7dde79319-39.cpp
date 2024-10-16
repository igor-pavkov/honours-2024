const char *load_elf_strerror(int error)
{
    switch (error) {
    case 0:
        return "No error";
    case ELF_LOAD_FAILED:
        return "Failed to load ELF";
    case ELF_LOAD_NOT_ELF:
        return "The image is not ELF";
    case ELF_LOAD_WRONG_ARCH:
        return "The image is from incompatible architecture";
    case ELF_LOAD_WRONG_ENDIAN:
        return "The image has incorrect endianness";
    case ELF_LOAD_TOO_BIG:
        return "The image segments are too big to load";
    default:
        return "Unknown error";
    }
}