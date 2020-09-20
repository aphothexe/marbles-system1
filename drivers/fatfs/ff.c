/*----------------------------------------------------------------------------/
/  FatFs - Generic FAT Filesystem Module  R0.14b                              /
/-----------------------------------------------------------------------------/
/
/ Copyright (C) 2021, ChaN, all right reserved.
/
/ FatFs module is an open source software. Redistribution and use of FatFs in
/ source and binary forms, with or without modification, are permitted provided
/ that the following condition is met:
/
/ 1. Redistributions of source code must retain the above copyright notice,
/    this condition and the following disclaimer.
/
/ This software is provided by the copyright holder and contributors "AS IS"
/ and any warranties related to this software are DISCLAIMED.
/ The copyright owner or contributors be NOT LIABLE for any damages caused
/ by use of this software.
/
/----------------------------------------------------------------------------*/


#include <string.h>
#include "ff.h"			/* Declarations of FatFs API */
#include "diskio.h"		/* Declarations of device I/O functions */


/*--------------------------------------------------------------------------

   Module Private Definitions

---------------------------------------------------------------------------*/

#if FF_DEFINED != 86631	/* Revision ID */
#error Wrong include file (ff.h).
#endif


/* Limits and boundaries */
#define MAX_DIR		0x200000		/* Max size of FAT directory */
#define MAX_DIR_EX	0x10000000		/* Max size of exFAT directory */
#define MAX_FAT12	0xFF5			/* Max FAT12 clusters (differs from specs, but right for real DOS/Windows behavior) */
#define MAX_FAT16	0xFFF5			/* Max FAT16 clusters (differs from specs, but right for real DOS/Windows behavior) */
#define MAX_FAT32	0x0FFFFFF5		/* Max FAT32 clusters (not specified, practical limit) */
#define MAX_EXFAT	0x7FFFFFFD		/* Max exFAT clusters (differs from specs, implementation limit) */


/* Character code support macros */
#define IsUpper(c)		((c) >= 'A' && (c) <= 'Z')
#define IsLower(c)		((c) >= 'a' && (c) <= 'z')
#define IsDigit(c)		((c) >= '0' && (c) <= '9')
#define IsSeparator(c)	((c) == '/' || (c) == '\\')
#define IsTerminator(c)	((UINT)(c) < (FF_USE_LFN ? ' ' : '!'))
#define IsSurrogate(c)	((c) >= 0xD800 && (c) <= 0xDFFF)
#define IsSurrogateH(c)	((c) >= 0xD800 && (c) <= 0xDBFF)
#define IsSurrogateL(c)	((c) >= 0xDC00 && (c) <= 0xDFFF)


/* Additional file access control and file status flags for internal use */
#define FA_SEEKEND	0x20	/* Seek to end of the file on file open */
#define FA_MODIFIED	0x40	/* File has been modified */
#define FA_DIRTY	0x80	/* FIL.buf[] needs to be written-back */


/* Additional file attribute bits for internal use */
#define AM_VOL		0x08	/* Volume label */
#define AM_LFN		0x0F	/* LFN entry */
#define AM_MASK		0x3F	/* Mask of defined bits in FAT */
#define AM_MASKX	0x37	/* Mask of defined bits in exFAT */


/* Name status flags in fn[11] */
#define NSFLAG		11		/* Index of the name status byte */
#define NS_LOSS		0x01	/* Out of 8.3 format */
#define NS_LFN		0x02	/* Force to create LFN entry */
#define NS_LAST		0x04	/* Last segment */
#define NS_BODY		0x08	/* Lower case flag (body) */
#define NS_EXT		0x10	/* Lower case flag (ext) */
#define NS_DOT		0x20	/* Dot entry */
#define NS_NOLFN	0x40	/* Do not find LFN */
#define NS_NONAME	0x80	/* Not followed */


/* exFAT directory entry types */
#define	ET_BITMAP	0x81	/* Allocation bitmap */
#define	ET_UPCASE	0x82	/* Up-case table */
#define	ET_VLABEL	0x83	/* Volume label */
#define	ET_FILEDIR	0x85	/* File and directory */
#define	ET_STREAM	0xC0	/* Stream extension */
#define	ET_FILENAME	0xC1	/* Name extension */


/* FatFs refers the FAT structure as simple byte array instead of structure member
/ because the C structure is not binary compatible between different platforms */

#define BS_JmpBoot			0		/* x86 jump instruction (3-byte) */
#define BS_OEMName			3		/* OEM name (8-byte) */
#define BPB_BytsPerSec		11		/* Sector size [byte] (WORD) */
#define BPB_SecPerClus		13		/* Cluster size [sector] (BYTE) */
#define BPB_RsvdSecCnt		14		/* Size of reserved area [sector] (WORD) */
#define BPB_NumFATs			16		/* Number of FATs (BYTE) */
#define BPB_RootEntCnt		17		/* Size of root directory area for FAT [entry] (WORD) */
#define BPB_TotSec16		19		/* Volume size (16-bit) [sector] (WORD) */
#define BPB_Media			21		/* Media descriptor byte (BYTE) */
#define BPB_FATSz16			22		/* FAT size (16-bit) [sector] (WORD) */
#define BPB_SecPerTrk		24		/* Number of sectors per track for int13h [sector] (WORD) */
#define BPB_NumHeads		26		/* Number of heads for int13h (WORD) */
#define BPB_HiddSec			28		/* Volume offset from top of the drive (DWORD) */
#define BPB_TotSec32		32		/* Volume size (32-bit) [sector] (DWORD) */
#define BS_DrvNum			36		/* Physical drive number for int13h (BYTE) */
#define BS_NTres			37		/* WindowsNT error flag (BYTE) */
#define BS_BootSig			38		/* Extended boot signature (BYTE) */
#define BS_VolID			39		/* Volume serial number (DWORD) */
#define BS_VolLab			43		/* Volume label string (8-byte) */
#define BS_FilSysType		54		/* Filesystem type string (8-byte) */
#define BS_BootCode			62		/* Boot code (448-byte) */
#define BS_55AA				510		/* Signature word (WORD) */

#define BPB_FATSz32			36		/* FAT32: FAT size [sector] (DWORD) */
#define BPB_ExtFlags32		40		/* FAT32: Extended flags (WORD) */
#define BPB_FSVer32			42		/* FAT32: Filesystem version (WORD) */
#define BPB_RootClus32		44		/* FAT32: Root directory cluster (DWORD) */
#define BPB_FSInfo32		48		/* FAT32: Offset of FSINFO sector (WORD) */
#define BPB_BkBootSec32		50		/* FAT32: Offset of backup boot sector (WORD) */
#define BS_DrvNum32			64		/* FAT32: Physical drive number for int13h (BYTE) */
#define BS_NTres32			65		/* FAT32: Error flag (BYTE) */
#define BS_BootSig32		66		/* FAT32: Extended boot signature (BYTE) */
#define BS_VolID32			67		/* FAT32: Volume serial number (DWORD) */
#define BS_VolLab32			71		/* FAT32: Volume label string (8-byte) */
#define BS_FilSysType32		82		/* FAT32: Filesystem type string (8-byte) */
#define BS_BootCode32		90		/* FAT32: Boot code (420-byte) */

#define BPB_ZeroedEx		11		/* exFAT: MBZ field (53-byte) */
#define BPB_VolOfsEx		64		/* exFAT: Volume offset from top of the drive [sector] (QWORD) */
#define BPB_TotSecEx		72		/* exFAT: Volume size [sector] (QWORD) */
#define BPB_FatOfsEx		80		/* exFAT: FAT offset from top of the volume [sector] (DWORD) */
#define BPB_FatSzEx			84		/* exFAT: FAT size [sector] (DWORD) */
#define BPB_DataOfsEx		88		/* exFAT: Data offset from top of the volume [sector] (DWORD) */
#define BPB_NumClusEx		92		/* exFAT: Number of clusters (DWORD) */
#define BPB_RootClusEx		96		/* exFAT: Root directory start cluster (DWORD) */
#define BPB_VolIDEx			100		/* exFAT: Volume serial number (DWORD) */
#define BPB_FSVerEx			104		/* exFAT: Filesystem version (WORD) */
#define BPB_VolFlagEx		106		/* exFAT: Volume flags (WORD) */
#define BPB_BytsPerSecEx	108		/* exFAT: Log2 of sector size in unit of byte (BYTE) */
#define BPB_SecPerClusEx	109		/* exFAT: Log2 of cluster size in unit of sector (BYTE) */
#define BPB_NumFATsEx		110		/* exFAT: Number of FATs (BYTE) */
#define BPB_DrvNumEx		111		/* exFAT: Physical drive number for int13h (BYTE) */
#define BPB_PercInUseEx		112		/* exFAT: Percent in use (BYTE) */
#define BPB_RsvdEx			113		/* exFAT: Reserved (7-byte) */
#define BS_BootCodeEx		120		/* exFAT: Boot code (390-byte) */

#define DIR_Name			0		/* Short file name (11-byte) */
#define DIR_Attr			11		/* Attribute (BYTE) */
#define DIR_NTres			12		/* Lower case flag (BYTE) */
#define DIR_CrtTime10		13		/* Created time sub-second (BYTE) */
#define DIR_CrtTime			14		/* Created time (DWORD) */
#define DIR_LstAccDate		18		/* Last accessed date (WORD) */
#define DIR_FstClusHI		20		/* Higher 16-bit of first cluster (WORD) */
#define DIR_ModTime			22		/* Modified time (DWORD) */
#define DIR_FstClusLO		26		/* Lower 16-bit of first cluster (WORD) */
#define DIR_FileSize		28		/* File size (DWORD) */
#define LDIR_Ord			0		/* LFN: LFN order and LLE flag (BYTE) */
#define LDIR_Attr			11		/* LFN: LFN attribute (BYTE) */
#define LDIR_Type			12		/* LFN: Entry type (BYTE) */
#define LDIR_Chksum			13		/* LFN: Checksum of the SFN (BYTE) */
#define LDIR_FstClusLO		26		/* LFN: MBZ field (WORD) */
#define XDIR_Type			0		/* exFAT: Type of exFAT directory entry (BYTE) */
#define XDIR_NumLabel		1		/* exFAT: Number of volume label characters (BYTE) */
#define XDIR_Label			2		/* exFAT: Volume label (11-WORD) */
#define XDIR_CaseSum		4		/* exFAT: Sum of case conversion table (DWORD) */
#define XDIR_NumSec			1		/* exFAT: Number of secondary entries (BYTE) */
#define XDIR_SetSum			2		/* exFAT: Sum of the set of directory entries (WORD) */
#define XDIR_Attr			4		/* exFAT: File attribute (WORD) */
#define XDIR_CrtTime		8		/* exFAT: Created time (DWORD) */
#define XDIR_ModTime		12		/* exFAT: Modified time (DWORD) */
#define XDIR_AccTime		16		/* exFAT: Last accessed time (DWORD) */
#define XDIR_CrtTime10		20		/* exFAT: Created time subsecond (BYTE) */
#define XDIR_ModTime10		21		/* exFAT: Modified time subsecond (BYTE) */
#define XDIR_CrtTZ			22		/* exFAT: Created timezone (BYTE) */
#define XDIR_ModTZ			23		/* exFAT: Modified timezone (BYTE) */
#define XDIR_AccTZ			24		/* exFAT: Last accessed timezone (BYTE) */
#define XDIR_GenFlags		33		/* exFAT: General secondary flags (BYTE) */
#define XDIR_NumName		35		/* exFAT: Number of file name characters (BYTE) */
#define XDIR_NameHash		36		/* exFAT: Hash of file name (WORD) */
#define XDIR_ValidFileSize	40		/* exFAT: Valid file size (QWORD) */
#define XDIR_FstClus		52		/* exFAT: First cluster of the file data (DWORD) */
#define XDIR_FileSize		56		/* exFAT: File/Directory size (QWORD) */

#define SZDIRE				32		/* Size of a directory entry */
#define DDEM				0xE5	/* Deleted directory entry mark set to DIR_Name[0] */
#define RDDEM				0x05	/* Replacement of the character collides with DDEM */
#define LLEF				0x40	/* Last long entry flag in LDIR_Ord */

#define FSI_LeadSig			0		/* FAT32 FSI: Leading signature (DWORD) */
#define FSI_StrucSig		484		/* FAT32 FSI: Structure signature (DWORD) */
#define FSI_Free_Count		488		/* FAT32 FSI: Number of free clusters (DWORD) */
#define FSI_Nxt_Free		492		/* FAT32 FSI: Last allocated cluster (DWORD) */

