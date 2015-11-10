 
#include <stdio.h>
#include <windows.h>
#include <stdlib.h>

#define U8     unsigned char
#define S8     char
#define U16   unsigned short
#define U32   unsigned int
#define U64   unsigned __int64
#define PARTITION_TBL_POS	0x1BE
// MBR �������� 0~446 Byte ���Ŀ� ���� Partition Table�� �����ϹǷ� ���� ���
/*
* ------------- ����ü���� -----------------
* PARTITION : partition �� ������ ������� ����ü
* FAT32_BPB : BPB ��������������ѱ���ü
* VolStruct : FAT32_BPB ���� �ʿ��� ������ �����ϱ����� ����ü
* DitEntry : Directory Entry �� ������ �����ϱ� ���� ����ü
* LongDirEntry : LFN�� Directory Entry�� �����ϱ� ���� ����ü
*/

#pragma pack(1)
// �����Ϸ��Ǳ���ü�е����������Ѽ���
typedef struct _PARTITION{
	/*
	* --------- ����ü�������� ------------
	* BootFlag : ��Ƽ���� ���� ���� ���� Ȯ�� 0x80 ���� ����, 0x00 ���� �Ұ���
	* CHS_Start : CHS ����� ��Ƽ�� ���� ����
	* Type : ��Ƽ�� ����
	* CHS_End : CHS ����� ��Ƽ�� ���� ����
	* LBA_Start : LBA ����� ��Ƽ�� ���� ����
	* length : ������ �� ���� ��
	* ---------------------------------------
	*/
	U8 BootFlag;
	U8 CHS_Start[3];
	U8 type;
	U8 CHS_End[3];
	U32 LBA_Start;
	U32 length;
}PARTITION, *PPARTITION;
#pragma pack()

#pragma pack(1)
typedef struct _FAT32_BPB_struct{
	/*
	* --------- ����ü�������� ------------
	* 1. BPB ���뿵��(1)
	* JmpBoot[3] : ��Ʈ�ڵ�������ϱ������ּ�
	* OEMName[8] : OEM ȸ���̸�
	* BytesPerSec : ���ʹ����Ʈ��
	* SecPerClus : Ŭ�����ʹ缽�ͼ�
	* RsvdSecCnt : ����ȿ����Ǽ��ͼ�
	* NumFATs : ��������FAT �����ǰ������Ϲ������� 2��
	* RootEntCnt : FAT32 ������'0'
	* TotSec16 : FAT32 ������'0'
	* Media : 0xF8
	* FATs16 : FAT32 ������'0'
	* SecPerTrk : '0' ���̻����������ʴ´�
	* NumHeads : '0' ���̻����������ʴ´�
	* HiddSec : '0' ���̻����������ʴ´�
	* TotSec32 : �����������ϴ��Ѽ��ͼ�
	* 2. BPB FAT32 ����
	* FATs32 : FAT �����Ǽ��ͼ�
	* ExtFlags : FAT Table �����Ѽ����� , �Ϲ�������0x00
	* FileSysVer : FAT32 �ǹ������� , 0x00
	* RootDirClus : ��Ʈ���丮Ŭ����������ġ
	* FileSysInfo : FSInfo ����ü����ġ���Ϲ������κ����� 1�����Ϳ���ġ
	* BootBakSec : BR �ǹ��Ŭ��������ġ
	* Reserved[12] : ����ȿ������� 0����ä������
	* DirNum : ���������ʴ´�
	* Reserv1 : 0 ����ä������
	* BootSign : 0x29 �ΰ���
	* VolID : �����ǽø����ȣ
	* volLabel[11] : �ش���Ƽ���Ǻ������̺�
	* FileSysType[8] : FAT32 ��¹��ڿ��̵���
	* 3. BPB ���뿵��(2)
	* BootCodeArea : BootCode ����ִ�.
	* Signature : BR �Ǽջ󿩺�Ȯ�������� 0xAA55 �ΰ���
	* ---------------------------------------
	*/
	U8 JmpBoot[3];
	U8 OEMName[8];
	U16 BytesPerSec;
	U8 SecPerClus;
	U16 RsvdSecCnt;
	U8 NumFATs;
	U16 RootEntCnt;
	U16 TotSec16;
	U8 Media;
	U16 FATs16;
	U16 SecPerTrk;
	U16 NumHeads;
	U32 HiddSec;
	U32 TotSec32;
	// BPB ���뿵��
	U32 FATs32;
	U16 ExtFlags;
	U16 FileSysVer;
	U32 RootDirClus;
	U16 FileSysInfo;
	U16 BootBakSec;
	U8 Reserved[12];
	U8 DirNum;
	U8 Reserv1;
	U8 BootSign;
	U32 VolID;
	U8 volLabel[11];
	U8 FileSysType[8];
	// BPB FAT32 ����
	U8 BootCodeArea[420];
	U16 Signature;
}FAT32_BPB;
#pragma pack()

