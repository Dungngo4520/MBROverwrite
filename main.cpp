#include <stdio.h>
#include <Windows.h>

#define SECTOR_SIZE 512

VOID printHexSector(PCHAR hex);
/*
	Copy the original MBR to second sector
	Write the new MBR to first sector
	The new MBR execute code and load second sector (aka old MBR) to address 0x7C00
	Jump to 0x7C00
*/

int main() {
	HANDLE hOldMBR = INVALID_HANDLE_VALUE;
	HANDLE hBootloader = INVALID_HANDLE_VALUE;
	PWCHAR fileName = L"bootloader.bin";
	PWCHAR MBRpath = L"\\\\.\\PhysicalDrive0";

	hOldMBR = CreateFileW(MBRpath, GENERIC_ALL, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

	if (hOldMBR == INVALID_HANDLE_VALUE) {
		printf("Failed to get MBR handle. %d\n", GetLastError());
		return 1;
	}

	hBootloader = CreateFileW(fileName, GENERIC_ALL, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

	if (hBootloader == INVALID_HANDLE_VALUE) {
		printf("Failed to get file handle. %d\n", GetLastError());
		return 1;
	}

	DWORD fileSize = GetFileSize(hBootloader, NULL);
	printf("FileSize : %d\n", fileSize);
	DWORD fileSizeAlign = (fileSize + SECTOR_SIZE - 1) / SECTOR_SIZE * SECTOR_SIZE;

	PCHAR oldMBR = (PCHAR)calloc(SECTOR_SIZE, sizeof(CHAR));
	PCHAR newMBR = (PCHAR)calloc(fileSizeAlign, sizeof(CHAR));

	if (oldMBR && newMBR) {
		DWORD dwByteRead = 0;
		DWORD dwByteWrite = 0;

		// read old MBR
		if (!ReadFile(hOldMBR, oldMBR, SECTOR_SIZE, &dwByteRead, NULL)) {
			printf("Failed to read mbr. %d\n", GetLastError());
			return 1;
		}
		//printHexSector(oldMBR);

		//// read old MBR
		//if (!ReadFile(hOldMBR, oldMBR, SECTOR_SIZE, &dwByteRead, NULL)) {
		//	printf("Failed to read mbr. %d\n", GetLastError());
		//	return 1;
		//}
		//printHexSector(oldMBR);

		// read new MBR
		if (!ReadFile(hBootloader, newMBR, fileSize, &dwByteRead, NULL)) {
			printf("Failed to read mbr. %d\n", GetLastError());
			return 1;
		}

		// write old MBR to third sector
		SetFilePointer(hOldMBR, SECTOR_SIZE*2, NULL, FILE_BEGIN);
		if (!WriteFile(hOldMBR, oldMBR, SECTOR_SIZE, &dwByteWrite, NULL)) {
			printf("Failed to write to mbr. %d\n", GetLastError());
			return 1;
		}

		memcpy(newMBR + 440, oldMBR + 440, SECTOR_SIZE - 440);

		// write new MBR to the first sector
		SetFilePointer(hOldMBR, 0, NULL, FILE_BEGIN);
		if (!WriteFile(hOldMBR, newMBR, fileSizeAlign, &dwByteWrite, NULL)) {
			printf("Failed to write to mbr. %d\n", GetLastError());
			return 1;
		}
	}

	system("pause");
}

VOID printHexSector(PCHAR hex) {
	printf("     ");
	for (int i = 0; i < 16; i++) printf("%02x ", i);
	printf("\n\n");
	for (int i = 0; i < 32; i++) {
		printf("%02x   ", i);
		for (int t = 0; t < 16; t++) {
			printf("%02x ", hex[i * 16 + t] & 0xff);
			if (t == 15)printf("\n");
		}
	}
}