#define MBR_Table			446		/* MBR: Offset of partition table in the MBR */
#define SZ_PTE				16		/* MBR: Size of a partition table entry */
#define PTE_Boot			0		/* MBR PTE: Boot indicator */
#define PTE_StHead			1		/* MBR PTE: Start head */
#define PTE_StSec			2		/* MBR PTE: Start sector */
#define PTE_StCyl			3		/* MBR PTE: Start cylinder */
#define PTE_System			4		/* MBR PTE: System ID */
#define PTE_EdHead			5		/* MBR PTE: End head */
#define PTE_EdSec			6		/* MBR PTE: End sector */
#define PTE_EdCyl			7		/* MBR PTE: End cylinder */
#define PTE_StLba			8		/* MBR PTE: Start in LBA */
#define PTE_SizLba			12		/* MBR PTE: Size in LBA */

#define GPTH_Sign			0		/* GPT: Header signature (8-byte) */
#define GPTH_Rev			8		/* GPT: Revision (DWORD) */
#define GPTH_Size			12		/* GPT: Header size (DWORD) */
#define GPTH_Bcc			16		/* GPT: Header BCC (DWORD) */
#define GPTH_CurLba			24		/* GPT: Main header LBA (QWORD) */
#define GPTH_BakLba			32		/* GPT: Backup header LBA (QWORD) */
#define GPTH_FstLba			40		/* GPT: First LBA for partitions (QWORD) */
#define GPTH_LstLba			48		/* GPT: Last LBA for partitions (QWORD) */
#define GPTH_DskGuid		56		/* GPT: Disk GUID (16-byte) */
#define GPTH_PtOfs			72		/* GPT: Partation table LBA (QWORD) */
#define GPTH_PtNum			80		/* GPT: Number of table entries (DWORD) */
#define GPTH_PteSize		84		/* GPT: Size of table entry (DWORD) */
#define GPTH_PtBcc			88		/* GPT: Partation table BCC (DWORD) */
#define SZ_GPTE				128		/* GPT: Size of partition table entry */
#define GPTE_PtGuid			0		/* GPT PTE: Partition type GUID (16-byte) */
#define GPTE_UpGuid			16		/* GPT PTE: Partition unique GUID (16-byte) */
#define GPTE_FstLba			32		/* GPT PTE: First LBA (QWORD) */
#define GPTE_LstLba			40		/* GPT PTE: Last LBA inclusive (QWORD) */
#define GPTE_Flags			48		/* GPT PTE: Flags (QWORD) */
#define GPTE_Name			56		/* GPT PTE: Name */


/* Post process on fatal error in the file operations */
#define ABORT(fs, res)		{ fp->err = (BYTE)(res); LEAVE_FF(fs, res); }


/* Re-entrancy related */
#if FF_FS_REENTRANT
#if FF_USE_LFN == 1
#error Static LFN work area cannot be used in thread-safe configuration
#endif
#define LEAVE_FF(fs, res)	{ unlock_fs(fs, res); return res; }
#else
#define LEAVE_FF(fs, res)	return res
#endif


/* Definitions of logical drive - physical location conversion */
#if FF_MULTI_PARTITION
#define LD2PD(vol) VolToPart[vol].pd	/* Get physical drive number */
#define LD2PT(vol) VolToPart[vol].pt	/* Get partition number (0:auto search, 1..:forced partition number) */
#else
#define LD2PD(vol) (BYTE)(vol)	/* Each logical drive is associated with the same physical drive number */
#define LD2PT(vol) 0			/* Auto partition search */
#endif


/* Definitions of sector size */
#if (FF_MAX_SS < FF_MIN_SS) || (FF_MAX_SS != 512 && FF_MAX_SS != 1024 && FF_MAX_SS != 2048 && FF_MAX_SS != 4096) || (FF_MIN_SS != 512 && FF_MIN_SS != 1024 && FF_MIN_SS != 2048 && FF_MIN_SS != 4096)
#error Wrong sector size configuration
#endif
#if FF_MAX_SS == FF_MIN_SS
#define SS(fs)	((UINT)FF_MAX_SS)	/* Fixed sector size */
#else
#define SS(fs)	((fs)->ssize)	/* Variable sector size */
#endif


/* Timestamp */
#if FF_FS_NORTC == 1
#if FF_NORTC_YEAR < 1980 || FF_NORTC_YEAR > 2107 || FF_NORTC_MON < 1 || FF_NORTC_MON > 12 || FF_NORTC_MDAY < 1 || FF_NORTC_MDAY > 31
#error Invalid FF_FS_NORTC settings
#endif
#define GET_FATTIME()	((DWORD)(FF_NORTC_YEAR - 1980) << 25 | (DWORD)FF_NORTC_MON << 21 | (DWORD)FF_NORTC_MDAY << 16)
#else
#define GET_FATTIME()	get_fattime()
#endif


/* File lock controls */
#if FF_FS_LOCK != 0
#if FF_FS_READONLY
#error FF_FS_LOCK must be 0 at read-only configuration
#endif
typedef struct {
	FATFS *fs;		/* Object ID 1, volume (NULL:blank entry) */
	DWORD clu;		/* Object ID 2, containing directory (0:root) */
	DWORD ofs;		/* Object ID 3, offset in the directory */
	WORD ctr;		/* Object open counter, 0:none, 0x01..0xFF:read mode open count, 0x100:write mode */
} FILESEM;
#endif


/* SBCS up-case tables (\x80-\xFF) */
#define TBL_CT437  {0x80,0x9A,0x45,0x41,0x8E,0x41,0x8F,0x80,0x45,0x45,0x45,0x49,0x49,0x49,0x8E,0x8F, \
					0x90,0x92,0x92,0x4F,0x99,0x4F,0x55,0x55,0x59,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F, \
					0x41,0x49,0x4F,0x55,0xA5,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF, \
					0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
					0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF, \
					0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
					0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF, \
					0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF}
#define TBL_CT720  {0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F, \
					0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F, \
					0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF, \
					0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
					0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF, \
					0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
					0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF, \
					0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF}
#define TBL_CT737  {0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F, \
					0x90,0x92,0x92,0x93,0x94,0x95,0x96,0x97,0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87, \
					0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,0x90,0x91,0xAA,0x92,0x93,0x94,0x95,0x96, \
					0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
					0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF, \
					0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
					0x97,0xEA,0xEB,0xEC,0xE4,0xED,0xEE,0xEF,0xF5,0xF0,0xEA,0xEB,0xEC,0xED,0xEE,0xEF, \
					0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF}
#define TBL_CT771  {0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F, \
					0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F, \
					0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F, \
					0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
					0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF, \
					0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDC,0xDE,0xDE, \
					0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F, \
					0xF0,0xF0,0xF2,0xF2,0xF4,0xF4,0xF6,0xF6,0xF8,0xF8,0xFA,0xFA,0xFC,0xFC,0xFE,0xFF}
#define TBL_CT775  {0x80,0x9A,0x91,0xA0,0x8E,0x95,0x8F,0x80,0xAD,0xED,0x8A,0x8A,0xA1,0x8D,0x8E,0x8F, \
					0x90,0x92,0x92,0xE2,0x99,0x95,0x96,0x97,0x97,0x99,0x9A,0x9D,0x9C,0x9D,0x9E,0x9F, \
					0xA0,0xA1,0xE0,0xA3,0xA3,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF, \
					0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
					0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF, \
					0xB5,0xB6,0xB7,0xB8,0xBD,0xBE,0xC6,0xC7,0xA5,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
					0xE0,0xE1,0xE2,0xE3,0xE5,0xE5,0xE6,0xE3,0xE8,0xE8,0xEA,0xEA,0xEE,0xED,0xEE,0xEF, \
					0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF}
#define TBL_CT850  {0x43,0x55,0x45,0x41,0x41,0x41,0x41,0x43,0x45,0x45,0x45,0x49,0x49,0x49,0x41,0x41, \
					0x45,0x92,0x92,0x4F,0x4F,0x4F,0x55,0x55,0x59,0x4F,0x55,0x4F,0x9C,0x4F,0x9E,0x9F, \
					0x41,0x49,0x4F,0x55,0xA5,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF, \
					0xB0,0xB1,0xB2,0xB3,0xB4,0x41,0x41,0x41,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
					0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0x41,0x41,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF, \
					0xD1,0xD1,0x45,0x45,0x45,0x49,0x49,0x49,0x49,0xD9,0xDA,0xDB,0xDC,0xDD,0x49,0xDF, \
					0x4F,0xE1,0x4F,0x4F,0x4F,0x4F,0xE6,0xE8,0xE8,0x55,0x55,0x55,0x59,0x59,0xEE,0xEF, \
					0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF}
#define TBL_CT852  {0x80,0x9A,0x90,0xB6,0x8E,0xDE,0x8F,0x80,0x9D,0xD3,0x8A,0x8A,0xD7,0x8D,0x8E,0x8F, \
					0x90,0x91,0x91,0xE2,0x99,0x95,0x95,0x97,0x97,0x99,0x9A,0x9B,0x9B,0x9D,0x9E,0xAC, \
					0xB5,0xD6,0xE0,0xE9,0xA4,0xA4,0xA6,0xA6,0xA8,0xA8,0xAA,0x8D,0xAC,0xB8,0xAE,0xAF, \
					0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBD,0xBF, \
					0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC6,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF, \
					0xD1,0xD1,0xD2,0xD3,0xD2,0xD5,0xD6,0xD7,0xB7,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
					0xE0,0xE1,0xE2,0xE3,0xE3,0xD5,0xE6,0xE6,0xE8,0xE9,0xE8,0xEB,0xED,0xED,0xDD,0xEF, \
					0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xEB,0xFC,0xFC,0xFE,0xFF}
#define TBL_CT855  {0x81,0x81,0x83,0x83,0x85,0x85,0x87,0x87,0x89,0x89,0x8B,0x8B,0x8D,0x8D,0x8F,0x8F, \
					0x91,0x91,0x93,0x93,0x95,0x95,0x97,0x97,0x99,0x99,0x9B,0x9B,0x9D,0x9D,0x9F,0x9F, \
					0xA1,0xA1,0xA3,0xA3,0xA5,0xA5,0xA7,0xA7,0xA9,0xA9,0xAB,0xAB,0xAD,0xAD,0xAE,0xAF, \
					0xB0,0xB1,0xB2,0xB3,0xB4,0xB6,0xB6,0xB8,0xB8,0xB9,0xBA,0xBB,0xBC,0xBE,0xBE,0xBF, \
					0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC7,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF, \
					0xD1,0xD1,0xD3,0xD3,0xD5,0xD5,0xD7,0xD7,0xDD,0xD9,0xDA,0xDB,0xDC,0xDD,0xE0,0xDF, \
					0xE0,0xE2,0xE2,0xE4,0xE4,0xE6,0xE6,0xE8,0xE8,0xEA,0xEA,0xEC,0xEC,0xEE,0xEE,0xEF, \
					0xF0,0xF2,0xF2,0xF4,0xF4,0xF6,0xF6,0xF8,0xF8,0xFA,0xFA,0xFC,0xFC,0xFD,0xFE,0xFF}
#define TBL_CT857  {0x80,0x9A,0x90,0xB6,0x8E,0xB7,0x8F,0x80,0xD2,0xD3,0xD4,0xD8,0xD7,0x49,0x8E,0x8F, \
					0x90,0x92,0x92,0xE2,0x99,0xE3,0xEA,0xEB,0x98,0x99,0x9A,0x9D,0x9C,0x9D,0x9E,0x9E, \
					0xB5,0xD6,0xE0,0xE9,0xA5,0xA5,0xA6,0xA6,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF, \
					0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
					0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC7,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF, \
					0xD0,0xD1,0xD2,0xD3,0xD4,0x49,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
					0xE0,0xE1,0xE2,0xE3,0xE5,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xEB,0xDE,0xED,0xEE,0xEF, \
					0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF}
#define TBL_CT860  {0x80,0x9A,0x90,0x8F,0x8E,0x91,0x86,0x80,0x89,0x89,0x92,0x8B,0x8C,0x98,0x8E,0x8F, \
					0x90,0x91,0x92,0x8C,0x99,0xA9,0x96,0x9D,0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F, \
					0x86,0x8B,0x9F,0x96,0xA5,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF, \
					0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
					0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF, \
					0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
					0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF, \
					0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF}
#define TBL_CT861  {0x80,0x9A,0x90,0x41,0x8E,0x41,0x8F,0x80,0x45,0x45,0x45,0x8B,0x8B,0x8D,0x8E,0x8F, \
					0x90,0x92,0x92,0x4F,0x99,0x8D,0x55,0x97,0x97,0x99,0x9A,0x9D,0x9C,0x9D,0x9E,0x9F, \
					0xA4,0xA5,0xA6,0xA7,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF, \
					0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
					0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF, \
					0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
					0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF, \
					0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF}
#define TBL_CT862  {0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F, \
					0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F, \
					0x41,0x49,0x4F,0x55,0xA5,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF, \
					0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
					0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF, \
					0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
					0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF, \
					0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF}