#pragma pack(1)
typedef struct _VOL_struct{
	/*
	* --------- ����ü�������� ------------
	* Drive
	* VolBeginSec
	* FirstDataSec : ù��° ������ ����
	* RootDirSec : Root Directory�� ���� ����
	* RootDirSecCnt : Root Directory�� �� ���� ��
	* RootEntCnt : Root Directory�� Directory Entry ����
	* FATSize : FAT ������ ���� ��
	* FATStartSec : FAT ���� ������
	* TotalClusCnt : ������ �� Ŭ������ ��
	* TotalSec : ������ �� ����
	* DataSecSize : ������ ������ ���ͼ�
	* ClusterSize : Ŭ������ ũ��
	* SecPerClus : �Ѱ� Ŭ�������� ���� ��
	* ---------------------------------------
	*/
	U32 Drive;
	U32 VolBeginSec;
	U32 FirstDataSec;
	U32 RootDirSec;
	U32 RootEntCnt;
	U32 RootDirSecCnt;
	U32 FATSize;
	U32 FATStartSec;
	U32 TotalClusCnt;
	U32 TotalSec;
	U32 DataSecSize;
	U32 ClusterSize;
	U32 SecPerClus;
}VolStruct;
#pragma pack()

#pragma pack(1)
typedef struct _DIR_struct{
	/*
	* --------- ����ü�������� ------------
	* Name[11] : ����/���丮 �� + Ȯ����
	* Attr : ���� ���� ex) 0x04 �ý���, 0x10 Directory 0x20 Archive File 0xF0 LFNs
	* NTRes : 0 ����
	* CrtTimeTenth : ���� �ð�
	* CrtTime : ���� �ð�
	* CrtDate : ���� ����
	* LstAccDate : ���� ��¥
	* FstClusHi : ����/���丮�� ù ��° Ŭ�������� ���� 2Byte
	* WriteTime : ���� �ð�
	* WriteDate : ���� ����
	* FstClustLow : ����/���丮�� ù ��° Ŭ�������� ���� 2Byte
	* FileSize : ���� ũ��
	* ---------------------------------------
	*/
	U8 Name[11];
	U8 Attr;
	U8 NTRes;
	U8 CrtTimeTenth;
	U16 CrtTime;
	U16 CrtDate;
	U16 LstAccDate;
	U16 FstClusHi;
	U16 WriteTime;
	U16 WriteDate;
	U16 FstClustLow;
	U32 FileSize;
}DirEntry;
#pragma pack()

#pragma pack(1)
typedef struct _LONG_DIR_struct{
	/*
	* --------- ����ü�������� ------------
	* Order : LFN ���� ���� 
	* Name1+Name2+Name3 : ���� �� 
	* Attr : 0x0F ����
	* Type : �������� 0 rhwjd
	* chksum : Short Directory Entry �� Checksum ����
	& FstClusLo : 0 ����
	* ---------------------------------------
	*/
	U8 Order;
	U8 Name1[10];
	U8 Attr;
	U8 Type;
	U8 chksum;
	U8 Name2[12];
	U16 FstClusLo;
	U8 Name3[4];
}LongDirEntry;
#pragma pack()

