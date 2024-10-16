static int init_ehdr(ELFOBJ *bin) {
	ut8 e_ident[EI_NIDENT];
	ut8 ehdr[sizeof (Elf_(Ehdr))] = {0};
	int i, len;
	if (r_buf_read_at (bin->b, 0, e_ident, EI_NIDENT) == -1) {
		bprintf ("Warning: read (magic)\n");
		return false;
	}
	sdb_set (bin->kv, "elf_type.cparse", "enum elf_type { ET_NONE=0, ET_REL=1,"
			" ET_EXEC=2, ET_DYN=3, ET_CORE=4, ET_LOOS=0xfe00, ET_HIOS=0xfeff,"
			" ET_LOPROC=0xff00, ET_HIPROC=0xffff };", 0);
	sdb_set (bin->kv, "elf_machine.cparse", "enum elf_machine{EM_NONE=0, EM_M32=1,"
			" EM_SPARC=2, EM_386=3, EM_68K=4, EM_88K=5, EM_486=6, "
			" EM_860=7, EM_MIPS=8, EM_S370=9, EM_MIPS_RS3_LE=10, EM_RS6000=11,"
			" EM_UNKNOWN12=12, EM_UNKNOWN13=13, EM_UNKNOWN14=14, "
			" EM_PA_RISC=15, EM_PARISC=EM_PA_RISC, EM_nCUBE=16, EM_VPP500=17,"
			" EM_SPARC32PLUS=18, EM_960=19, EM_PPC=20, EM_PPC64=21, "
			" EM_S390=22, EM_UNKNOWN22=EM_S390, EM_UNKNOWN23=23, EM_UNKNOWN24=24,"
			" EM_UNKNOWN25=25, EM_UNKNOWN26=26, EM_UNKNOWN27=27, EM_UNKNOWN28=28,"
			" EM_UNKNOWN29=29, EM_UNKNOWN30=30, EM_UNKNOWN31=31, EM_UNKNOWN32=32,"
			" EM_UNKNOWN33=33, EM_UNKNOWN34=34, EM_UNKNOWN35=35, EM_V800=36,"
			" EM_FR20=37, EM_RH32=38, EM_RCE=39, EM_ARM=40, EM_ALPHA=41, EM_SH=42,"
			" EM_SPARCV9=43, EM_TRICORE=44, EM_ARC=45, EM_H8_300=46, EM_H8_300H=47,"
			" EM_H8S=48, EM_H8_500=49, EM_IA_64=50, EM_MIPS_X=51, EM_COLDFIRE=52,"
			" EM_68HC12=53, EM_MMA=54, EM_PCP=55, EM_NCPU=56, EM_NDR1=57,"
			" EM_STARCORE=58, EM_ME16=59, EM_ST100=60, EM_TINYJ=61, EM_AMD64=62,"
			" EM_X86_64=EM_AMD64, EM_PDSP=63, EM_UNKNOWN64=64, EM_UNKNOWN65=65,"
			" EM_FX66=66, EM_ST9PLUS=67, EM_ST7=68, EM_68HC16=69, EM_68HC11=70,"
			" EM_68HC08=71, EM_68HC05=72, EM_SVX=73, EM_ST19=74, EM_VAX=75, "
			" EM_CRIS=76, EM_JAVELIN=77, EM_FIREPATH=78, EM_ZSP=79, EM_MMIX=80,"
			" EM_HUANY=81, EM_PRISM=82, EM_AVR=83, EM_FR30=84, EM_D10V=85, EM_D30V=86,"
			" EM_V850=87, EM_M32R=88, EM_MN10300=89, EM_MN10200=90, EM_PJ=91,"
			" EM_OPENRISC=92, EM_ARC_A5=93, EM_XTENSA=94, EM_NUM=95};", 0);
	sdb_num_set (bin->kv, "elf_header.offset", 0, 0);
	sdb_num_set (bin->kv, "elf_header.size", sizeof (Elf_(Ehdr)), 0);
#if R_BIN_ELF64
	sdb_set (bin->kv, "elf_header.format", "[16]z[2]E[2]Exqqqxwwwwww"
		" ident (elf_type)type (elf_machine)machine version entry phoff shoff flags ehsize"
		" phentsize phnum shentsize shnum shstrndx", 0);
#else
	sdb_set (bin->kv, "elf_header.format", "[16]z[2]E[2]Exxxxxwwwwww"
		" ident (elf_type)type (elf_machine)machine version entry phoff shoff flags ehsize"
		" phentsize phnum shentsize shnum shstrndx", 0);
#endif
	bin->endian = (e_ident[EI_DATA] == ELFDATA2MSB)? 1: 0;
	memset (&bin->ehdr, 0, sizeof (Elf_(Ehdr)));

	len = r_buf_read_at (bin->b, 0, ehdr, sizeof (Elf_(Ehdr)));
	if (len < 1) {
		bprintf ("Warning: read (ehdr)\n");
		return false;
	}
	memcpy (&bin->ehdr.e_ident, ehdr, 16);
	i = 16;
	bin->ehdr.e_type = READ16 (ehdr, i)
	bin->ehdr.e_machine = READ16 (ehdr, i)
	bin->ehdr.e_version = READ32 (ehdr, i)
#if R_BIN_ELF64
	bin->ehdr.e_entry = READ64 (ehdr, i)
	bin->ehdr.e_phoff = READ64 (ehdr, i)
	bin->ehdr.e_shoff = READ64 (ehdr, i)
#else
	bin->ehdr.e_entry = READ32 (ehdr, i)
	bin->ehdr.e_phoff = READ32 (ehdr, i)
	bin->ehdr.e_shoff = READ32 (ehdr, i)
#endif
	bin->ehdr.e_flags = READ32 (ehdr, i)
	bin->ehdr.e_ehsize = READ16 (ehdr, i)
	bin->ehdr.e_phentsize = READ16 (ehdr, i)
	bin->ehdr.e_phnum = READ16 (ehdr, i)
	bin->ehdr.e_shentsize = READ16 (ehdr, i)
	bin->ehdr.e_shnum = READ16 (ehdr, i)
	bin->ehdr.e_shstrndx = READ16 (ehdr, i)
	return handle_e_ident (bin);
	// Usage example:
	// > td `k bin/cur/info/elf_type.cparse`; td `k bin/cur/info/elf_machine.cparse`
	// > pf `k bin/cur/info/elf_header.format` @ `k bin/cur/info/elf_header.offset`
}