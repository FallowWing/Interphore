#define ZIP_HEADERS_MAX 2048

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint;

struct LocalFileHeader {
	uint signature;
	uint16 extractVersion;
	uint16 bitFlags;
	uint16 compressionMethod;
	uint16 loadModFileTime;
	uint16 loadModFileDate;
	uint crc;
	uint compressedSize;
	uint uncompressedSize;
	uint16 fileNameLength;
	uint16 extraFieldLength;

	char *fileName;
	char *extraField;
	unsigned char *compressedData;
	unsigned char *uncompressedData;
};

struct Zip {
	LocalFileHeader headers[ZIP_HEADERS_MAX];
	int headersNum;
};

void openZip(unsigned char *data, int size, Zip *zip);
void closeZip(Zip *zip);

void openZip(unsigned char *data, int size, Zip *zip) {
	memset(zip, 0, sizeof(Zip));

	for (;;) {
		int signature = ((int *)data)[0];
		if (signature == 0x04034b50) {
			LocalFileHeader *header = &zip->headers[zip->headersNum++];
			ConsumeBytes(&header->signature, data, 4);
			ConsumeBytes(&header->extractVersion, data, 2);
			ConsumeBytes(&header->bitFlags, data, 2);
			ConsumeBytes(&header->compressionMethod, data, 2);
			ConsumeBytes(&header->loadModFileTime, data, 2);
			ConsumeBytes(&header->loadModFileDate, data, 2);
			ConsumeBytes(&header->crc, data, 4);
			ConsumeBytes(&header->compressedSize, data, 4);
			ConsumeBytes(&header->uncompressedSize, data, 4);
			ConsumeBytes(&header->fileNameLength, data, 2);
			ConsumeBytes(&header->extraFieldLength, data, 2);

			header->fileName = (char *)Malloc(header->fileNameLength+1);
			memset(header->fileName, 0, header->fileNameLength+1);

			header->extraField = (char *)Malloc(header->extraFieldLength+1);
			memset(header->extraField, 0, header->extraFieldLength+1);

			header->compressedData = (unsigned char *)Malloc(header->compressedSize);
			header->uncompressedData = (unsigned char *)Malloc(header->uncompressedSize);

			ConsumeBytes(header->fileName, data, header->fileNameLength);
			ConsumeBytes(header->extraField, data, header->extraFieldLength);
			ConsumeBytes(header->compressedData, data, header->compressedSize);

			if (header->compressionMethod == 0) {
				memcpy(header->uncompressedData, header->compressedData, header->uncompressedSize);
			} else if (header->compressionMethod == 8) {
				z_stream stream = {};
				stream.next_in = header->compressedData;
				stream.avail_in = header->compressedSize;
				stream.next_out = header->uncompressedData;
				stream.avail_out = header->uncompressedSize;

				int status;
				if ((status = inflateInit2(&stream, -MZ_DEFAULT_WINDOW_BITS)) == Z_OK) {
					if ((status = inflate(&stream, Z_SYNC_FLUSH)) == Z_STREAM_END) {
						if ((status = inflateEnd(&stream)) != Z_OK) {
							printf("inflateEnd() failed with %d!\n", status);
						}
					} else {
						printf("inflate() failed with %d!\n", status);
					}
				} else {
					printf("inflateInit() failed with %d!\n", status);
				}
			}
		} else if (signature == 0x08074b50) {
			printf("We don't parse data descriptors! (Unsupported zip file)\n");
			return;
		} else if (signature == 0x02014b50) { // Central directory structure
			break;
		} else if (signature == 0x08064b50) { // Archive extra data record
			break;
		} else {
			printf("Unknown signature %x (Unsupported zip file)\n", signature);
			return;
		}
	}
}

void closeZip(Zip *zip) {
	for (int i = 0; i < zip->headersNum; i++) {
		LocalFileHeader *header = &zip->headers[i];

		if (header->fileName) Free(header->fileName);
		if (header->extraField) Free(header->extraField);
		if (header->compressedData) Free(header->compressedData);
		if (header->uncompressedData) Free(header->uncompressedData);
	}
}
