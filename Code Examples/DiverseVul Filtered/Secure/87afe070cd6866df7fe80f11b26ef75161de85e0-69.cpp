void PrintEncryptUsage()
{
	u32 i=0;
	gf_sys_format_help(helpout, help_flags, "# Encryption/Decryption Options\n"
	"MP4Box supports encryption and decryption of ISMA, OMA and CENC content, see [encryption filter `gpac -h cecrypt`](cecrypt).\n"
	"It requires a specific XML file called `CryptFile`, whose syntax is available at https://wiki.gpac.io/Common-Encryption\n"
	"Image files (HEIF) can also be crypted / decrypted, using CENC only.\n"
	"  \n"
	"Options:\n"
	);
	while (m4b_crypt_args[i].name) {
		GF_GPACArg *arg = (GF_GPACArg *) &m4b_crypt_args[i];
		i++;
		gf_sys_print_arg(helpout, help_flags, arg, "mp4box-crypt");
	}
}