#define TBL_CT863  {0x43,0x55,0x45,0x41,0x41,0x41,0x86,0x43,0x45,0x45,0x45,0x49,0x49,0x8D,0x41,0x8F, \
					0x45,0x45,0x45,0x4F,0x45,0x49,0x55,0x55,0x98,0x4F,0x55,0x9B,0x9C,0x55,0x55,0x9F, \
					0xA0,0xA1,0x4F,0x55,0xA4,0xA5,0xA6,0xA7,0x49,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF, \
					0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
					0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF, \
					0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
					0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF, \
					0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF}
#define TBL_CT864  {0x80,0x9A,0x45,0x41,0x8E,0x41,0x8F,0x80,0x45,0x45,0x45,0x49,0x49,0x49,0x8E,0x8F, \
					0x90,0x92,0x92,0x4F,0x99,0x4F,0x55,0x55,0x59,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F, \
					0x41,0x49,0x4F,0x55,0xA5,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF, \
					0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
					0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF, \
					0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
					0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF, \
					0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF}
#define TBL_CT865  {0x80,0x9A,0x90,0x41,0x8E,0x41,0x8F,0x80,0x45,0x45,0x45,0x49,0x49,0x49,0x8E,0x8F, \
					0x90,0x92,0x92,0x4F,0x99,0x4F,0x55,0x55,0x59,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F, \
					0x41,0x49,0x4F,0x55,0xA5,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF, \
					0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
					0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF, \
					0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
					0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF, \
					0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF}
#define TBL_CT866  {0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F, \
					0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F, \
					0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F, \
					0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
					0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF, \
					0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
					0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F, \
					0xF0,0xF0,0xF2,0xF2,0xF4,0xF4,0xF6,0xF6,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF}
#define TBL_CT869  {0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F, \
					0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0x86,0x9C,0x8D,0x8F,0x90, \
					0x91,0x90,0x92,0x95,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF, \
					0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
					0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF, \
					0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xA4,0xA5,0xA6,0xD9,0xDA,0xDB,0xDC,0xA7,0xA8,0xDF, \
					0xA9,0xAA,0xAC,0xAD,0xB5,0xB6,0xB7,0xB8,0xBD,0xBE,0xC6,0xC7,0xCF,0xCF,0xD0,0xEF, \
					0xF0,0xF1,0xD1,0xD2,0xD3,0xF5,0xD4,0xF7,0xF8,0xF9,0xD5,0x96,0x95,0x98,0xFE,0xFF}


/* DBCS code range |----- 1st byte -----|  |----------- 2nd byte -----------| */
/*                  <------>    <------>    <------>    <------>    <------>  */
#define TBL_DC932 {0x81, 0x9F, 0xE0, 0xFC, 0x40, 0x7E, 0x80, 0xFC, 0x00, 0x00}
#define TBL_DC936 {0x81, 0xFE, 0x00, 0x00, 0x40, 0x7E, 0x80, 0xFE, 0x00, 0x00}
#define TBL_DC949 {0x81, 0xFE, 0x00, 0x00, 0x41, 0x5A, 0x61, 0x7A, 0x81, 0xFE}
#define TBL_DC950 {0x81, 0xFE, 0x00, 0x00, 0x40, 0x7E, 0xA1, 0xFE, 0x00, 0x00}


/* Macros for table definitions */
#define MERGE_2STR(a, b) a ## b
#define MKCVTBL(hd, cp) MERGE_2STR(hd, cp)




/*--------------------------------------------------------------------------

   Module Private Work Area

---------------------------------------------------------------------------*/
/* Remark: Variables defined here without initial value shall be guaranteed
/  zero/null at start-up. If not, the linker option or start-up routine is
/  not compliance with C standard. */

/*--------------------------------*/
/* File/Volume controls           */
/*--------------------------------*/

#if FF_VOLUMES < 1 || FF_VOLUMES > 10
#error Wrong FF_VOLUMES setting
#endif
static FATFS* FatFs[FF_VOLUMES];	/* Pointer to the filesystem objects (logical drives) */
static WORD Fsid;					/* Filesystem mount ID */

#if FF_FS_RPATH != 0
static BYTE CurrVol;				/* Current drive */
#endif

#if FF_FS_LOCK != 0
static FILESEM Files[FF_FS_LOCK];	/* Open object lock semaphores */
#endif

#if FF_STR_VOLUME_ID
#ifdef FF_VOLUME_STRS
static const char* const VolumeStr[FF_VOLUMES] = {FF_VOLUME_STRS};	/* Pre-defined volume ID */
#endif
#endif

#if FF_LBA64
#if FF_MIN_GPT > 0x100000000
#error Wrong FF_MIN_GPT setting
#endif
static const BYTE GUID_MS_Basic[16] = {0xA2,0xA0,0xD0,0xEB,0xE5,0xB9,0x33,0x44,0x87,0xC0,0x68,0xB6,0xB7,0x26,0x99,0xC7};
#endif



/*--------------------------------*/
/* LFN/Directory working buffer   */
/*--------------------------------*/

#if FF_USE_LFN == 0		/* Non-LFN configuration */
#if FF_FS_EXFAT
#error LFN must be enabled when enable exFAT
#endif
#define DEF_NAMBUF
#define INIT_NAMBUF(fs)
#define FREE_NAMBUF()
#define LEAVE_MKFS(res)	return res

#else					/* LFN configurations */
#if FF_MAX_LFN < 12 || FF_MAX_LFN > 255
#error Wrong setting of FF_MAX_LFN
#endif
#if FF_LFN_BUF < FF_SFN_BUF || FF_SFN_BUF < 12
#error Wrong setting of FF_LFN_BUF or FF_SFN_BUF
#endif
#if FF_LFN_UNICODE < 0 || FF_LFN_UNICODE > 3
#error Wrong setting of FF_LFN_UNICODE
#endif
static const BYTE LfnOfs[] = {1,3,5,7,9,14,16,18,20,22,24,28,30};	/* FAT: Offset of LFN characters in the directory entry */
#define MAXDIRB(nc)	((nc + 44U) / 15 * SZDIRE)	/* exFAT: Size of directory entry block scratchpad buffer needed for the name length */

#if FF_USE_LFN == 1		/* LFN enabled with static working buffer */
#if FF_FS_EXFAT
static BYTE	DirBuf[MAXDIRB(FF_MAX_LFN)];	/* Directory entry block scratchpad buffer */
#endif
static WCHAR LfnBuf[FF_MAX_LFN + 1];		/* LFN working buffer */
#define DEF_NAMBUF
#define INIT_NAMBUF(fs)
#define FREE_NAMBUF()
#define LEAVE_MKFS(res)	return res

#elif FF_USE_LFN == 2 	/* LFN enabled with dynamic working buffer on the stack */
#if FF_FS_EXFAT
#define DEF_NAMBUF		WCHAR lbuf[FF_MAX_LFN+1]; BYTE dbuf[MAXDIRB(FF_MAX_LFN)];	/* LFN working buffer and directory entry block scratchpad buffer */
#define INIT_NAMBUF(fs)	{ (fs)->lfnbuf = lbuf; (fs)->dirbuf = dbuf; }
#define FREE_NAMBUF()
#else
#define DEF_NAMBUF		WCHAR lbuf[FF_MAX_LFN+1];	/* LFN working buffer */
#define INIT_NAMBUF(fs)	{ (fs)->lfnbuf = lbuf; }
#define FREE_NAMBUF()
#endif
#define LEAVE_MKFS(res)	return res

#elif FF_USE_LFN == 3 	/* LFN enabled with dynamic working buffer on the heap */
#if FF_FS_EXFAT
#define DEF_NAMBUF		WCHAR *lfn;	/* Pointer to LFN working buffer and directory entry block scratchpad buffer */
#define INIT_NAMBUF(fs)	{ lfn = ff_memalloc((FF_MAX_LFN+1)*2 + MAXDIRB(FF_MAX_LFN)); if (!lfn) LEAVE_FF(fs, FR_NOT_ENOUGH_CORE); (fs)->lfnbuf = lfn; (fs)->dirbuf = (BYTE*)(lfn+FF_MAX_LFN+1); }
#define FREE_NAMBUF()	ff_memfree(lfn)
#else
#define DEF_NAMBUF		WCHAR *lfn;	/* Pointer to LFN working buffer */
#define INIT_NAMBUF(fs)	{ lfn = ff_memalloc((FF_MAX_LFN+1)*2); if (!lfn) LEAVE_FF(fs, FR_NOT_ENOUGH_CORE); (fs)->lfnbuf = lfn; }
#define FREE_NAMBUF()	ff_memfree(lfn)
#endif
#define LEAVE_MKFS(res)	{ if (!work) ff_memfree(buf); return res; }
#define MAX_MALLOC	0x8000	/* Must be >=FF_MAX_SS */

#else
#error Wrong setting of FF_USE_LFN

#endif	/* FF_USE_LFN == 1 */
#endif	/* FF_USE_LFN == 0 */



/*--------------------------------*/
/* Code conversion tables         */
/*--------------------------------*/

#if FF_CODE_PAGE == 0	/* Run-time code page configuration */
#define CODEPAGE CodePage
static WORD CodePage;	/* Current code page */
static const BYTE *ExCvt, *DbcTbl;	/* Pointer to current SBCS up-case table and DBCS code range table below */

static const BYTE Ct437[] = TBL_CT437;
static const BYTE Ct720[] = TBL_CT720;
static const BYTE Ct737[] = TBL_CT737;
static const BYTE Ct771[] = TBL_CT771;
static const BYTE Ct775[] = TBL_CT775;
static const BYTE Ct850[] = TBL_CT850;
static const BYTE Ct852[] = TBL_CT852;
static const BYTE Ct855[] = TBL_CT855;
static const BYTE Ct857[] = TBL_CT857;
static const BYTE Ct860[] = TBL_CT860;
static const BYTE Ct861[] = TBL_CT861;
static const BYTE Ct862[] = TBL_CT862;
static const BYTE Ct863[] = TBL_CT863;
static const BYTE Ct864[] = TBL_CT864;
static const BYTE Ct865[] = TBL_CT865;
static const BYTE Ct866[] = TBL_CT866;
static const BYTE Ct869[] = TBL_CT869;
static const BYTE Dc932[] = TBL_DC932;
static const BYTE Dc936[] = TBL_DC936;
static const BYTE Dc949[] = TBL_DC949;
static const BYTE Dc950[] = TBL_DC950;

#elif FF_CODE_PAGE < 900	/* Static code page configuration (SBCS) */
#define CODEPAGE FF_CODE_PAGE
static const BYTE ExCvt[] = MKCVTBL(TBL_CT, FF_CODE_PAGE);

#else					/* Static code page configuration (DBCS) */
#define CODEPAGE FF_CODE_PAGE
static const BYTE DbcTbl[] = MKCVTBL(TBL_DC, FF_CODE_PAGE);

#endif




/*--------------------------------------------------------------------------

   Module Private Functions

---------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------*/
/* Load/Store multi-byte word in the FAT structure                       */
/*-----------------------------------------------------------------------*/

static WORD ld_word (const BYTE* ptr)	/*	 Load a 2-byte little-endian word */
{
	WORD rv;

	rv = ptr[1];
	rv = rv << 8 | ptr[0];
	return rv;
}

static DWORD ld_dword (const BYTE* ptr)	/* Load a 4-byte little-endian word */
{
	DWORD rv;

	rv = ptr[3];
	rv = rv << 8 | ptr[2];
	rv = rv << 8 | ptr[1];
	rv = rv << 8 | ptr[0];
	return rv;
}

#if FF_FS_EXFAT
static QWORD ld_qword (const BYTE* ptr)	/* Load an 8-byte little-endian word */
{
	QWORD rv;

	rv = ptr[7];
	rv = rv << 8 | ptr[6];
	rv = rv << 8 | ptr[5];
	rv = rv << 8 | ptr[4];
	rv = rv << 8 | ptr[3];
	rv = rv << 8 | ptr[2];
	rv = rv << 8 | ptr[1];
	rv = rv << 8 | ptr[0];
	return rv;
}
#endif

#if !FF_FS_READONLY
static void st_word (BYTE* ptr, WORD val)	/* Store a 2-byte word in little-endian */
{
	*ptr++ = (BYTE)val; val >>= 8;
	*ptr++ = (BYTE)val;
}

static void st_dword (BYTE* ptr, DWORD val)	/* Store a 4-byte word in little-endian */
{
	*ptr++ = (BYTE)val; val >>= 8;
	*ptr++ = (BYTE)val; val >>= 8;
	*ptr++ = (BYTE)val; val >>= 8;
	*ptr++ = (BYTE)val;
}

#if FF_FS_EXFAT
static void st_qword (BYTE* ptr, QWORD val)	/* Store an 8-byte word in little-endian */
{
	*ptr++ = (BYTE)val; val >>= 8;
	*ptr++ = (BYTE)val; val >>= 8;
	*ptr++ = (BYTE)val; val >>= 8;
	*ptr++ = (BYTE)val; val >>= 8;
	*ptr++ = (BYTE)val; val >>= 8;
	*ptr++ = (BYTE)val; val >>= 8;
	*ptr++ = (BYTE)val; val >>= 8;
	*ptr++ = (BYTE)val;
}
#endif
#endif	/* !FF_FS_READONLY */



