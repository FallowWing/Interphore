struct Zip {
};

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

	char fileName[Kilobytes(64)];
	char extraField[Kilobytes(64)];
};

struct DataDescriptor {
	uint signature;
	uint crc;
	uint compressedSize;
	uint uncompressedSize;
};

// local file header signature     4 bytes  (0x04034b50)
// version needed to extract       2 bytes
// general purpose bit flag        2 bytes
// compression method              2 bytes
// last mod file time              2 bytes
// last mod file date              2 bytes
// crc-32                          4 bytes
// compressed size                 4 bytes
// uncompressed size               4 bytes
// file name length                2 bytes
// extra field length              2 bytes

void openZip(unsigned char *data, int size, Zip *zip);
uint endian_swap(uint x);
#define ConsumeBytes(dest, src, count) memcpy(dest, src, count); src += count;

void openZip(unsigned char *data, int size, Zip *zip) {
	unsigned char *baseData = data;

	for (;;) {
		LocalFileHeader header = {};
		ConsumeBytes(&header.signature, data, 4);

		if (header.signature != 0x04034b50) {
			printf("Not a zip file\n");
			return;
		}

		ConsumeBytes(&header.extractVersion, data, 2);
		ConsumeBytes(&header.bitFlags, data, 2);
		ConsumeBytes(&header.compressionMethod, data, 2);
		ConsumeBytes(&header.loadModFileTime, data, 2);
		ConsumeBytes(&header.loadModFileDate, data, 2);
		ConsumeBytes(&header.crc, data, 4);
		ConsumeBytes(&header.compressedSize, data, 4);
		ConsumeBytes(&header.uncompressedSize, data, 4);
		ConsumeBytes(&header.fileNameLength, data, 2);
		ConsumeBytes(&header.extraFieldLength, data, 2);

		ConsumeBytes(&header.fileName, data, header.fileNameLength);
		ConsumeBytes(&header.extraField, data, header.extraFieldLength);

		printf("Name: %s\n", header.fileName);

		// DataDescriptor descriptor = {};
		// ConsumeBytes(&descriptor.signature, data, 4);
		// ConsumeBytes(&descriptor.compressedSize, data, 4);
		// ConsumeBytes(&descriptor.uncompressedSize, data, 4);

	// uint16 extractVersion;
	// uint16 bitFlags;
	// uint16 compressionMethod;
	// uint16 loadModFileTime;
	// uint16 loadModFileDate;
	// uint crc;
	// uint compressedSize;
	// uint uncompressedSize;
	// uint16 fileNameLength;
	// uint16 extraFieldLength;
		// header->fileNameLength = endian_swap(header->fileNameLength);
		// header->extraFieldLength = endian_swap(header->extraFieldLength);

		// char *fileName = NULL;
		// char *extraField = NULL;

		// memcpy(fileName, data, header->fileNameLength);
		// data += header->fileNameLength;

		// memcpy(extraField, data, header->extraFieldLength);
		// data += header->extraFieldLength;

		// printf("File name: %s\n", fileName);
	}
}

uint endian_swap(uint x) {
	return (x>>24) | ((x>>8) & 0x0000ff00) | ((x<<8) & 0x00ff0000) | (x<<24);
}
