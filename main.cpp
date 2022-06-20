#include <stdio.h>
#include <Windows.h>

int main() {
	HANDLE hMbr = INVALID_HANDLE_VALUE;
	HANDLE hBootloader = INVALID_HANDLE_VALUE;
	PCHAR fileName = "bootloader.bin";

	hMbr = CreateFileA("\\\\.\\PhysicalDrive0", GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

	if (hMbr == INVALID_HANDLE_VALUE) {
		printf("Failed to get MBR handle. %d\n", GetLastError());
		return 1;
	}

	hBootloader = CreateFileA(fileName, GENERIC_ALL, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

	if (hBootloader == INVALID_HANDLE_VALUE) {
		printf("Failed to get MBR handle. %d\n", GetLastError());
		return 1;
	}

	DWORD fileSize = GetFileSize(hBootloader, NULL);
	if (fileSize != 512) {
		printf("bootloader size not 512.\n");
		return 1;
	}

	PCHAR buffer = (PCHAR)calloc(BUFSIZ, sizeof(CHAR));
	if (buffer) {
		DWORD dwByteRead = 0;
		if (ReadFile(hBootloader, buffer, 512, &dwByteRead, NULL)) {
			if (!WriteFile(hMbr, buffer, dwByteRead, &dwByteRead, NULL)) {
				printf("Failed to write to mbr. %d\n", GetLastError());
				return 1;
			}
		}
		else {
			printf("Failed to read file. %d\n", GetLastError());
			return 1;
		}
	}

	system("pause");
}