/*-----------------------------------------------------------------------*/
/* String functions                                                      */
/*-----------------------------------------------------------------------*/

/* Test if the byte is DBC 1st byte */
static int dbc_1st (BYTE c)
{
#if FF_CODE_PAGE == 0		/* Variable code page */
	if (DbcTbl && c >= DbcTbl[0]) {
		if (c <= DbcTbl[1]) return 1;					/* 1st byte range 1 */
		if (c >= DbcTbl[2] && c <= DbcTbl[3]) return 1;	/* 1st byte range 2 */
	}
#elif FF_CODE_PAGE >= 900	/* DBCS fixed code page */
	if (c >= DbcTbl[0]) {
		if (c <= DbcTbl[1]) return 1;
		if (c >= DbcTbl[2] && c <= DbcTbl[3]) return 1;
	}
#else						/* SBCS fixed code page */
	if (c != 0) return 0;	/* Always false */
#endif
	return 0;
}


/* Test if the byte is DBC 2nd byte */
static int dbc_2nd (BYTE c)
{
#if FF_CODE_PAGE == 0		/* Variable code page */
	if (DbcTbl && c >= DbcTbl[4]) {
		if (c <= DbcTbl[5]) return 1;					/* 2nd byte range 1 */
		if (c >= DbcTbl[6] && c <= DbcTbl[7]) return 1;	/* 2nd byte range 2 */
		if (c >= DbcTbl[8] && c <= DbcTbl[9]) return 1;	/* 2nd byte range 3 */
	}
#elif FF_CODE_PAGE >= 900	/* DBCS fixed code page */
	if (c >= DbcTbl[4]) {
		if (c <= DbcTbl[5]) return 1;
		if (c >= DbcTbl[6] && c <= DbcTbl[7]) return 1;
		if (c >= DbcTbl[8] && c <= DbcTbl[9]) return 1;
	}
#else						/* SBCS fixed code page */
	if (c != 0) return 0;	/* Always false */
#endif
	return 0;
}


#if FF_USE_LFN

/* Get a Unicode code point from the TCHAR string in defined API encodeing */
static DWORD tchar2uni (	/* Returns a character in UTF-16 encoding (>=0x10000 on surrogate pair, 0xFFFFFFFF on decode error) */
	const TCHAR** str		/* Pointer to pointer to TCHAR string in configured encoding */
)
{
	DWORD uc;
	const TCHAR *p = *str;

#if FF_LFN_UNICODE == 1		/* UTF-16 input */
	WCHAR wc;

	uc = *p++;	/* Get a unit */
	if (IsSurrogate(uc)) {	/* Surrogate? */
		wc = *p++;		/* Get low surrogate */
		if (!IsSurrogateH(uc) || !IsSurrogateL(wc)) return 0xFFFFFFFF;	/* Wrong surrogate? */
		uc = uc << 16 | wc;
	}

#elif FF_LFN_UNICODE == 2	/* UTF-8 input */
	BYTE b;
	int nf;

	uc = (BYTE)*p++;	/* Get an encoding unit */
	if (uc & 0x80) {	/* Multiple byte code? */
		if        ((uc & 0xE0) == 0xC0) {	/* 2-byte sequence? */
			uc &= 0x1F; nf = 1;
		} else if ((uc & 0xF0) == 0xE0) {	/* 3-byte sequence? */
			uc &= 0x0F; nf = 2;
		} else if ((uc & 0xF8) == 0xF0) {	/* 4-byte sequence? */
			uc &= 0x07; nf = 3;
		} else {							/* Wrong sequence */
			return 0xFFFFFFFF;
		}
		do {	/* Get trailing bytes */
			b = (BYTE)*p++;
			if ((b & 0xC0) != 0x80) return 0xFFFFFFFF;	/* Wrong sequence? */
			uc = uc << 6 | (b & 0x3F);
		} while (--nf != 0);
		if (uc < 0x80 || IsSurrogate(uc) || uc >= 0x110000) return 0xFFFFFFFF;	/* Wrong code? */
		if (uc >= 0x010000) uc = 0xD800DC00 | ((uc - 0x10000) << 6 & 0x3FF0000) | (uc & 0x3FF);	/* Make a surrogate pair if needed */
	}

#elif FF_LFN_UNICODE == 3	/* UTF-32 input */
	uc = (TCHAR)*p++;	/* Get a unit */
	if (uc >= 0x110000 || IsSurrogate(uc)) return 0xFFFFFFFF;	/* Wrong code? */
	if (uc >= 0x010000) uc = 0xD800DC00 | ((uc - 0x10000) << 6 & 0x3FF0000) | (uc & 0x3FF);	/* Make a surrogate pair if needed */

#else		/* ANSI/OEM input */
	BYTE b;
	WCHAR wc;

	wc = (BYTE)*p++;			/* Get a byte */
	if (dbc_1st((BYTE)wc)) {	/* Is it a DBC 1st byte? */
		b = (BYTE)*p++;			/* Get 2nd byte */
		if (!dbc_2nd(b)) return 0xFFFFFFFF;	/* Invalid code? */
		wc = (wc << 8) + b;		/* Make a DBC */
	}
	if (wc != 0) {
		wc = ff_oem2uni(wc, CODEPAGE);	/* ANSI/OEM ==> Unicode */
		if (wc == 0) return 0xFFFFFFFF;	/* Invalid code? */
	}
	uc = wc;

#endif
	*str = p;	/* Next read pointer */
	return uc;
}


/* Store a Unicode char in defined API encoding */
static UINT put_utf (	/* Returns number of encoding units written (0:buffer overflow or wrong encoding) */
	DWORD chr,	/* UTF-16 encoded character (Surrogate pair if >=0x10000) */
	TCHAR* buf,	/* Output buffer */
	UINT szb	/* Size of the buffer */
)
{
#if FF_LFN_UNICODE == 1	/* UTF-16 output */
	WCHAR hs, wc;

	hs = (WCHAR)(chr >> 16);
	wc = (WCHAR)chr;
	if (hs == 0) {	/* Single encoding unit? */
		if (szb < 1 || IsSurrogate(wc)) return 0;	/* Buffer overflow or wrong code? */
		*buf = wc;
		return 1;
	}
	if (szb < 2 || !IsSurrogateH(hs) || !IsSurrogateL(wc)) return 0;	/* Buffer overflow or wrong surrogate? */
	*buf++ = hs;
	*buf++ = wc;
	return 2;

#elif FF_LFN_UNICODE == 2	/* UTF-8 output */
	DWORD hc;

	if (chr < 0x80) {	/* Single byte code? */
		if (szb < 1) return 0;	/* Buffer overflow? */
		*buf = (TCHAR)chr;
		return 1;
	}
	if (chr < 0x800) {	/* 2-byte sequence? */
		if (szb < 2) return 0;	/* Buffer overflow? */
		*buf++ = (TCHAR)(0xC0 | (chr >> 6 & 0x1F));
		*buf++ = (TCHAR)(0x80 | (chr >> 0 & 0x3F));
		return 2;
	}
	if (chr < 0x10000) {	/* 3-byte sequence? */
		if (szb < 3 || IsSurrogate(chr)) return 0;	/* Buffer overflow or wrong code? */
		*buf++ = (TCHAR)(0xE0 | (chr >> 12 & 0x0F));
		*buf++ = (TCHAR)(0x80 | (chr >> 6 & 0x3F));
		*buf++ = (TCHAR)(0x80 | (chr >> 0 & 0x3F));
		return 3;
	}
	/* 4-byte sequence */
	if (szb < 4) return 0;	/* Buffer overflow? */
	hc = ((chr & 0xFFFF0000) - 0xD8000000) >> 6;	/* Get high 10 bits */
	chr = (chr & 0xFFFF) - 0xDC00;					/* Get low 10 bits */
	if (hc >= 0x100000 || chr >= 0x400) return 0;	/* Wrong surrogate? */
	chr = (hc | chr) + 0x10000;
	*buf++ = (TCHAR)(0xF0 | (chr >> 18 & 0x07));
	*buf++ = (TCHAR)(0x80 | (chr >> 12 & 0x3F));
	*buf++ = (TCHAR)(0x80 | (chr >> 6 & 0x3F));
	*buf++ = (TCHAR)(0x80 | (chr >> 0 & 0x3F));
	return 4;

#elif FF_LFN_UNICODE == 3	/* UTF-32 output */
	DWORD hc;

	if (szb < 1) return 0;	/* Buffer overflow? */
	if (chr >= 0x10000) {	/* Out of BMP? */
		hc = ((chr & 0xFFFF0000) - 0xD8000000) >> 6;	/* Get high 10 bits */
		chr = (chr & 0xFFFF) - 0xDC00;					/* Get low 10 bits */
		if (hc >= 0x100000 || chr >= 0x400) return 0;	/* Wrong surrogate? */
		chr = (hc | chr) + 0x10000;
	}
	*buf++ = (TCHAR)chr;
	return 1;

#else						/* ANSI/OEM output */
	WCHAR wc;

	wc = ff_uni2oem(chr, CODEPAGE);
	if (wc >= 0x100) {	/* Is this a DBC? */
		if (szb < 2) return 0;
		*buf++ = (char)(wc >> 8);	/* Store DBC 1st byte */
		*buf++ = (TCHAR)wc;			/* Store DBC 2nd byte */
		return 2;
	}
	if (wc == 0 || szb < 1) return 0;	/* Invalid char or buffer overflow? */
	*buf++ = (TCHAR)wc;					/* Store the character */
	return 1;
#endif
}
#endif	/* FF_USE_LFN */


#if FF_FS_REENTRANT
/*-----------------------------------------------------------------------*/
/* Request/Release grant to access the volume                            */
/*-----------------------------------------------------------------------*/
static int lock_fs (		/* 1:Ok, 0:timeout */
	FATFS* fs		/* Filesystem object */
)
{
	return ff_req_grant(fs->sobj);
}


static void unlock_fs (
	FATFS* fs,		/* Filesystem object */
	FRESULT res		/* Result code to be returned */
)
{
	if (fs && res != FR_NOT_ENABLED && res != FR_INVALID_DRIVE && res != FR_TIMEOUT) {
		ff_rel_grant(fs->sobj);
	}
}

#endif



#if FF_FS_LOCK != 0
/*-----------------------------------------------------------------------*/
/* File lock control functions                                           */
/*-----------------------------------------------------------------------*/

static FRESULT chk_lock (	/* Check if the file can be accessed */
	DIR* dp,		/* Directory object pointing the file to be checked */
	int acc			/* Desired access type (0:Read mode open, 1:Write mode open, 2:Delete or rename) */
)
{
	UINT i, be;

	/* Search open object table for the object */
	be = 0;
	for (i = 0; i < FF_FS_LOCK; i++) {
		if (Files[i].fs) {	/* Existing entry */
			if (Files[i].fs == dp->obj.fs &&	 	/* Check if the object matches with an open object */
				Files[i].clu == dp->obj.sclust &&
				Files[i].ofs == dp->dptr) break;
		} else {			/* Blank entry */
			be = 1;
		}
	}
	if (i == FF_FS_LOCK) {	/* The object has not been opened */
		return (!be && acc != 2) ? FR_TOO_MANY_OPEN_FILES : FR_OK;	/* Is there a blank entry for new object? */
	}

	/* The object was opened. Reject any open against writing file and all write mode open */
	return (acc != 0 || Files[i].ctr == 0x100) ? FR_LOCKED : FR_OK;
}


static int enq_lock (void)	/* Check if an entry is available for a new object */
{
	UINT i;

	for (i = 0; i < FF_FS_LOCK && Files[i].fs; i++) ;
	return (i == FF_FS_LOCK) ? 0 : 1;
}


static UINT inc_lock (	/* Increment object open counter and returns its index (0:Internal error) */
	DIR* dp,	/* Directory object pointing the file to register or increment */
	int acc		/* Desired access (0:Read, 1:Write, 2:Delete/Rename) */
)
{
	UINT i;


	for (i = 0; i < FF_FS_LOCK; i++) {	/* Find the object */
		if (Files[i].fs == dp->obj.fs
		 && Files[i].clu == dp->obj.sclust
		 && Files[i].ofs == dp->dptr) break;
	}

	if (i == FF_FS_LOCK) {			/* Not opened. Register it as new. */
		for (i = 0; i < FF_FS_LOCK && Files[i].fs; i++) ;
		if (i == FF_FS_LOCK) return 0;	/* No free entry to register (int err) */
		Files[i].fs = dp->obj.fs;
		Files[i].clu = dp->obj.sclust;
		Files[i].ofs = dp->dptr;
		Files[i].ctr = 0;
	}

	if (acc >= 1 && Files[i].ctr) return 0;	/* Access violation (int err) */

	Files[i].ctr = acc ? 0x100 : Files[i].ctr + 1;	/* Set semaphore value */

	return i + 1;	/* Index number origin from 1 */
}


