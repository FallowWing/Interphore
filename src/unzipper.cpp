#define ZIP_HEADERS_MAX 2048
#define INFLATE_BUFFER_SIZE (1024 * 1024)

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

unsigned char inBuffer[INFLATE_BUFFER_SIZE];
unsigned char outBuffer[INFLATE_BUFFER_SIZE];

void openZip(unsigned char *data, int size, Zip *zip);

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
				uint infile_remaining = header->compressedSize;
				unsigned char *inPos = header->compressedData;
				unsigned char *outPos = header->uncompressedData;

				z_stream stream = {};
				stream.next_in = inBuffer;
				stream.avail_in = 0;
				stream.next_out = outBuffer;
				stream.avail_out = INFLATE_BUFFER_SIZE;

				if (inflateInit2(&stream, -MZ_DEFAULT_WINDOW_BITS)) {
					printf("inflateInit() failed!\n");
					return;
				}

				for (;;) {
					if (!stream.avail_in) {
						uint bytesToGet = Min(INFLATE_BUFFER_SIZE, infile_remaining);

						memcpy(inBuffer, inPos, bytesToGet);

						stream.next_in = inBuffer;
						stream.avail_in = bytesToGet;

						inPos += bytesToGet;
						infile_remaining -= bytesToGet;
					}

					int status = inflate(&stream, Z_SYNC_FLUSH);

					if (status == Z_STREAM_END || !stream.avail_out) {
						uint bytesDone = INFLATE_BUFFER_SIZE - stream.avail_out;
						memcpy(outPos, outBuffer, bytesDone);

						stream.next_out = outBuffer;
						stream.avail_out = INFLATE_BUFFER_SIZE;
					}

					if (status == Z_STREAM_END) {
						break;
					} else if (status != Z_OK) {
						printf("inflate() failed with status %i!\n", status);
						return;
					}
				}

				if (inflateEnd(&stream) != Z_OK) {
					printf("inflateEnd() failed!\n");
					return;
				}
			}

			printf("Name: %s\n", header->fileName);
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