/*
* ------------- �Լ�����-----------------
* HDD_read : ������ġ�κ��ͼ��͸��о�͸޸𸮿���������Լ�
* HDD_write : ������ġ�Ǽ��Ϳ������͸����������Լ�
* HexDump : Dump�� Memory�� Hex�� �����ֱ� ���� �Լ�(���˿�)
* get_partition : ��Ƽ�� ���� �� BPB �����ּ��� ȹ���� ���� �Լ�
* get_BPB_info : BPB ������������ü�������ϱ������Լ�
* print_longName : LFNs�� ����ϱ� ���� �Լ�
* show_dir : Root Directory Entry�� ����ϱ� ���� �Լ�(���˿�)
* show_del_dir : ������ Directory Entry�� ����ϱ� ���� �Լ�
* rec_file : ���� ������ ���� �Լ�
* rec_dir : ���丮 ������ ���� �Լ�
* -----------------------------------------
*/

U32 HDD_read(U8 drv, U32 SecAddr, U32 blocks, U8* buf);
U32 HDD_write(U8 drv, U32 SecAddr, U32 blocks, U8* buf);
void HexDump  (U8 *addr, U32 len);
void get_partition(PPARTITION pPartition, U8 pSecBuf[512]);
U32 get_BPB_info(FAT32_BPB* BPB, VolStruct* pVol);
void print_longName(LongDirEntry* pLongDir, U32 EntryNum);
U32 show_dir(DirEntry* pDir);
U32 show_del_dir(DirEntry* pDir);
U32 rec_file(DirEntry* pDir,U32 rec_entry, U32* fat_entry, U32 Flag, U32 upClus);
U32 rec_dir(DirEntry* pDir, U32 useClus);

VolStruct gVol;

int main()
{
	/*
	* ------------- ��������-----------------
	* mbr_buf : mbr ������ ������ ���� ����
	* bpb_buf : bpb ������ ������ ���� ����
	* fat_buf : fat ������ ������ ���� ����
	* pPartition_arr : ��Ƽ�� ���� ������ ���� ����ü ����
	* root_buf : Root Directory Entry ������ ������ ���� ����
	* sel_m_menu : ���� �޴������� ������ ���� ����
	* sel_rec_entry : �����ϰ��� �ϴ� ���丮 ��Ʈ�� �Է� ����
	* -----------------------------------------
	*/

	U32 sel_m_menu;
	U32 sel_rec_entry;

	U8 mbr_buf[512];
	U8 bpb_buf[512];
	U32 fat_buf[128];
	U8* root_buf;
	PARTITION pPartition_arr[50];

	gVol.Drive = 0x2;
	gVol.VolBeginSec = 0x0;
	// �ʱ� HDD ������ ���� ��ġ��ȣ�� ���� ���� �ʱ�ȭ

 	if(HDD_read(gVol.Drive, gVol.VolBeginSec, 1, mbr_buf)== 0)
	{
		printf( "Boot Sector Read Failed \n" );
		return 1;
	}
	// mbr ������ ������ ���� HDD_read �Լ� ȣ��
	
	get_partition(pPartition_arr, mbr_buf);
	// mbr ������ �̿��� ��Ƽ�� ���� ������ ���� get_partition �Լ� ȣ��
	
	gVol.VolBeginSec = pPartition_arr->LBA_Start;
	// ���� ��ġ�� Partition�� ���� ���ͷ� ����
	if(HDD_read(gVol.Drive, gVol.VolBeginSec, 1, bpb_buf)==0)
	{
		printf( "BPB Sector Read Failed \n");
		return 1;
	}
	// BPB ������ ������ ���� HDD_read �Լ� ȣ��

	if(get_BPB_info((FAT32_BPB *)bpb_buf, &gVol) == 0)
	{
		printf( "It is not FAT32 File System \n" );
		return 1;
	}
	// BPB ������ ������ ����ü�� �����ϱ� ���� get_BPB_info �Լ� ȣ��

	gVol.RootDirSecCnt = 10;
	gVol.RootEntCnt = 100;
	// Root Directory Entry �� ���͸� �о���� ���� ���� ����
	/*
	* ---------------- �����ؾ��� ���� ---------------- 
	* �� ������ ���� �ƴ϶� �������� ������ ó���� ����� �����ؾ���
    * ------------------------------------------------- 
	*/

	root_buf = (U8*)malloc(gVol.RootDirSecCnt*512);
	// Root Directory Entry ������ŭ�� �����Ҵ�

	if(HDD_read(gVol.Drive, gVol.RootDirSec, gVol.RootDirSecCnt, root_buf)==0)
	{
		printf("Root Directory Read Failed \n");
		return 1;
	}

	printf("============= USB Recovery Tool Ver.FAT32 =============\n");
	printf("1. Analyze USB \n");
	printf("2. Exit \n");
	printf("select 1 or 2 : ");
	scanf("%d", &sel_m_menu);
	// ���� �޴� ��� �� ���� �Է�

	switch(sel_m_menu)
	{
	case 1:
		show_del_dir((DirEntry*)root_buf);
		break;
	case 2:
		exit(1);
	}
	// ���� �޴� �Է� ������ ���� �б⸦ ���� switch ��
	// 1 : ������ ����/���丮 ���
	// 2 : ���α׷� ����

	printf("\n\n============= Recovery Mode =============\n");
	printf("�����ϰ��� �ϴ� ������ Entry Number�� �Է��ϼ��� : ");
	scanf("%d", &sel_rec_entry);
	// ���� ��� ��� �� ������ Directory Entry ����

	if(HDD_read(gVol.Drive,gVol.FATStartSec, 1, fat_buf)==0)
	{
		printf( "FAT Sector Read Failed \n");
		return 1;
	}
	// FAT ������ ������ ���� HDD_read �Լ� ȣ��

	rec_file((DirEntry*)root_buf,sel_rec_entry,fat_buf, 0, 0);
	// �������� ������ ���� rec_file �Լ� ȣ��
	
	return 0;
}