static FRESULT dec_lock (	/* Decrement object open counter */
	UINT i			/* Semaphore index (1..) */
)
{
	WORD n;
	FRESULT res;


	if (--i < FF_FS_LOCK) {	/* Index number origin from 0 */
		n = Files[i].ctr;
		if (n == 0x100) n = 0;	/* If write mode open, delete the entry */
		if (n > 0) n--;			/* Decrement read mode open count */
		Files[i].ctr = n;
		if (n == 0) Files[i].fs = 0;	/* Delete the entry if open count gets zero */
		res = FR_OK;
	} else {
		res = FR_INT_ERR;		/* Invalid index nunber */
	}
	return res;
}


static void clear_lock (	/* Clear lock entries of the volume */
	FATFS *fs
)
{
	UINT i;

	for (i = 0; i < FF_FS_LOCK; i++) {
		if (Files[i].fs == fs) Files[i].fs = 0;
	}
}

#endif	/* FF_FS_LOCK != 0 */



/*-----------------------------------------------------------------------*/
/* Move/Flush disk access window in the filesystem object                */
/*-----------------------------------------------------------------------*/
#if !FF_FS_READONLY
static FRESULT sync_window (	/* Returns FR_OK or FR_DISK_ERR */
	FATFS* fs			/* Filesystem object */
)
{
	FRESULT res = FR_OK;


	if (fs->wflag) {	/* Is the disk access window dirty? */
		if (disk_write(fs->pdrv, fs->win, fs->winsect, 1) == RES_OK) {	/* Write it back into the volume */
			fs->wflag = 0;	/* Clear window dirty flag */
			if (fs->winsect - fs->fatbase < fs->fsize) {	/* Is it in the 1st FAT? */
				if (fs->n_fats == 2) disk_write(fs->pdrv, fs->win, fs->winsect + fs->fsize, 1);	/* Reflect it to 2nd FAT if needed */
			}
		} else {
			res = FR_DISK_ERR;
		}
	}
	return res;
}
#endif


static FRESULT move_window (	/* Returns FR_OK or FR_DISK_ERR */
	FATFS* fs,		/* Filesystem object */
	LBA_t sect		/* Sector LBA to make appearance in the fs->win[] */
)
{
	FRESULT res = FR_OK;


	if (sect != fs->winsect) {	/* Window offset changed? */
#if !FF_FS_READONLY
		res = sync_window(fs);		/* Flush the window */
#endif
		if (res == FR_OK) {			/* Fill sector window with new data */
			if (disk_read(fs->pdrv, fs->win, sect, 1) != RES_OK) {
				sect = (LBA_t)0 - 1;	/* Invalidate window if read data is not valid */
				res = FR_DISK_ERR;
			}
			fs->winsect = sect;
		}
	}
	return res;
}




#if !FF_FS_READONLY
/*-----------------------------------------------------------------------*/
/* Synchronize filesystem and data on the storage                        */
/*-----------------------------------------------------------------------*/

static FRESULT sync_fs (	/* Returns FR_OK or FR_DISK_ERR */
	FATFS* fs		/* Filesystem object */
)
{
	FRESULT res;


	res = sync_window(fs);
	if (res == FR_OK) {
		if (fs->fs_type == FS_FAT32 && fs->fsi_flag == 1) {	/* FAT32: Update FSInfo sector if needed */
			/* Create FSInfo structure */
			memset(fs->win, 0, sizeof fs->win);
			st_word(fs->win + BS_55AA, 0xAA55);					/* Boot signature */
			st_dword(fs->win + FSI_LeadSig, 0x41615252);		/* Leading signature */
			st_dword(fs->win + FSI_StrucSig, 0x61417272);		/* Structure signature */
			st_dword(fs->win + FSI_Free_Count, fs->free_clst);	/* Number of free clusters */
			st_dword(fs->win + FSI_Nxt_Free, fs->last_clst);	/* Last allocated culuster */
			fs->winsect = fs->volbase + 1;						/* Write it into the FSInfo sector (Next to VBR) */
			disk_write(fs->pdrv, fs->win, fs->winsect, 1);
			fs->fsi_flag = 0;
		}
		/* Make sure that no pending write process in the lower layer */
		if (disk_ioctl(fs->pdrv, CTRL_SYNC, 0) != RES_OK) res = FR_DISK_ERR;
	}

	return res;
}

#endif



/*-----------------------------------------------------------------------*/
/* Get physical sector number from cluster number                        */
/*-----------------------------------------------------------------------*/

static LBA_t clst2sect (	/* !=0:Sector number, 0:Failed (invalid cluster#) */
	FATFS* fs,		/* Filesystem object */
	DWORD clst		/* Cluster# to be converted */
)
{
	clst -= 2;		/* Cluster number is origin from 2 */
	if (clst >= fs->n_fatent - 2) return 0;		/* Is it invalid cluster number? */
	return fs->database + (LBA_t)fs->csize * clst;	/* Start sector number of the cluster */
}




/*-----------------------------------------------------------------------*/
/* FAT access - Read value of an FAT entry                               */
/*-----------------------------------------------------------------------*/

static DWORD get_fat (		/* 0xFFFFFFFF:Disk error, 1:Internal error, 2..0x7FFFFFFF:Cluster status */
	FFOBJID* obj,	/* Corresponding object */
	DWORD clst		/* Cluster number to get the value */
)
{
	UINT wc, bc;
	DWORD val;
	FATFS *fs = obj->fs;


	if (clst < 2 || clst >= fs->n_fatent) {	/* Check if in valid range */
		val = 1;	/* Internal error */

	} else {
		val = 0xFFFFFFFF;	/* Default value falls on disk error */

		switch (fs->fs_type) {
		case FS_FAT12 :
			bc = (UINT)clst; bc += bc / 2;
			if (move_window(fs, fs->fatbase + (bc / SS(fs))) != FR_OK) break;
			wc = fs->win[bc++ % SS(fs)];		/* Get 1st byte of the entry */
			if (move_window(fs, fs->fatbase + (bc / SS(fs))) != FR_OK) break;
			wc |= fs->win[bc % SS(fs)] << 8;	/* Merge 2nd byte of the entry */
			val = (clst & 1) ? (wc >> 4) : (wc & 0xFFF);	/* Adjust bit position */
			break;

		case FS_FAT16 :
			if (move_window(fs, fs->fatbase + (clst / (SS(fs) / 2))) != FR_OK) break;
			val = ld_word(fs->win + clst * 2 % SS(fs));		/* Simple WORD array */
			break;

		case FS_FAT32 :
			if (move_window(fs, fs->fatbase + (clst / (SS(fs) / 4))) != FR_OK) break;
			val = ld_dword(fs->win + clst * 4 % SS(fs)) & 0x0FFFFFFF;	/* Simple DWORD array but mask out upper 4 bits */
			break;
#if FF_FS_EXFAT
		case FS_EXFAT :
			if ((obj->objsize != 0 && obj->sclust != 0) || obj->stat == 0) {	/* Object except root dir must have valid data length */
				DWORD cofs = clst - obj->sclust;	/* Offset from start cluster */
				DWORD clen = (DWORD)((LBA_t)((obj->objsize - 1) / SS(fs)) / fs->csize);	/* Number of clusters - 1 */

				if (obj->stat == 2 && cofs <= clen) {	/* Is it a contiguous chain? */
					val = (cofs == clen) ? 0x7FFFFFFF : clst + 1;	/* No data on the FAT, generate the value */
					break;
				}
				if (obj->stat == 3 && cofs < obj->n_cont) {	/* Is it in the 1st fragment? */
					val = clst + 1; 	/* Generate the value */
					break;
				}
				if (obj->stat != 2) {	/* Get value from FAT if FAT chain is valid */
					if (obj->n_frag != 0) {	/* Is it on the growing edge? */
						val = 0x7FFFFFFF;	/* Generate EOC */
					} else {
						if (move_window(fs, fs->fatbase + (clst / (SS(fs) / 4))) != FR_OK) break;
						val = ld_dword(fs->win + clst * 4 % SS(fs)) & 0x7FFFFFFF;
					}
					break;
				}
			}
			val = 1;	/* Internal error */
			break;
#endif
		default:
			val = 1;	/* Internal error */
		}
	}

	return val;
}




#if !FF_FS_READONLY
/*-----------------------------------------------------------------------*/
/* FAT access - Change value of an FAT entry                             */
/*-----------------------------------------------------------------------*/

static FRESULT put_fat (	/* FR_OK(0):succeeded, !=0:error */
	FATFS* fs,		/* Corresponding filesystem object */
	DWORD clst,		/* FAT index number (cluster number) to be changed */
	DWORD val		/* New value to be set to the entry */
)
{
	UINT bc;
	BYTE *p;
	FRESULT res = FR_INT_ERR;


	if (clst >= 2 && clst < fs->n_fatent) {	/* Check if in valid range */
		switch (fs->fs_type) {
		case FS_FAT12:
			bc = (UINT)clst; bc += bc / 2;	/* bc: byte offset of the entry */
			res = move_window(fs, fs->fatbase + (bc / SS(fs)));
			if (res != FR_OK) break;
			p = fs->win + bc++ % SS(fs);
			*p = (clst & 1) ? ((*p & 0x0F) | ((BYTE)val << 4)) : (BYTE)val;	/* Update 1st byte */
			fs->wflag = 1;
			res = move_window(fs, fs->fatbase + (bc / SS(fs)));
			if (res != FR_OK) break;
			p = fs->win + bc % SS(fs);
			*p = (clst & 1) ? (BYTE)(val >> 4) : ((*p & 0xF0) | ((BYTE)(val >> 8) & 0x0F));	/* Update 2nd byte */
			fs->wflag = 1;
			break;

		case FS_FAT16:
			res = move_window(fs, fs->fatbase + (clst / (SS(fs) / 2)));
			if (res != FR_OK) break;
			st_word(fs->win + clst * 2 % SS(fs), (WORD)val);	/* Simple WORD array */
			fs->wflag = 1;
			break;

		case FS_FAT32:
#if FF_FS_EXFAT
		case FS_EXFAT:
#endif
			res = move_window(fs, fs->fatbase + (clst / (SS(fs) / 4)));
			if (res != FR_OK) break;
			if (!FF_FS_EXFAT || fs->fs_type != FS_EXFAT) {
				val = (val & 0x0FFFFFFF) | (ld_dword(fs->win + clst * 4 % SS(fs)) & 0xF0000000);
			}
			st_dword(fs->win + clst * 4 % SS(fs), val);
			fs->wflag = 1;
			break;
		}
	}
	return res;
}

#endif /* !FF_FS_READONLY */




#if FF_FS_EXFAT && !FF_FS_READONLY
/*-----------------------------------------------------------------------*/
/* exFAT: Accessing FAT and Allocation Bitmap                            */
/*-----------------------------------------------------------------------*/

/*--------------------------------------*/
/* Find a contiguous free cluster block */
/*--------------------------------------*/

static DWORD find_bitmap (	/* 0:Not found, 2..:Cluster block found, 0xFFFFFFFF:Disk error */
	FATFS* fs,	/* Filesystem object */
	DWORD clst,	/* Cluster number to scan from */
	DWORD ncl	/* Number of contiguous clusters to find (1..) */
)
{
	BYTE bm, bv;
	UINT i;
	DWORD val, scl, ctr;


	clst -= 2;	/* The first bit in the bitmap corresponds to cluster #2 */
	if (clst >= fs->n_fatent - 2) clst = 0;
	scl = val = clst; ctr = 0;
	for (;;) {
		if (move_window(fs, fs->bitbase + val / 8 / SS(fs)) != FR_OK) return 0xFFFFFFFF;
		i = val / 8 % SS(fs); bm = 1 << (val % 8);
		do {
			do {
				bv = fs->win[i] & bm; bm <<= 1;		/* Get bit value */
				if (++val >= fs->n_fatent - 2) {	/* Next cluster (with wrap-around) */
					val = 0; bm = 0; i = SS(fs);
				}
				if (bv == 0) {	/* Is it a free cluster? */
					if (++ctr == ncl) return scl + 2;	/* Check if run length is sufficient for required */
				} else {
					scl = val; ctr = 0;		/* Encountered a cluster in-use, restart to scan */
				}
				if (val == clst) return 0;	/* All cluster scanned? */
			} while (bm != 0);
			bm = 1;
		} while (++i < SS(fs));
	}
}


/*----------------------------------------*/
/* Set/Clear a block of allocation bitmap */
/*----------------------------------------*/

