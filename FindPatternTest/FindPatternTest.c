// FindPatternTest.cpp : Defines the entry point for the console application.
//

#include <Windows.h>
#include <assert.h>
#include <tchar.h>
#include <strsafe.h>
#include "findpattern.h"

const UINT8 sigBlInitializeLibrary[] = { 0x40, 0x53, 0x48, 0x83, 0xEC, 0x20, 0x8B, 0x1A };

BOOLEAN ImageLoad( IN PWSTR ModulePath, OUT UINT8** ImageBase, OUT UINT32* ImageSize )
{
	HANDLE FileHandle = NULL;
	UINT32 FileSize = 0;
	VOID* FileBytes = NULL;
	ULONG BytesRead = 0;

	assert( ImageBase != NULL );

	FileHandle = CreateFileW( ModulePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	if (FileHandle == INVALID_HANDLE_VALUE)
	{
		_tprintf( TEXT( "Terminal failure: Unable to open file \"%s\" for read.\n" ), ModulePath );
		return FALSE;
	}

	if (GetFileAttributesW( ModulePath ) & FILE_ATTRIBUTE_COMPRESSED)
	{
		FileSize = GetCompressedFileSizeW( ModulePath, NULL );
	}
	else
	{
		FileSize = GetFileSize( FileHandle, NULL );
	}

	if (FileSize == 0)
	{
		_tprintf( TEXT( "Terminal failure: Unable to get file size for file \"%s\".\r\n" ), ModulePath );
		CloseHandle( FileHandle );
		return FALSE;
	}

	FileBytes = (UINT8*)LocalAlloc( LPTR, FileSize );
	if (FileBytes == NULL)
	{
		_tprintf( TEXT( "Terminal failure: Failed to allocate image bytes of file \"%s\".\r\n" ), ModulePath );
		CloseHandle( FileHandle );
		return FALSE;
	}

	*ImageBase = FileBytes;
	*ImageSize = FileSize;

	if (ReadFile( FileHandle, FileBytes, FileSize, &BytesRead, NULL) == FALSE)
	{
		_tprintf( TEXT( "Terminal failure: Unable to read from file. Error %08X\r\n" ), GetLastError( ) );
		CloseHandle( FileHandle );
		return FALSE;
	}

	CloseHandle( FileHandle );

	return TRUE;
}

BOOLEAN ImageFree( IN UINT8* ImageBase )
{
	if (ImageBase)
		LocalFree( ImageBase );
	return TRUE;
}

VOID SpewBytes( UINT8* Bytes, UINT32 Size )
{
	for (UINT32 i = 1; i <= Size; i++)
		_tprintf( (i % 16 == 0) ? L"%02X\r\n" : (i % 8 == 0) ? L"%02X  " : L"%02X ", Bytes[i - 1] );
}

int main()
{
	VOID* Found = NULL;
	UINT8* ImageBase = NULL;
	UINT32 ImageSize = 0;

	if (ImageLoad( L"C:\\Users\\Owner\\Desktop\\ntdll\\winload.efi", &ImageBase, &ImageSize ) == TRUE)
	{
		Found = FindPattern( ImageBase, ImageSize, sigBlInitializeLibrary, sizeof( sigBlInitializeLibrary ) );
	}

	SpewBytes( (UINT8*)Found, 64 );

	ImageFree( ImageBase );

    return 0;
}