U32 HDD_read(U8 drv, U32 SecAddr, U32 blocks, U8* buf)
{
	/*
	* ------------- ��������-----------------
	* drv : �����ϰ����ϴ¹�����ġ
	* SecAddr : ���ۼ��ͼ��������Ѻ���
	* blocks : �о�ü��ͼ�
	* buf : �ش缽�Ϳ����� dumpdata
	* ret : �ش���ġ������������ȯ�뺯��
	* ldistanceLow : ����23bit ����
	* ldistanceHigh : ����9bit ����
	* dwpointer : ���ۼ���
	* bytestoread : ������������������
	* numread : ��������Ʈ��
	* cur_drv : �����ϰ����ϴµ���̺�� ex) \\.\PhysicalDrive0
	* g_hDevice : �����ڵ�
	* -----------------------------------------
	*/

	U32 ret;
	U32 ldistanceLow, ldistanceHigh, dwpointer, bytestoread, numread;
	char cur_drv[100];
	HANDLE g_hDevice;

	sprintf(cur_drv, "\\\\.\\PhysicalDrive%d" ,(U32)drv);
	// CreateFIle������PhysicalDrive�������������ѹ��ڿ�����

	g_hDevice = CreateFile(cur_drv, GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	// CreateFile��Ȱ����PhysicalDrive ���ٿ������ڵ����

	if(g_hDevice==INVALID_HANDLE_VALUE)      return 0;
	// �ش����̽����翩�ο������б�

	ldistanceLow = SecAddr << 9;
	ldistanceHigh = SecAddr >> (32-9);
	// ldistanceLow ���� 9 bit, ldistanceHigh ���� 23 bit ����

	dwpointer = SetFilePointer(g_hDevice, ldistanceLow, ( long *)&ldistanceHigh, FILE_BEGIN);
	// ���ۼ��ͼ���

	if(dwpointer != 0xFFFFFFFF)
	{
		bytestoread = blocks * 512; // �о�õ����ͱ���
		ret = ReadFile(g_hDevice, buf, bytestoread, ( unsigned long*)&numread, NULL);
		if(ret)       ret = 1;
		else ret = 0;
	}

	CloseHandle(g_hDevice);
	// �ڵ�����
	return ret;
}

U32 HDD_write(U8 drv, U32 SecAddr, U32 blocks, U8* buf)
{
	/*
	* ------------- ��������-----------------
	* drv : �����ϰ����ϴ¹�����ġ
	* SecAddr : ���ۼ��ͼ��������Ѻ���
	* blocks : �о�ü��ͼ�
	* buf : �ش缽�Ϳ����� dumpdata
	* ret : �ش���ġ������������ȯ�뺯��
	* ldistanceLow : ����23bit ����
	* ldistanceHigh : ����9bit ����
	* dwpointer : ���ۼ���
	* bytestoread : ������������������
	* numread : ��������Ʈ��
	* cur_drv : �����ϰ����ϴµ���̺�� ex) \\.\PhysicalDrive0
	* g_hDevice : �����ڵ�
	* -----------------------------------------
	*/

	U32 ret = 0;
	U32 ldistanceLow, ldistanceHigh, dwpointer, bytestoread, numread;
	char cur_drv[100];
	HANDLE g_hDevice;

	sprintf(cur_drv, "\\\\.\\PhysicalDrive%d" ,(U32)drv);
	// CreateFIle������PhysicalDrive�������������ѹ��ڿ�����

	g_hDevice = CreateFile(cur_drv, GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	// CreateFile��Ȱ����PhysicalDrive ���ٿ������ڵ����

	if(g_hDevice==INVALID_HANDLE_VALUE)      return 0;
	// �ش����̽����翩�ο������б�

	ldistanceLow = SecAddr << 9;
	ldistanceHigh = SecAddr >> (32-9);

	dwpointer = SetFilePointer(g_hDevice, ldistanceLow, ( long *)&ldistanceHigh, FILE_BEGIN);
	// ���ۼ��ͼ���

	if(dwpointer != 0xFFFFFFFF)
	{
		bytestoread = blocks * 512; // �о�õ����ͱ���
		ret = WriteFile(g_hDevice, buf, bytestoread, ( unsigned long*)&numread, NULL);
		if(ret)       ret = 1;
		else ret = 0;
	}

	CloseHandle(g_hDevice);
	// �ڵ�����
	return ret;
}

void HexDump  (U8 *addr, U32 len){

	U8            *s=addr, *endPtr=(U8*)((U32)addr+len);
	U32           i, remainder=len%16;

	printf( "\n Offset        Hex Value         Ascii value\n" );

	// print out 16 byte blocks.
	while (s+16<=endPtr){

		// offset ���
		printf( "0x%08lx  " , (long)(s-addr));

		// 16 bytes �����γ������
		for (i=0; i<16; i++){
			printf( "%02x ", s[i]);
		}
		printf( " ");

		for (i=0; i<16; i++){
			if (s[i]>=32 && s[i]<=125)printf("%c", s[i]);
			else printf("." );
		}
		s += 16;
		printf( "\n");
	}

	// Print out remainder.
	if (remainder){

		// offset ���
		printf( "0x%08lx  " , (long)(s-addr));

		// 16 bytes ����������ϰ��������
		for (i=0; i<remainder; i++){
			printf( "%02x ", s[i]);
		}
		for (i=0; i<(16-remainder); i++){
			printf( "   " );
		}

		printf( " ");
		for (i=0; i<remainder; i++){
			if (s[i]>=32 && s[i]<=125) printf("%c", s[i]);
			else   printf(".");
		}
		for (i=0; i<(16-remainder); i++){
			printf( " ");
		}
		printf( "\n");
	}
	return;
}      // HexDump.

void get_partition(PPARTITION pPartition, U8 pSecBuf[512])
{
	memcpy(pPartition, (pSecBuf + PARTITION_TBL_POS), sizeof(PARTITION));
	// Partition ����
}

U32 get_BPB_info(FAT32_BPB* BPB, VolStruct* pVol)
{
	if(BPB->RootEntCnt != 0 || BPB->Signature != 0xAA55)   return 0;
	// Root Directory & Signature ���̻��������Ǵ��Ͽ��б�

	pVol->TotalSec = BPB->TotSec32;
	// Get Total Sector
	pVol->FATSize = BPB->FATs32;
	// Get FAT Size
	pVol->FATStartSec = pVol->VolBeginSec + BPB->RsvdSecCnt;
	// Get FAT Start Sector
	pVol->RootEntCnt = BPB->RootEntCnt;
	//Get Root Directory Entry Count
	pVol->RootDirSec = pVol->VolBeginSec + BPB->RsvdSecCnt + (BPB->NumFATs * BPB->FATs32);
	//Get Root Directory Sector
	pVol->FirstDataSec = pVol->VolBeginSec + BPB->RsvdSecCnt + (BPB->NumFATs * pVol->FATSize) + pVol->RootDirSecCnt;
	// Get FAT Start Sector
	pVol->DataSecSize = pVol->TotalSec - (BPB->RsvdSecCnt + (BPB->NumFATs * pVol->FATSize) + pVol->RootDirSecCnt);
	// Get Size Of Data Area
	pVol->TotalClusCnt = pVol->DataSecSize / BPB->SecPerClus;
	//Get Total Cluster Count
	pVol->ClusterSize = BPB->SecPerClus * BPB->BytesPerSec;
	//Get Size Of Cluster
	pVol->SecPerClus = BPB->SecPerClus;
	// Get Sector Per Cluster
	return 1;
}

U32 show_dir(DirEntry* pDir)
{
	/*
	 * ------------- ��������-----------------
	 * i,j : �ݺ� ����
	 * LongEntryEn : LFNs ����� ����
	 * -----------------------------------------
	 */

	U32 i, j, LongEntryEn=0;

	for(i=0;i<=gVol.RootEntCnt;i++)
	{
		switch((U8) pDir[i].Name[0])
		{
		case 0x00:
			// End Of Entry
			return 1; 
		case 0xE5 :
			// delete of Entry
			continue;
		}
		// Directory Entry �� ���� Ȯ��
		
		if(pDir[i].Attr == 0x0F)
		{
			LongEntryEn = 1;
			// Long File Name Entry
			continue;
		}
		// LFNs Directory Entry ����

		printf("------------------------- Entry Number %d -------------------------\n",i);

		if(pDir[i].Attr == 0x10)
			printf("Directory Name : ");
		else
			printf("File Name : ");
		// Directory or File Name is

		if(LongEntryEn == 1)
		{
			print_longName((LongDirEntry*)pDir, i-1);
			LongEntryEn = 0;
		} // LFNs �� ����� ����/���丮 �̸� ���
		else
		{
			for(j=0;j<11;j++)
				printf("%c",pDir[i].Name[j]);
		} // Short Name ���
		printf("\n");

		printf("File Size : %d\n",pDir[i].FileSize);
		printf("Start Cluster : %d\n",(pDir[i].FstClustLow | pDir[i].FstClusHi << 16));
		// First Cluster Low/High �� ��ü ���
	}
	return 1;
}

void print_longName(LongDirEntry* pLongDir, U32 EntryNum)
{
	/*
	 * ------------- ��������-----------------
	 * filename : LFNs ���� ������ ��ġ�� ���� �迭(Name1 + Name2 + Name3)
	 * final : filename�� �����ڵ� ������� ������ ���� ���� ��
	 * nameOffset : LFNs Entry ������ Name%d �̵��� ���� ����
	 * -----------------------------------------
	 */
	wchar_t filename[512];
	char final[512];
	U32 nameOffset = 0;

	do{
		memcpy(&filename[nameOffset],pLongDir[EntryNum].Name1, 10);
		// Name1�� ����� �̸� ���ڿ� ����
		nameOffset += 5; 
		// Name1 �Ӽ����� Name2 �Ӽ����� �̵�
		memcpy(&filename[nameOffset],pLongDir[EntryNum].Name2, 12);
		// Name2�� ����� �̸� ���ڿ� ����
		nameOffset += 6;
		// Name2 �Ӽ����� Name3 �Ӽ����� �̵�
		memcpy(&filename[nameOffset],pLongDir[EntryNum].Name3, 4);
		// Name3�� ����� �̸� ���ڿ� ����
		nameOffset += 2;

	}while((pLongDir[EntryNum--].Order & 0x40)==0);
	// 0x40�� or ������ ���� ���� ������ LFNs

	filename[nameOffset] = 0x0000;
	// ���ڿ��� ���� �˸��� 0x0000 ����

	wcstombs(final,filename, 512);
	// �����ڵ�->�ƽ�Ű�ڵ�� ��ȯ
	printf("%s",final);
}

U32 show_del_dir(DirEntry* pDir)
{
	/*
	 * ------------- ��������-----------------
	 * i,j : �ݺ� ����
	 * LongEntryEn : LFNs ����� ����
	 * -----------------------------------------
	 */

	U32 i, j, LongEntryEn=0;

	for(i=0;i<=gVol.RootEntCnt;i++)
	{
		if((U8) pDir[i].Name[0] == 0x00)
			return 1;
		else if((U8) pDir[i].Name[0] != 0xE5)
			continue;
		// Directory Entry �� ���� Ȯ��
		
		if(pDir[i].Attr == 0x0F)
		{
			LongEntryEn = 1;
			// Long File Name Entry
			continue;
		}
		// LFNs Directory Entry ����

		printf("------------------------- Entry Number %d -------------------------\n",i);

		//if(pDir[i].Attr != 0x04
		if(pDir[i].Attr == 0x10)
			printf("Directory Name : ");
		else
			printf("File Name : ");
		// Directory or File Name is

		if(LongEntryEn == 1)
		{
			print_longName((LongDirEntry*)pDir, i-1);
			LongEntryEn = 0;
		} // LFNs �� ����� ����/���丮 �̸� ���
		else
		{
			for(j=0;j<11;j++)
				printf("%c",pDir[i].Name[j]);
		} // Short Name ���
		printf("\n");

		printf("File Size : %d\n",pDir[i].FileSize);
		printf("Start Cluster : %d\n",(pDir[i].FstClustLow | pDir[i].FstClusHi << 16));
		// First Cluster Low/High �� ��ü ���
	}
	return 1;
}

U32 rec_file(DirEntry* rData, U32 rec_entry, U32* fat_entry, U32 Flag, U32 upClus)
{
	/*
	 * ------------- ��������-----------------
	 * i : �ݺ� ����
	 * useClus : �� Ŭ������
	 * reset_buf : �� Ŭ������ �ʱ�ȭ�� ����
	 * rec_buf : ���� ���� ����� ����
	 * dir_buf : ���丮�� ������ ����� ���� ���丮�� Directory Entry ������ ���� ����
	 * FstClustNum : ù ��° Ŭ�����͸� �����ϱ� ���� ����
	 * StartSec : ���� ���� ���� ������ ���� ���� -> ���ʿ��� Root Directory Sector���� ������ ���������
	 *            �� ���� ���ʹ� Sub Directory���� ����Ǳ� ������ ������ X
	 * Flag : 0 -> Root Directory, 1 -> Sub Directory
	 * upClus : ���� ���丮�� Ŭ������ ��ȣ
	 * -----------------------------------------
	 */

	U32 i, useClus;
	U8 reset_buf[4096];
	U8 rec_buf[4096];
	U8 dir_buf[4096];
	U32 FstClustNum;
	U32 StartSec;


	memset(reset_buf, 0x00, sizeof(reset_buf)); // �ʱ�ȭ�� �޸� ����(0x00 ���� ��ü �ʱ�ȭ ��Ŵ)
	memset(rec_buf, 0x00, sizeof(rec_buf)); // ���� �޸� �ʱ�ȭ
	
	FstClustNum = rData[rec_entry].FstClustLow | rData[rec_entry].FstClusHi << 16;
	// First Cluster Total ���� 
	
	HDD_read(gVol.Drive, gVol.RootDirSec + (FstClustNum - 2)*gVol.SecPerClus, 8, rec_buf);
	// ���� ���� ����
	
	for(i=0;;i++)
	{
		if(fat_entry[i] == 0x00000000)
		{
			useClus = i;
			// �� Ŭ������ �߰� �� �ش� Ŭ������ ��ȣ useClus�� ����
			break;
		}	
	}
	// for(;;)
	// FAT ���������� �� Ŭ������ �˻�
	// -------------------------------------------------------------------------------------- ���� ���� ����(���� X)
	fat_entry[useClus] = 0x0FFFFFFF;
	// �� Ŭ�����͸� ��� Ŭ�����ͷ� ����
	rData[rec_entry].Name[0] = 'R';
	// ���� �ñ״��� 0xE5 -> R ���ڿ��� ����
	// -------------------------------------------------------------------------------------- ���� ���� ����(���� X)
	
	// -------------------------------------------------------------------------------------- ���� ���� ���� ����
	HDD_write(gVol.Drive, gVol.RootDirSec + (useClus-2)*gVol.SecPerClus, 8, reset_buf);
	// ����� Ŭ������ �ʱ�ȭ
	if(Flag == 0) 
	{	
		StartSec = gVol.RootDirSec;
		Flag = 1;
	}
	else
	{
		StartSec = gVol.RootDirSec + (upClus - 2 ) * gVol.SecPerClus;
	} // Root Directory�� Sub Directory �� ���� Directory Entry �� ���� �ּ� ������ ���� �б�

	rData[rec_entry].FstClustLow = useClus;
	// ���� ���� Ŭ������ -> ����� Ŭ������ 
	HDD_write(gVol.Drive, gVol.RootDirSec + (useClus-2)*gVol.SecPerClus, 8, rec_buf);
	// ����ص� ���� ���� ���ο� Ŭ�����Ϳ� ����
	HDD_write(gVol.Drive, StartSec, 8, rData);
	// ���� �ñ״��� -> R �ñ״�ó ����
	HDD_write(gVol.Drive, gVol.FATStartSec, 8, fat_entry);
	// �� Ŭ������ ->  ��� Ŭ������ ���� ���� ����
	// -------------------------------------------------------------------------------------- ���� ���� ���� ����
	
	/*
	* ---------------- �����ؾ��� ���� ---------------- 
	* �ش� ������ ũ�Ⱑ 1 Cluster �� �Ѿ ���� ����
	* ------------------------------------------------- 
	*/
	if(rData[rec_entry].Attr == 0x10)
	{		
		StartSec = gVol.RootDirSec + (FstClustNum - 2 ) * gVol.SecPerClus;
		// Sub Directory Entry �� ���� ��ġ
		HDD_read(gVol.Drive, StartSec, 8, dir_buf);
		// Sub Directory Entry Dump
		rec_dir(dir_buf, Flag, useClus);
		// Directory �� ���� ������ ���� rec_dir �Լ� ȣ��
	}// �ش� Directory Entry�� Directory �� ����� ����
	
	printf("%s ��/�� ���� �Ϸ� �Ǿ����ϴ�. \n", rData[rec_entry].Name);
	// ������ ����/���丮 ��� �Ϸ� �޼��� ���

	return 0;
}

U32 rec_dir(DirEntry* pDir, U32 Flag, U32 useClus)
{
	/*
	 * ------------- ��������-----------------
	 * i : �ݺ� ����
	 * pDir : Sub Directory �� Dump �� ��� ����
	 * U32 Flag : 0 -> Root Directory, 1 -> Sub Directory
	 * useClus : ���� ���丮�� Ŭ������ ��ȣ
	 * FstClustNum : �ش� Directory Entry �� ����ϴ� Ŭ������ ��ȣ( != useClus)
	 * fat_buf : fat Table Entry�� ������� ����
	 * -----------------------------------------
	 */

	U32 i;
	U32 FstClustNum;
	U32 fat_buf[128];

	for(i=0;pDir[i].Name[0]!=0x00;i++)
	{
		FstClustNum = pDir[i].FstClustLow | pDir[i].FstClusHi << 16;
		if(FstClustNum == 0x00000000 || pDir[i].Name[0] != 0xE5)
		{
			continue;
		}// Ŭ������ �Ҵ� ���� Ȯ��
		else
		{
			if(HDD_read(gVol.Drive,gVol.FATStartSec, 1, fat_buf)==0)
			{
				printf( "FAT Sector Read Failed \n");
				return 1;
			}
			// FAT ������ ������ ���� HDD_read �Լ� ȣ��
			rec_file(pDir, i, fat_buf, Flag, useClus);
		}
	}
	return 0;
}	 
