#pragma once

// From ntdef.h
typedef struct _UNICODE_STRING {
	USHORT Length;
	USHORT MaximumLength;
	PWCH   Buffer;
} UNICODE_STRING;
typedef UNICODE_STRING* PUNICODE_STRING;

typedef struct _OBJECT_ATTRIBUTES {
	ULONG Length;
	HANDLE RootDirectory;
	PUNICODE_STRING ObjectName;
	ULONG Attributes;
	PVOID SecurityDescriptor;        // Points to type SECURITY_DESCRIPTOR
	PVOID SecurityQualityOfService;  // Points to type SECURITY_QUALITY_OF_SERVICE
} OBJECT_ATTRIBUTES;
typedef OBJECT_ATTRIBUTES* POBJECT_ATTRIBUTES;

#define OBJ_CASE_INSENSITIVE         0x00000040

#define FILE_DIRECTORY_FILE          0x00000001
#define FILE_SYNCHRONOUS_IO_NONALERT 0x00000020
#define FILE_OPEN_FOR_BACKUP_INTENT  0x00004000

#define FILE_OPEN                    0x00000001

typedef struct _FILE_ID_BOTH_DIR_INFORMATION {
	ULONG NextEntryOffset;
	ULONG FileIndex;
	LARGE_INTEGER CreationTime;
	LARGE_INTEGER LastAccessTime;
	LARGE_INTEGER LastWriteTime;
	LARGE_INTEGER ChangeTime;
	LARGE_INTEGER EndOfFile;
	LARGE_INTEGER AllocationSize;
	ULONG FileAttributes;
	ULONG FileNameLength;
	ULONG EaSize;
	CCHAR ShortNameLength;
	WCHAR ShortName[12];
	LARGE_INTEGER FileId;
	_Field_size_bytes_(FileNameLength) WCHAR FileName[1];
} FILE_ID_BOTH_DIR_INFORMATION, * PFILE_ID_BOTH_DIR_INFORMATION;

typedef enum _FILE_INFORMATION_CLASS {
	FileDirectoryInformation = 1,
	FileFullDirectoryInformation,                   // 2
	FileBothDirectoryInformation,                   // 3
	FileBasicInformation,                           // 4
	FileStandardInformation,                        // 5
	FileCompressionInformation = 28,
	FileIdBothDirectoryInformation = 37
} FILE_INFORMATION_CLASS, * PFILE_INFORMATION_CLASS;

// From wdm.h
typedef struct _IO_STATUS_BLOCK {
	union {
		NTSTATUS Status;
		PVOID Pointer;
	} DUMMYUNIONNAME;
	ULONG_PTR Information;
} IO_STATUS_BLOCK, * PIO_STATUS_BLOCK;

typedef
VOID
(NTAPI* PIO_APC_ROUTINE) (
	_In_ PVOID ApcContext,
	_In_ PIO_STATUS_BLOCK IoStatusBlock,
	_In_ ULONG Reserved
	);

typedef NTSTATUS(WINAPI* PNtCreateFile)(HANDLE, ACCESS_MASK, POBJECT_ATTRIBUTES, PIO_STATUS_BLOCK, PLARGE_INTEGER, ULONG, ULONG, ULONG, ULONG, PVOID, ULONG);
typedef NTSTATUS(WINAPI* PNtQueryDirectoryFile)(HANDLE, HANDLE, PIO_APC_ROUTINE, PVOID, PIO_STATUS_BLOCK, PVOID, ULONG, FILE_INFORMATION_CLASS, BOOLEAN, PUNICODE_STRING, BOOLEAN);

// From Cygwin-msys2-3_1_6-release\winsup\cygwin\ntdll.h

/* Like RtlInitEmptyUnicodeString, but initialize Length to len, too.
   This is for instance useful when creating a UNICODE_STRING from an
   NtQueryInformationFile info buffer, where the length of the filename
   is known, but you can't rely on the string being 0-terminated.
   If you know it's 0-terminated, just use RtlInitUnicodeString(). */
inline
VOID NTAPI RtlInitCountedUnicodeString(PUNICODE_STRING dest, PCWSTR buf, USHORT len)
{
	dest->Length = dest->MaximumLength = len;
	dest->Buffer = (PWSTR)buf;
}
/* Split path into dirname and basename part.  This function does not
	 copy anything!  It just initializes the dirname and basename
	 UNICODE_STRINGs so that their Buffer members point to the right spot
	 into path's Buffer, and the Length (and MaximumLength) members are set
	 to match the dirname part and the basename part.
	 Note that dirname's Length is set so that it also includes the trailing
	 backslash.  If you don't need it, just subtract sizeof(WCHAR) from
	 dirname.Length. */
inline
VOID NTAPI RtlSplitUnicodePath(PUNICODE_STRING path, PUNICODE_STRING dirname, PUNICODE_STRING basename)
{
	USHORT len = path->Length / sizeof(WCHAR);
	while (len > 0 && path->Buffer[--len] != L'\\')
		;
	++len;
	if (dirname)
		RtlInitCountedUnicodeString(dirname, path->Buffer, len * sizeof(WCHAR));
	if (basename)
		RtlInitCountedUnicodeString(basename, &path->Buffer[len],
			path->Length - len * sizeof(WCHAR));
}

static PNtCreateFile NtCreateFile;
static PNtQueryDirectoryFile NtQueryDirectoryFile;