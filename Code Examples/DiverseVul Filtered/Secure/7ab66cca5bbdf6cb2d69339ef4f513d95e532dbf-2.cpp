bool r_pkcs7_parse_digestalgorithmidentifier (RPKCS7DigestAlgorithmIdentifiers *dai, RASN1Object *object) {
	ut32 i;
	if (!dai && !object) {
		return false;
	}
	if (object->list.length > 0) {
		dai->elements = (RX509AlgorithmIdentifier **) calloc (object->list.length, sizeof (RX509AlgorithmIdentifier*));
		if (!dai->elements) {
			return false;
		}
		dai->length = object->list.length;
		for (i = 0; i < dai->length; ++i) {
			// r_x509_parse_algorithmidentifier returns bool,
			// so i have to allocate before calling the function
			dai->elements[i] = (RX509AlgorithmIdentifier *) malloc (sizeof (RX509AlgorithmIdentifier));
			//should i handle invalid memory? the function checks the pointer
			//or it should return if dai->elements[i] == NULL ?
			if (dai->elements[i]) {
				//Memset is needed to initialize to 0 the structure and avoid garbage.
				memset (dai->elements[i], 0, sizeof (RX509AlgorithmIdentifier));
				r_x509_parse_algorithmidentifier (dai->elements[i], object->list.objects[i]);
			}
		}
	}
	return true;
}