void PrintEncodeUsage()
{
	u32 i=0;
	gf_sys_format_help(helpout, help_flags, "# MPEG-4 Scene Encoding Options\n"
		"## General considerations\n"
		"MP4Box supports encoding and decoding of of BT, XMT, VRML and (partially) X3D formats int MPEG-4 BIFS, and encoding and decoding of XSR and SVG into MPEG-4 LASeR\n"
		"Any media track specified through a `MuxInfo` element will be imported in the resulting MP4 file.\n"
		"See https://wiki.gpac.io/MPEG-4-BIFS-Textual-Format and related pages.\n"
		"## Scene Random Access\n"
		"MP4Box can encode BIFS or LASeR streams and insert random access points at a given frequency. This is useful when packaging content for broadcast, where users will not turn in the scene at the same time. In MPEG-4 terminology, this is called the __scene carousel__."
		"## BIFS Chunk Processing\n"
		"The BIFS chunk encoding mode alows encoding single BIFS access units from an initial context and a set of commands.\n"
		"The generated AUs are raw BIFS (not SL-packetized), in files called FILE-ESID-AUIDX.bifs, with FILE the basename of the input file.\n"
		"Commands with a timing of 0 in the input will modify the carousel version only (i.e. output context).\n"
		"Commands with a timing different from 0 in the input will generate new AUs.\n"
		"  \n"
		"Options:\n"
	);

	while (m4b_senc_args[i].name) {
		GF_GPACArg *arg = (GF_GPACArg *) &m4b_senc_args[i];
		i++;
		gf_sys_print_arg(helpout, help_flags, arg, "mp4box-senc");
	}
}