static FRESULT change_bitmap (
	FATFS* fs,	/* Filesystem object */
	DWORD clst,	/* Cluster number to change from */
	DWORD ncl,	/* Number of clusters to be changed */
	int bv		/* bit value to be set (0 or 1) */
)
{
	BYTE bm;
	UINT i;
	LBA_t sect;


	clst -= 2;	/* The first bit corresponds to cluster #2 */
	sect = fs->bitbase + clst / 8 / SS(fs);	/* Sector address */
	i = clst / 8 % SS(fs);					/* Byte offset in the sector */
	bm = 1 << (clst % 8);					/* Bit mask in the byte */
	for (;;) {
		if (move_window(fs, sect++) != FR_OK) return FR_DISK_ERR;
		do {
			do {
				if (bv == (int)((fs->win[i] & bm) != 0)) return FR_INT_ERR;	/* Is the bit expected value? */
				fs->win[i] ^= bm;	/* Flip the bit */
				fs->wflag = 1;
				if (--ncl == 0) return FR_OK;	/* All bits processed? */
			} while (bm <<= 1);		/* Next bit */
			bm = 1;
		} while (++i < SS(fs));		/* Next byte */
		i = 0;
	}
}


/*---------------------------------------------*/
/* Fill the first fragment of the FAT chain    */
/*---------------------------------------------*/

static FRESULT fill_first_frag (
	FFOBJID* obj	/* Pointer to the corresponding object */
)
{
	FRESULT res;
	DWORD cl, n;


	if (obj->stat == 3) {	/* Has the object been changed 'fragmented' in this session? */
		for (cl = obj->sclust, n = obj->n_cont; n; cl++, n--) {	/* Create cluster chain on the FAT */
			res = put_fat(obj->fs, cl, cl + 1);
			if (res != FR_OK) return res;
		}
		obj->stat = 0;	/* Change status 'FAT chain is valid' */
	}
	return FR_OK;
}


/*---------------------------------------------*/
/* Fill the last fragment of the FAT chain     */
/*---------------------------------------------*/

static FRESULT fill_last_frag (
	FFOBJID* obj,	/* Pointer to the corresponding object */
	DWORD lcl,		/* Last cluster of the fragment */
	DWORD term		/* Value to set the last FAT entry */
)
{
	FRESULT res;


	while (obj->n_frag > 0) {	/* Create the chain of last fragment */
		res = put_fat(obj->fs, lcl - obj->n_frag + 1, (obj->n_frag > 1) ? lcl - obj->n_frag + 2 : term);
		if (res != FR_OK) return res;
		obj->n_frag--;
	}
	return FR_OK;
}

#endif	/* FF_FS_EXFAT && !FF_FS_READONLY */



#if !FF_FS_READONLY
/*-----------------------------------------------------------------------*/
/* FAT handling - Remove a cluster chain                                 */
/*-----------------------------------------------------------------------*/

static FRESULT remove_chain (	/* FR_OK(0):succeeded, !=0:error */
	FFOBJID* obj,		/* Corresponding object */
	DWORD clst,			/* Cluster to remove a chain from */
	DWORD pclst			/* Previous cluster of clst (0 if entire chain) */
)
{
	FRESULT res = FR_OK;
	DWORD nxt;
	FATFS *fs = obj->fs;
#if FF_FS_EXFAT || FF_USE_TRIM
	DWORD scl = clst, ecl = clst;
#endif
#if FF_USE_TRIM
	LBA_t rt[2];
#endif

	if (clst < 2 || clst >= fs->n_fatent) return FR_INT_ERR;	/* Check if in valid range */

	/* Mark the previous cluster 'EOC' on the FAT if it exists */
	if (pclst != 0 && (!FF_FS_EXFAT || fs->fs_type != FS_EXFAT || obj->stat != 2)) {
		res = put_fat(fs, pclst, 0xFFFFFFFF);
		if (res != FR_OK) return res;
	}

	/* Remove the chain */
	do {
		nxt = get_fat(obj, clst);			/* Get cluster status */
		if (nxt == 0) break;				/* Empty cluster? */
		if (nxt == 1) return FR_INT_ERR;	/* Internal error? */
		if (nxt == 0xFFFFFFFF) return FR_DISK_ERR;	/* Disk error? */
		if (!FF_FS_EXFAT || fs->fs_type != FS_EXFAT) {
			res = put_fat(fs, clst, 0);		/* Mark the cluster 'free' on the FAT */
			if (res != FR_OK) return res;
		}
		if (fs->free_clst < fs->n_fatent - 2) {	/* Update FSINFO */
			fs->free_clst++;
			fs->fsi_flag |= 1;
		}
#if FF_FS_EXFAT || FF_USE_TRIM
		if (ecl + 1 == nxt) {	/* Is next cluster contiguous? */
			ecl = nxt;
		} else {				/* End of contiguous cluster block */
#if FF_FS_EXFAT
			if (fs->fs_type == FS_EXFAT) {
				res = change_bitmap(fs, scl, ecl - scl + 1, 0);	/* Mark the cluster block 'free' on the bitmap */
				if (res != FR_OK) return res;
			}
#endif
#if FF_USE_TRIM
			rt[0] = clst2sect(fs, scl);					/* Start of data area to be freed */
			rt[1] = clst2sect(fs, ecl) + fs->csize - 1;	/* End of data area to be freed */
			disk_ioctl(fs->pdrv, CTRL_TRIM, rt);		/* Inform storage device that the data in the block may be erased */
#endif
			scl = ecl = nxt;
		}
#endif
		clst = nxt;					/* Next cluster */
	} while (clst < fs->n_fatent);	/* Repeat while not the last link */

#if FF_FS_EXFAT
	/* Some post processes for chain status */
	if (fs->fs_type == FS_EXFAT) {
		if (pclst == 0) {	/* Has the entire chain been removed? */
			obj->stat = 0;		/* Change the chain status 'initial' */
		} else {
			if (obj->stat == 0) {	/* Is it a fragmented chain from the beginning of this session? */
				clst = obj->sclust;		/* Follow the chain to check if it gets contiguous */
				while (clst != pclst) {
					nxt = get_fat(obj, clst);
					if (nxt < 2) return FR_INT_ERR;
					if (nxt == 0xFFFFFFFF) return FR_DISK_ERR;
					if (nxt != clst + 1) break;	/* Not contiguous? */
					clst++;
				}
				if (clst == pclst) {	/* Has the chain got contiguous again? */
					obj->stat = 2;		/* Change the chain status 'contiguous' */
				}
			} else {
				if (obj->stat == 3 && pclst >= obj->sclust && pclst <= obj->sclust + obj->n_cont) {	/* Was the chain fragmented in this session and got contiguous again? */
					obj->stat = 2;	/* Change the chain status 'contiguous' */
				}
			}
		}
	}
#endif
	return FR_OK;
}




/*-----------------------------------------------------------------------*/
/* FAT handling - Stretch a chain or Create a new chain                  */
/*-----------------------------------------------------------------------*/

static DWORD create_chain (	/* 0:No free cluster, 1:Internal error, 0xFFFFFFFF:Disk error, >=2:New cluster# */
	FFOBJID* obj,		/* Corresponding object */
	DWORD clst			/* Cluster# to stretch, 0:Create a new chain */
)
{
	DWORD cs, ncl, scl;
	FRESULT res;
	FATFS *fs = obj->fs;


	if (clst == 0) {	/* Create a new chain */
		scl = fs->last_clst;				/* Suggested cluster to start to find */
		if (scl == 0 || scl >= fs->n_fatent) scl = 1;
	}
	else {				/* Stretch a chain */
		cs = get_fat(obj, clst);			/* Check the cluster status */
		if (cs < 2) return 1;				/* Test for insanity */
		if (cs == 0xFFFFFFFF) return cs;	/* Test for disk error */
		if (cs < fs->n_fatent) return cs;	/* It is already followed by next cluster */
		scl = clst;							/* Cluster to start to find */
	}
	if (fs->free_clst == 0) return 0;		/* No free cluster */

#if FF_FS_EXFAT
	if (fs->fs_type == FS_EXFAT) {	/* On the exFAT volume */
		ncl = find_bitmap(fs, scl, 1);				/* Find a free cluster */
		if (ncl == 0 || ncl == 0xFFFFFFFF) return ncl;	/* No free cluster or hard error? */
		res = change_bitmap(fs, ncl, 1, 1);			/* Mark the cluster 'in use' */
		if (res == FR_INT_ERR) return 1;
		if (res == FR_DISK_ERR) return 0xFFFFFFFF;
		if (clst == 0) {							/* Is it a new chain? */
			obj->stat = 2;							/* Set status 'contiguous' */
		} else {									/* It is a stretched chain */
			if (obj->stat == 2 && ncl != scl + 1) {	/* Is the chain got fragmented? */
				obj->n_cont = scl - obj->sclust;	/* Set size of the contiguous part */
				obj->stat = 3;						/* Change status 'just fragmented' */
			}
		}
		if (obj->stat != 2) {	/* Is the file non-contiguous? */
			if (ncl == clst + 1) {	/* Is the cluster next to previous one? */
				obj->n_frag = obj->n_frag ? obj->n_frag + 1 : 2;	/* Increment size of last framgent */
			} else {				/* New fragment */
				if (obj->n_frag == 0) obj->n_frag = 1;
				res = fill_last_frag(obj, clst, ncl);	/* Fill last fragment on the FAT and link it to new one */
				if (res == FR_OK) obj->n_frag = 1;
			}
		}
	} else
#endif
	{	/* On the FAT/FAT32 volume */
		ncl = 0;
		if (scl == clst) {						/* Stretching an existing chain? */
			ncl = scl + 1;						/* Test if next cluster is free */
			if (ncl >= fs->n_fatent) ncl = 2;
			cs = get_fat(obj, ncl);				/* Get next cluster status */
			if (cs == 1 || cs == 0xFFFFFFFF) return cs;	/* Test for error */
			if (cs != 0) {						/* Not free? */
				cs = fs->last_clst;				/* Start at suggested cluster if it is valid */
				if (cs >= 2 && cs < fs->n_fatent) scl = cs;
				ncl = 0;
			}
		}
		if (ncl == 0) {	/* The new cluster cannot be contiguous and find another fragment */
			ncl = scl;	/* Start cluster */
			for (;;) {
				ncl++;							/* Next cluster */
				if (ncl >= fs->n_fatent) {		/* Check wrap-around */
					ncl = 2;
					if (ncl > scl) return 0;	/* No free cluster found? */
				}
				cs = get_fat(obj, ncl);			/* Get the cluster status */
				if (cs == 0) break;				/* Found a free cluster? */
				if (cs == 1 || cs == 0xFFFFFFFF) return cs;	/* Test for error */
				if (ncl == scl) return 0;		/* No free cluster found? */
			}
		}
		res = put_fat(fs, ncl, 0xFFFFFFFF);		/* Mark the new cluster 'EOC' */
		if (res == FR_OK && clst != 0) {
			res = put_fat(fs, clst, ncl);		/* Link it from the previous one if needed */
		}
	}

	if (res == FR_OK) {			/* Update FSINFO if function succeeded. */
		fs->last_clst = ncl;
		if (fs->free_clst <= fs->n_fatent - 2) fs->free_clst--;
		fs->fsi_flag |= 1;
	} else {
		ncl = (res == FR_DISK_ERR) ? 0xFFFFFFFF : 1;	/* Failed. Generate error status */
	}

	return ncl;		/* Return new cluster number or error status */
}

#endif /* !FF_FS_READONLY */




#if FF_USE_FASTSEEK
/*-----------------------------------------------------------------------*/
/* FAT handling - Convert offset into cluster with link map table        */
/*-----------------------------------------------------------------------*/

static DWORD clmt_clust (	/* <2:Error, >=2:Cluster number */
	FIL* fp,		/* Pointer to the file object */
	FSIZE_t ofs		/* File offset to be converted to cluster# */
)
{
	DWORD cl, ncl, *tbl;
	FATFS *fs = fp->obj.fs;


	tbl = fp->cltbl + 1;	/* Top of CLMT */
	cl = (DWORD)(ofs / SS(fs) / fs->csize);	/* Cluster order from top of the file */
	for (;;) {
		ncl = *tbl++;			/* Number of cluters in the fragment */
		if (ncl == 0) return 0;	/* End of table? (error) */
		if (cl < ncl) break;	/* In this fragment? */
		cl -= ncl; tbl++;		/* Next fragment */
	}
	return cl + *tbl;	/* Return the cluster number */
}

#endif	/* FF_USE_FASTSEEK */




/*-----------------------------------------------------------------------*/
/* Directory handling - Fill a cluster with zeros                        */
/*-----------------------------------------------------------------------*/

#if !FF_FS_READONLY
static FRESULT dir_clear (	/* Returns FR_OK or FR_DISK_ERR */
	FATFS *fs,		/* Filesystem object */
	DWORD clst		/* Directory table to clear */
)
{
	LBA_t sect;
	UINT n, szb;
	BYTE *ibuf;


	if (sync_window(fs) != FR_OK) return FR_DISK_ERR;	/* Flush disk access window */
	sect = clst2sect(fs, clst);		/* Top of the cluster */
	fs->winsect = sect;				/* Set window to top of the cluster */
	memset(fs->win, 0, sizeof fs->win);	/* Clear window buffer */
#if FF_USE_LFN == 3		/* Quick table clear by using multi-secter write */
	/* Allocate a temporary buffer */
	for (szb = ((DWORD)fs->csize * SS(fs) >= MAX_MALLOC) ? MAX_MALLOC : fs->csize * SS(fs), ibuf = 0; szb > SS(fs) && (ibuf = ff_memalloc(szb)) == 0; szb /= 2) ;
	if (szb > SS(fs)) {		/* Buffer allocated? */
		memset(ibuf, 0, szb);
		szb /= SS(fs);		/* Bytes -> Sectors */
		for (n = 0; n < fs->csize && disk_write(fs->pdrv, ibuf, sect + n, szb) == RES_OK; n += szb) ;	/* Fill the cluster with 0 */
		ff_memfree(ibuf);
	} else
#endif
	{
		ibuf = fs->win; szb = 1;	/* Use window buffer (many single-sector writes may take a time) */
		for (n = 0; n < fs->csize && disk_write(fs->pdrv, ibuf, sect + n, szb) == RES_OK; n += szb) ;	/* Fill the cluster with 0 */
	}
	return (n == fs->csize) ? FR_OK : FR_DISK_ERR;
}
#endif	/* !FF_FS_READONLY */




/*-----------------------------------------------------------------------*/
/* Directory handling - Set directory index                              */
/*-----------------------------------------------------------------------*/

static FRESULT dir_sdi (	/* FR_OK(0):succeeded, !=0:error */
	DIR* dp,		/* Pointer to directory object */
	DWORD ofs		/* Offset of directory table */
)
{
	DWORD csz, clst;
	FATFS *fs = dp->obj.fs;


	if (ofs >= (DWORD)((FF_FS_EXFAT && fs->fs_type == FS_EXFAT) ? MAX_DIR_EX : MAX_DIR) || ofs % SZDIRE) {	/* Check range of offset and alignment */
		return FR_INT_ERR;
	}
	dp->dptr = ofs;				/* Set current offset */
	clst = dp->obj.sclust;		/* Table start cluster (0:root) */
	if (clst == 0 && fs->fs_type >= FS_FAT32) {	/* Replace cluster# 0 with root cluster# */
		clst = (DWORD)fs->dirbase;
		if (FF_FS_EXFAT) dp->obj.stat = 0;	/* exFAT: Root dir has an FAT chain */
	}

	if (clst == 0) {	/* Static table (root-directory on the FAT volume) */
		if (ofs / SZDIRE >= fs->n_rootdir) return FR_INT_ERR;	/* Is index out of range? */
		dp->sect = fs->dirbase;

	} else {			/* Dynamic table (sub-directory or root-directory on the FAT32/exFAT volume) */
		csz = (DWORD)fs->csize * SS(fs);	/* Bytes per cluster */
		while (ofs >= csz) {				/* Follow cluster chain */
			clst = get_fat(&dp->obj, clst);				/* Get next cluster */
			if (clst == 0xFFFFFFFF) return FR_DISK_ERR;	/* Disk error */
			if (clst < 2 || clst >= fs->n_fatent) return FR_INT_ERR;	/* Reached to end of table or internal error */
			ofs -= csz;
		}
		dp->sect = clst2sect(fs, clst);
	}
	dp->clust = clst;					/* Current cluster# */
	if (dp->sect == 0) return FR_INT_ERR;
	dp->sect += ofs / SS(fs);			/* Sector# of the directory entry */
	dp->dir = fs->win + (ofs % SS(fs));	/* Pointer to the entry in the win[] */

	return FR_OK;
}




/*-----------------------------------------------------------------------*/
/* Directory handling - Move directory table index next                  */
/*-----------------------------------------------------------------------*/

static FRESULT dir_next (	/* FR_OK(0):succeeded, FR_NO_FILE:End of table, FR_DENIED:Could not stretch */
	DIR* dp,				/* Pointer to the directory object */
	int stretch				/* 0: Do not stretch table, 1: Stretch table if needed */
)
{
	DWORD ofs, clst;
	FATFS *fs = dp->obj.fs;


	ofs = dp->dptr + SZDIRE;	/* Next entry */
	if (ofs >= (DWORD)((FF_FS_EXFAT && fs->fs_type == FS_EXFAT) ? MAX_DIR_EX : MAX_DIR)) dp->sect = 0;	/* Disable it if the offset reached the max value */
	if (dp->sect == 0) return FR_NO_FILE;	/* Report EOT if it has been disabled */

	if (ofs % SS(fs) == 0) {	/* Sector changed? */
		dp->sect++;				/* Next sector */

		if (dp->clust == 0) {	/* Static table */
			if (ofs / SZDIRE >= fs->n_rootdir) {	/* Report EOT if it reached end of static table */
				dp->sect = 0; return FR_NO_FILE;
			}
		}
		else {					/* Dynamic table */
			if ((ofs / SS(fs) & (fs->csize - 1)) == 0) {	/* Cluster changed? */
				clst = get_fat(&dp->obj, dp->clust);		/* Get next cluster */
				if (clst <= 1) return FR_INT_ERR;			/* Internal error */
				if (clst == 0xFFFFFFFF) return FR_DISK_ERR;	/* Disk error */
				if (clst >= fs->n_fatent) {					/* It reached end of dynamic table */
#if !FF_FS_READONLY
					if (!stretch) {								/* If no stretch, report EOT */
						dp->sect = 0; return FR_NO_FILE;
					}
					clst = create_chain(&dp->obj, dp->clust);	/* Allocate a cluster */
					if (clst == 0) return FR_DENIED;			/* No free cluster */
					if (clst == 1) return FR_INT_ERR;			/* Internal error */
					if (clst == 0xFFFFFFFF) return FR_DISK_ERR;	/* Disk error */
					if (dir_clear(fs, clst) != FR_OK) return FR_DISK_ERR;	/* Clean up the stretched table */
					if (FF_FS_EXFAT) dp->obj.stat |= 4;			/* exFAT: The directory has been stretched */
#else
					if (!stretch) dp->sect = 0;					/* (this line is to suppress compiler warning) */
					dp->sect = 0; return FR_NO_FILE;			/* Report EOT */
#endif
				}
				dp->clust = clst;		/* Initialize data for new cluster */
				dp->sect = clst2sect(fs, clst);
			}
		}
	}
	dp->dptr = ofs;						/* Current entry */
	dp->dir = fs->win + ofs % SS(fs);	/* Pointer to the entry in the win[] */

	return FR_OK;
}




#if !FF_FS_READONLY
/*-----------------------------------------------------------------------*/
/* Directory handling - Reserve a block of directory entries             */
/*-----------------------------------------------------------------------*/

static FRESULT dir_alloc (	/* FR_OK(0):succeeded, !=0:error */
	DIR* dp,				/* Pointer to the directory object */
	UINT n_ent				/* Number of contiguous entries to allocate */
)
{
	FRESULT res;
	UINT n;
	FATFS *fs = dp->obj.fs;


	res = dir_sdi(dp, 0);
	if (res == FR_OK) {
		n = 0;
		do {
			res = move_window(fs, dp->sect);
			if (res != FR_OK) break;
#if FF_FS_EXFAT
			if ((fs->fs_type == FS_EXFAT) ? (int)((dp->dir[XDIR_Type] & 0x80) == 0) : (int)(dp->dir[DIR_Name] == DDEM || dp->dir[DIR_Name] == 0)) {	/* Is the entry free? */
#else
			if (dp->dir[DIR_Name] == DDEM || dp->dir[DIR_Name] == 0) {	/* Is the entry free? */
#endif
				if (++n == n_ent) break;	/* Is a block of contiguous free entries found? */
			} else {
				n = 0;				/* Not a free entry, restart to search */
			}
			res = dir_next(dp, 1);	/* Next entry with table stretch enabled */
		} while (res == FR_OK);
	}

	if (res == FR_NO_FILE) res = FR_DENIED;	/* No directory entry to allocate */
	return res;
}

#endif	/* !FF_FS_READONLY */




/*-----------------------------------------------------------------------*/
/* FAT: Directory handling - Load/Store start cluster number             */
/*-----------------------------------------------------------------------*/

static DWORD ld_clust (	/* Returns the top cluster value of the SFN entry */
	FATFS* fs,			/* Pointer to the fs object */
	const BYTE* dir		/* Pointer to the key entry */
)
{
	DWORD cl;

	cl = ld_word(dir + DIR_FstClusLO);
	if (fs->fs_type == FS_FAT32) {
		cl |= (DWORD)ld_word(dir + DIR_FstClusHI) << 16;
	}

	return cl;
}


#if !FF_FS_READONLY
static void st_clust (
	FATFS* fs,	/* Pointer to the fs object */
	BYTE* dir,	/* Pointer to the key entry */
	DWORD cl	/* Value to be set */
)
{
	st_word(dir + DIR_FstClusLO, (WORD)cl);
	if (fs->fs_type == FS_FAT32) {
		st_word(dir + DIR_FstClusHI, (WORD)(cl >> 16));
	}
}
#endif



#if FF_USE_LFN
/*--------------------------------------------------------*/
/* FAT-LFN: Compare a part of file name with an LFN entry */
/*--------------------------------------------------------*/

static int cmp_lfn (		/* 1:matched, 0:not matched */
	const WCHAR* lfnbuf,	/* Pointer to the LFN working buffer to be compared */
	BYTE* dir				/* Pointer to the directory entry containing the part of LFN */
)
{
	UINT i, s;
	WCHAR wc, uc;


	if (ld_word(dir + LDIR_FstClusLO) != 0) return 0;	/* Check LDIR_FstClusLO */

	i = ((dir[LDIR_Ord] & 0x3F) - 1) * 13;	/* Offset in the LFN buffer */

	for (wc = 1, s = 0; s < 13; s++) {		/* Process all characters in the entry */
		uc = ld_word(dir + LfnOfs[s]);		/* Pick an LFN character */
		if (wc != 0) {
			if (i >= FF_MAX_LFN + 1 || ff_wtoupper(uc) != ff_wtoupper(lfnbuf[i++])) {	/* Compare it */
				return 0;					/* Not matched */
			}
			wc = uc;
		} else {
			if (uc != 0xFFFF) return 0;		/* Check filler */
		}
	}

	if ((dir[LDIR_Ord] & LLEF) && wc && lfnbuf[i]) return 0;	/* Last segment matched but different length */

	return 1;		/* The part of LFN matched */
}


#if FF_FS_MINIMIZE <= 1 || FF_FS_RPATH >= 2 || FF_USE_LABEL || FF_FS_EXFAT
/*-----------------------------------------------------*/
/* FAT-LFN: Pick a part of file name from an LFN entry */
/*-----------------------------------------------------*/

static int pick_lfn (	/* 1:succeeded, 0:buffer overflow or invalid LFN entry */
	WCHAR* lfnbuf,		/* Pointer to the LFN working buffer */
	BYTE* dir			/* Pointer to the LFN entry */
)
{
	UINT i, s;
	WCHAR wc, uc;


	if (ld_word(dir + LDIR_FstClusLO) != 0) return 0;	/* Check LDIR_FstClusLO is 0 */

	i = ((dir[LDIR_Ord] & ~LLEF) - 1) * 13;	/* Offset in the LFN buffer */

	for (wc = 1, s = 0; s < 13; s++) {		/* Process all characters in the entry */
		uc = ld_word(dir + LfnOfs[s]);		/* Pick an LFN character */
		if (wc != 0) {
			if (i >= FF_MAX_LFN + 1) return 0;	/* Buffer overflow? */
			lfnbuf[i++] = wc = uc;			/* Store it */
		} else {
			if (uc != 0xFFFF) return 0;		/* Check filler */
		}
	}

	if (dir[LDIR_Ord] & LLEF && wc != 0) {	/* Put terminator if it is the last LFN part and not terminated */
		if (i >= FF_MAX_LFN + 1) return 0;	/* Buffer overflow? */
		lfnbuf[i] = 0;
	}

	return 1;		/* The part of LFN is valid */
}
#endif


#if !FF_FS_READONLY
/*-----------------------------------------*/
/* FAT-LFN: Create an entry of LFN entries */
/*-----------------------------------------*/

static void put_lfn (
	const WCHAR* lfn,	/* Pointer to the LFN */
	BYTE* dir,			/* Pointer to the LFN entry to be created */
	BYTE ord,			/* LFN order (1-20) */
	BYTE sum			/* Checksum of the corresponding SFN */
)
{
	UINT i, s;
	WCHAR wc;


	dir[LDIR_Chksum] = sum;			/* Set checksum */
	dir[LDIR_Attr] = AM_LFN;		/* Set attribute. LFN entry */
	dir[LDIR_Type] = 0;
	st_word(dir + LDIR_FstClusLO, 0);

	i = (ord - 1) * 13;				/* Get offset in the LFN working buffer */
	s = wc = 0;
	do {
		if (wc != 0xFFFF) wc = lfn[i++];	/* Get an effective character */
		st_word(dir + LfnOfs[s], wc);		/* Put it */
		if (wc == 0) wc = 0xFFFF;			/* Padding characters for following items */
	} while (++s < 13);
	if (wc == 0xFFFF || !lfn[i]) ord |= LLEF;	/* Last LFN part is the start of LFN sequence */
	dir[LDIR_Ord] = ord;			/* Set the LFN order */
}

#endif	/* !FF_FS_READONLY */
#endif	/* FF_USE_LFN */



#if FF_USE_LFN && !FF_FS_READONLY
/*-----------------------------------------------------------------------*/
/* FAT-LFN: Create a Numbered SFN                                        */
/*-----------------------------------------------------------------------*/

static void gen_numname (
	BYTE* dst,			/* Pointer to the buffer to store numbered SFN */
	const BYTE* src,	/* Pointer to SFN in directory form */
	const WCHAR* lfn,	/* Pointer to LFN */
	UINT seq			/* Sequence number */
)
{
	BYTE ns[8], c;
	UINT i, j;
	WCHAR wc;
	DWORD sreg;


	memcpy(dst, src, 11);	/* Prepare the SFN to be modified */

	if (seq > 5) {	/* In case of many collisions, generate a hash number instead of sequential number */
		sreg = seq;
		while (*lfn) {	/* Create a CRC as hash value */
			wc = *lfn++;
			for (i = 0; i < 16; i++) {
				sreg = (sreg << 1) + (wc & 1);
				wc >>= 1;
				if (sreg & 0x10000) sreg ^= 0x11021;
			}
		}
		seq = (UINT)sreg;
	}

	/* Make suffix (~ + hexdecimal) */
	i = 7;
	do {
		c = (BYTE)((seq % 16) + '0'); seq /= 16;
		if (c > '9') c += 7;
		ns[i--] = c;
	} while (i && seq);
	ns[i] = '~';

	/* Append the suffix to the SFN body */
	for (j = 0; j < i && dst[j] != ' '; j++) {	/* Find the offset to append */
		if (dbc_1st(dst[j])) {	/* To avoid DBC break up */
			if (j == i - 1) break;
			j++;
		}
	}
	do {	/* Append the suffix */
		dst[j++] = (i < 8) ? ns[i++] : ' ';
	} while (j < 8);
}
#endif	/* FF_USE_LFN && !FF_FS_READONLY */



#if FF_USE_LFN
/*-----------------------------------------------------------------------*/
/* FAT-LFN: Calculate checksum of an SFN entry                           */
/*-----------------------------------------------------------------------*/

static BYTE sum_sfn (
	const BYTE* dir		/* Pointer to the SFN entry */
)
{
	BYTE sum = 0;
	UINT n = 11;

	do {
		sum = (sum >> 1) + (sum << 7) + *dir++;
	} while (--n);
	return sum;
}

#endif	/* FF_USE_LFN */



#if FF_FS_EXFAT
/*-----------------------------------------------------------------------*/
/* exFAT: Checksum                                                       */
/*-----------------------------------------------------------------------*/

static WORD xdir_sum (	/* Get checksum of the directoly entry block */
	const BYTE* dir		/* Directory entry block to be calculated */
)
{
	UINT i, szblk;
	WORD sum;


	szblk = (dir[XDIR_NumSec] + 1) * SZDIRE;	/* Number of bytes of the entry block */
	for (i = sum = 0; i < szblk; i++) {
		if (i == XDIR_SetSum) {	/* Skip 2-byte sum field */
			i++;
		} else {
			sum = ((sum & 1) ? 0x8000 : 0) + (sum >> 1) + dir[i];
		}
	}
	return sum;
}



static WORD xname_sum (	/* Get check sum (to be used as hash) of the file name */
	const WCHAR* name	/* File name to be calculated */
)
{
	WCHAR chr;
	WORD sum = 0;


	while ((chr = *name++) != 0) {
		chr = (WCHAR)ff_wtoupper(chr);		/* File name needs to be up-case converted */
		sum = ((sum & 1) ? 0x8000 : 0) + (sum >> 1) + (chr & 0xFF);
		sum = ((sum & 1) ? 0x8000 : 0) + (sum >> 1) + (chr >> 8);
	}
	return sum;
}


#if !FF_FS_READONLY && FF_USE_MKFS
static DWORD xsum32 (	/* Returns 32-bit checksum */
	BYTE  dat,			/* Byte to be calculated (byte-by-byte processing) */
	DWORD sum			/* Previous sum value */
)
{
	sum = ((sum & 1) ? 0x80000000 : 0) + (sum >> 1) + dat;
	return sum;
}
#endif



/*-----------------------------------*/
/* exFAT: Get a directry entry block */
/*-----------------------------------*/

static FRESULT load_xdir (	/* FR_INT_ERR: invalid entry block */
	DIR* dp					/* Reading direcotry object pointing top of the entry block to load */
)
{
	FRESULT res;
	UINT i, sz_ent;
	BYTE *dirb = dp->obj.fs->dirbuf;	/* Pointer to the on-memory direcotry entry block 85+C0+C1s */


	/* Load file directory entry */
	res = move_window(dp->obj.fs, dp->sect);
	if (res != FR_OK) return res;
	if (dp->dir[XDIR_Type] != ET_FILEDIR) return FR_INT_ERR;	/* Invalid order */
	memcpy(dirb + 0 * SZDIRE, dp->dir, SZDIRE);
	sz_ent = (dirb[XDIR_NumSec] + 1) * SZDIRE;
	if (sz_ent < 3 * SZDIRE || sz_ent > 19 * SZDIRE) return FR_INT_ERR;

	/* Load stream extension entry */
	res = dir_next(dp, 0);
	if (res == FR_NO_FILE) res = FR_INT_ERR;	/* It cannot be */
	if (res != FR_OK) return res;
	res = move_window(dp->obj.fs, dp->sect);
	if (res != FR_OK) return res;
	if (dp->dir[XDIR_Type] != ET_STREAM) return FR_INT_ERR;	/* Invalid order */
	memcpy(dirb + 1 * SZDIRE, dp->dir, SZDIRE);
	if (MAXDIRB(dirb[XDIR_NumName]) > sz_ent) return FR_INT_ERR;

	/* Load file name entries */
	i = 2 * SZDIRE;	/* Name offset to load */
	do {
		res = dir_next(dp, 0);
		if (res == FR_NO_FILE) res = FR_INT_ERR;	/* It cannot be */
		if (res != FR_OK) return res;
		res = move_window(dp->obj.fs, dp->sect);
		if (res != FR_OK) return res;
		if (dp->dir[XDIR_Type] != ET_FILENAME) return FR_INT_ERR;	/* Invalid order */
		if (i < MAXDIRB(FF_MAX_LFN)) memcpy(dirb + i, dp->dir, SZDIRE);
	} while ((i += SZDIRE) < sz_ent);

	/* Sanity check (do it for only accessible object) */
	if (i <= MAXDIRB(FF_MAX_LFN)) {
		if (xdir_sum(dirb) != ld_word(dirb + XDIR_SetSum)) return FR_INT_ERR;
	}
	return FR_OK;
}


/*------------------------------------------------------------------*/
/* exFAT: Initialize object allocation info with loaded entry block */
/*------------------------------------------------------------------*/

static void init_alloc_info (
	FATFS* fs,		/* Filesystem object */
	FFOBJID* obj	/* Object allocation information to be initialized */
)
{
	obj->sclust = ld_dword(fs->dirbuf + XDIR_FstClus);		/* Start cluster */
	obj->objsize = ld_qword(fs->dirbuf + XDIR_FileSize);	/* Size */
	obj->stat = fs->dirbuf[XDIR_GenFlags] & 2;				/* Allocation status */
	obj->n_frag = 0;										/* No last fragment info */
}



#if !FF_FS_READONLY || FF_FS_RPATH != 0
/*------------------------------------------------*/
/* exFAT: Load the object's directory entry block */
/*------------------------------------------------*/

static FRESULT load_obj_xdir (
	DIR* dp,			/* Blank directory object to be used to access containing direcotry */
	const FFOBJID* obj	/* Object with its containing directory information */
)
{
	FRESULT res;

	/* Open object containing directory */
	dp->obj.fs = obj->fs;
	dp->obj.sclust = obj->c_scl;
	dp->obj.stat = (BYTE)obj->c_size;
	dp->obj.objsize = obj->c_size & 0xFFFFFF00;
	dp->obj.n_frag = 0;
	dp->blk_ofs = obj->c_ofs;

	res = dir_sdi(dp, dp->blk_ofs);	/* Goto object's entry block */
	if (res == FR_OK) {
		res = load_xdir(dp);		/* Load the object's entry block */
	}
	return res;
}
#endif


#if !FF_FS_READONLY
/*----------------------------------------*/
/* exFAT: Store the directory entry block */
/*----------------------------------------*/

static FRESULT store_xdir (
	DIR* dp				/* Pointer to the direcotry object */
)
{
	FRESULT res;
	UINT nent;
	BYTE *dirb = dp->obj.fs->dirbuf;	/* Pointer to the direcotry entry block 85+C0+C1s */

	/* Create set sum */
	st_word(dirb + XDIR_SetSum, xdir_sum(dirb));
	nent = dirb[XDIR_NumSec] + 1;

	/* Store the direcotry entry block to the directory */
	res = dir_sdi(dp, dp->blk_ofs);
	while (res == FR_OK) {
		res = move_window(dp->obj.fs, dp->sect);
		if (res != FR_OK) break;
		memcpy(dp->dir, dirb, SZDIRE);
		dp->obj.fs->wflag = 1;
		if (--nent == 0) break;
		dirb += SZDIRE;
		res = dir_next(dp, 0);
	}
	return (res == FR_OK || res == FR_DISK_ERR) ? res : FR_INT_ERR;
}



/*-------------------------------------------*/
/* exFAT: Create a new directory enrty block */
/*-------------------------------------------*/

static void create_xdir (
	BYTE* dirb,			/* Pointer to the direcotry entry block buffer */
	const WCHAR* lfn	/* Pointer to the object name */
)
{
	UINT i;
	BYTE nc1, nlen;
	WCHAR wc;


	/* Create file-directory and stream-extension entry */
	memset(dirb, 0, 2 * SZDIRE);
	dirb[0 * SZDIRE + XDIR_Type] = ET_FILEDIR;
	dirb[1 * SZDIRE + XDIR_Type] = ET_STREAM;

	/* Create file-name entries */
	i = SZDIRE * 2;	/* Top of file_name entries */
	nlen = nc1 = 0; wc = 1;
	do {
		dirb[i++] = ET_FILENAME; dirb[i++] = 0;
		do {	/* Fill name field */
			if (wc != 0 && (wc = lfn[nlen]) != 0) nlen++;	/* Get a character if exist */
			st_word(dirb + i, wc); 	/* Store it */
			i += 2;
		} while (i % SZDIRE != 0);
		nc1++;
	} while (lfn[nlen]);	/* Fill next entry if any char follows */

	dirb[XDIR_NumName] = nlen;		/* Set name length */
	dirb[XDIR_NumSec] = 1 + nc1;	/* Set secondary count (C0 + C1s) */
	st_word(dirb + XDIR_NameHash, xname_sum(lfn));	/* Set name hash */
}

#endif	/* !FF_FS_READONLY */
#endif	/* FF_FS_EXFAT */



#if FF_FS_MINIMIZE <= 1 || FF_FS_RPATH >= 2 || FF_USE_LABEL || FF_FS_EXFAT
/*-----------------------------------------------------------------------*/
/* Read an object from the directory                                     */
/*-----------------------------------------------------------------------*/

#define DIR_READ_FILE(dp) dir_read(dp, 0)
#define DIR_READ_LABEL(dp) dir_read(dp, 1)

static FRESULT dir_read (
	DIR* dp,		/* Pointer to the directory object */
	int vol			/* Filtered by 0:file/directory or 1:volume label */
)
{
	FRESULT res = FR_NO_FILE;
	FATFS *fs = dp->obj.fs;
	BYTE attr, b;
#if FF_USE_LFN
	BYTE ord = 0xFF, sum = 0xFF;
#endif

	while (dp->sect) {
		res = move_window(fs, dp->sect);
		if (res != FR_OK) break;
		b = dp->dir[DIR_Name];	/* Test for the entry type */
		if (b == 0) {
			res = FR_NO_FILE; break; /* Reached to end of the directory */
		}
#if FF_FS_EXFAT
		if (fs->fs_type == FS_EXFAT) {	/* On the exFAT volume */
			if (FF_USE_LABEL && vol) {
				if (b == ET_VLABEL) break;	/* Volume label entry? */
			} else {
				if (b == ET_FILEDIR) {		/* Start of t