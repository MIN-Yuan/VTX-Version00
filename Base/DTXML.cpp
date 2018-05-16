#include "stdafx.h"
#include "DTStr.h"
#include "dtxml.h"
#include "LZW.h"

#include "DTDebug.h"
//#include "../PCQuasar/File/FileEngine.h"
#include "Zip.h"
#include "Unzip.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


const char * XML_FILE_HEADER		="<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n";
const char * XML_HEAD_FORMAT		="<%s>";
const char * XML_TAIL_FORMAT		="</%s>";
const char * XML_HEAD				="<";
const char * XML_HEAD2				="</";
const char * XML_TAIL				=">";
const char * XML_ENTER				="\r\n";
const char * XML_TAB				="\t";
const char * XML_ENCRYPTED_SYMBOL	="EN";


const char XML_TAG_FIRST_CHAR		='<';
const char XML_TAIL_TAG_SECOND_CHAR	='/';
const char XML_TAG_LAST_CHAR		='>';


#ifdef _UNICODE
const WCHAR* XML_FILE_ZIP_INTERNALNAME	=L"Content.xml" ;
#else
const CHAR* XML_FILE_ZIP_INTERNALNAME = "Content.xml" ;
#endif
//////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG

UINT XMLTestThreadProc(LPVOID lParam)
{
	for(int i = 0 ; i< 1000000 ; i++ )
	{
		CDTXML dtXML ;
		dtXML.Open("CONFIG\\WORLD\\AMERICA\\FORD\\MONDEO\\PCM\\DEFAULT.XML") ;
		Sleep(10) ;
#ifdef _PC_QUASAR
		DMSG(GMsgConfiger,("XML Encrypt/DeCrypt at  %d \r\n" ,i ));
#endif
		dtXML.Close() ;
	}
#ifdef _PC_QUASAR
	DMSG(GMsgConfiger,("XML Test done !\r\n"));
#endif
	return 1 ;

}

#endif
//////////////////////////////////////////////////////////////////////////
CDTXML::CDTXML()
{
	Tag=NULL;
	Tag=new XML_TAG[XML_MAX_DEPTH];
	if(Tag==NULL){
#ifdef _PC_QUASAR
		DMSG_ERROR("Heap for tag");
#endif
	}

	Buffer=0;
	BufferSize=0;
	Depth=0;

	ReturnBuffer=0;

	
	TagMatchCase=TRUE;		//标记区分大小写
	TagSupportWildcard=FALSE;	//标记使用支持通配符

	LastSearchedTagLen=0;
	LastSearchedTag=NULL;

	
	FileHandle=INVALID_HANDLE_VALUE;
	CurFilePath=NULL;

	IsEncrypted=0;

	Key0=1111;
	Key1=333;
	Key2=55;
}

char CDTXML::Close()
{

	if(Buffer){
		delete[] Buffer;
		Buffer = NULL ;
	}

	if(ReturnBuffer){
		delete[] ReturnBuffer;
		ReturnBuffer=NULL;
	}

	if(FileHandle!=INVALID_HANDLE_VALUE){
 		CloseHandle(FileHandle);
 		FileHandle=INVALID_HANDLE_VALUE;
 	}
	
	if(CurFilePath){
		delete[] CurFilePath;
		CurFilePath=NULL;
	}

	return 1;
}


CDTXML::~CDTXML()
{
	Close();

	if(Tag){
		delete []Tag;
	}
}

//add by fanze support packge file
VOID CDTXML::SetPackgeFlage(BOOL bPackge )
{
	BPackgeFile = bPackge ;
}

//add by fanze
BOOL CDTXML::AttachBuf(char *pBuf ,int nLen)
{
	if(!pBuf)
		return FALSE ;
	if(Buffer)
		delete[] Buffer ;
	
	BufferSize = nLen ;
	Buffer=new char[BufferSize+1]; //分配缓冲区
	if(Buffer==0){
		return 0;
	}


	//拷贝数据
	memcpy(Buffer ,pBuf ,nLen) ;
	Buffer[BufferSize] = '\0' ;
	return  TRUE ;
}


//以UNICODE方式打开
char CDTXML::OpenU(const WCHAR * file, unsigned long create_desp)
{

	//For reopen file..
	Close();

	//Can not be "" 
	if(wcslen(file)<4){//.xml
		return 0;
	}

#ifdef _NOUSEPACKEGE

	unsigned long read;


	
	if(create_desp==NULL){
		create_desp=OPEN_EXISTING;
	}
#ifdef _WIN32_WCE
	FileHandle=CreateFileW(file,GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ,NULL,create_desp,NULL,NULL);
#else
	FileHandle=CreateFileW(file,GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,NULL,create_desp,NULL,NULL);
#endif


	if(FileHandle==INVALID_HANDLE_VALUE)
	{
		DWORD dwErr = GetLastError() ; 
		return 0;
	}


	BufferSize=GetFileSize(FileHandle,NULL);
	Buffer=new char[BufferSize];
	if(Buffer==0){
		return 0;
	}

	SetFilePointer(FileHandle,0,NULL,FILE_BEGIN);	
	ReadFile(FileHandle,Buffer,BufferSize,&read,NULL);
	if(read!=BufferSize){
		Close();
		return 0;
	}
#else	//使用包
//////////////////////////////////////////////////////////////////////////
//add by fanze
	if(!GOpenFileU(CString(file) ,this ,FileHandle))
	{
		Close() ;
		return 0 ;
	}

//////////////////////////////////////////////////////////////////////////

#endif


	//记下已打开的文件名
	CurFilePath = NewCopyU(file);
	Depth = 0;

	//判断是否为加密文件
	if(memcmp(Buffer,XML_ENCRYPTED_SYMBOL,2)==0)
	{
		IsEncrypted=1;
		DecryptToBuffer();
		//检查TOP标记是否存在		
	}


	if(BufferSize == 0)
	{
		//为空文件时,加入文件头
		if(Buffer)
		{
			delete []Buffer;
			Buffer=NULL;
		}
		BufferSize=strlen(XML_FILE_HEADER);
		Buffer=new char[BufferSize];
		memcpy(Buffer,XML_FILE_HEADER,BufferSize);
	}
	

	//错误信息
	char msg[128];
	if(!CheckFile(msg)){
		char ansi_name[MAX_PATH];
		WideCharToMultiByte(CP_ACP,0,file,-1,ansi_name,MAX_PATH,NULL,NULL);
#ifdef _PC_QUASAR
		DMSG(GMsgConfiger,("Error, %s. %s",ansi_name,msg));
#endif

	}

// #ifdef _PC_QUASAR
// 	DMSG(GMsgConfiger,("CDTXML::OpenU Ret \r\n"));
// #endif


	return 1;
}
//ASCII方式打开
char CDTXML::Open(const char * file, unsigned long create_desp)
{
	WCHAR unicode_file[MAX_PATH];
	MultiByteToWideChar(CP_ACP,0,file,-1,unicode_file,MAX_PATH);//A2W
	return OpenU(unicode_file,create_desp);

}



//fanze
char CDTXML::DecryptToBuffer()
{
#ifdef _ENCRTYP_LZW
	PBYTE p  ;
	INT size = 0 ;

	if(!DecompressLZW((unsigned char*)Buffer+2,BufferSize-2, p, size)){
		return FALSE;
	}

	delete[] Buffer;
	Buffer=(char*)p;
	BufferSize=size;
#else //using ZIP
	PBYTE p ;
	INT size = 0 ;
	HZIP hZip ;
	ZIPENTRY ze ;
	int i = 0 ;
	hZip = OpenZip(Buffer ,BufferSize ,NULL) ;
	if(!hZip){
		return  0 ;
	}

	//Internal Name ,xml file in ZIP file name 

	if(FindZipItem(hZip,XML_FILE_ZIP_INTERNALNAME,true,&i,&ze) != ZR_OK )//can not found
	{
		CloseZip(hZip) ;
		return 0 ;
	}

	p = new BYTE[ze.unc_size] ;
	ZRESULT zr = UnzipItem(hZip ,i ,p ,ze.unc_size) ;//Unzip it to buffer
	if(zr != ZR_OK){
		CloseZip(hZip) ;
		delete[] p ;
		return 0 ;
	}
	
	CloseZip(hZip) ;

	delete[] Buffer;
	Buffer = (CHAR*)p ;
	BufferSize = ze.unc_size ;


#endif

	return TRUE;
}

char CDTXML::EncryptAndSaveToFile()//加密,并保存至文件中
{
	unsigned char* p =NULL ;
	INT size = 0 ;
	unsigned long written;
	TCHAR tBuf[MAX_PATH] ={0} ;

#ifdef _ENCRTYP_LZW
	if(!CompressLZW((PBYTE)Buffer,BufferSize,p,size)){
		return FALSE;
	}
#else //using ZIP

	TCHAR pszTempFile[MAX_PATH] ={0};
	TCHAR pszCopy[MAX_PATH] ={0} ;
	GetSystemDirectory(pszTempFile ,MAX_PATH) ;
	DWORD dwRand = GetTickCount() ;
#ifdef UNICODE	
	wsprintf(tBuf ,L"%s\\%d.tmp" ,pszTempFile  ,dwRand) ;
	wsprintf(pszCopy ,L"%s\\%d" ,pszTempFile  ,dwRand) ;

#else 
	sprintf(tBuf ,"%s\\%d.tmp" ,pszTempFile ,dwRand ) ;
	sprintf(pszCopy  ,"%s\\%d" ,pszTempFile ,dwRand) ;
#endif

	//创建临时文件，存放ZIP内容
	HZIP hz =	CreateZip(tBuf  ,NULL) ;
	if(!hz){
		return FALSE ;
	}

#ifdef _UNICODE	
	//将当前打开文件，复制一份
	CopyFile(CurFilePath ,pszCopy ,FALSE) ;
	if(ZipAdd(hz ,XML_FILE_ZIP_INTERNALNAME ,pszCopy) != ZR_OK){
		return FALSE ;
	}
#else
	INT buffersize = MAX_PATH ;
	CHAR buffer[MAX_PATH] ={0} ;
	WideCharToMultiByte(CP_ACP,0,CurFilePath,-1,buffer,buffersize,NULL,NULL);
	CopyFile(buffer ,pszCopy ,TRUE) ;
	ZRESULT zr = ZipAdd(hz ,XML_FILE_ZIP_INTERNALNAME ,pszCopy) ;
	if(zr != ZR_OK){
		char buf[255] ={0} ;
		FormatZipMessageZ( zr,buf ,255) ;
		AfxMessageBox(buf) ;
		return FALSE ;
	}
#endif

	//删除备份文件
	DeleteFile(pszCopy) ;
	CloseZip(hz) ;

#endif

	CFile file  ;
	if(!file.Open(tBuf,CFile::modeReadWrite ,NULL) ){
		return FALSE ;
	}
	size = file.GetLength() ;
	p = new unsigned char[size] ;
	ZeroMemory(p ,size) ;
	file.Read(p ,size) ;
	file.Close() ;

	//删除临时ZIP文件
	DeleteFile(tBuf) ;
	
	SetFilePointer(FileHandle,0,NULL,FILE_BEGIN);
	//写入已经加密的符号
	if(!WriteFile(FileHandle,XML_ENCRYPTED_SYMBOL,2,&written,NULL)){
		return FALSE;	
	}
	if(written!=2){
		return FALSE;
	}

	if(!WriteFile(FileHandle,p,size,&written,NULL)){
		return FALSE;
	}
	if((int)written!=size){
		return FALSE;
	}
	SetEndOfFile(FileHandle);
	delete[] p ;
	p= NULL; 

	return TRUE;
}


char CDTXML::Enter(const char * tag, unsigned int index)
{
	long b,e;
	long findb, finde;
	int taglen=strlen(tag);

	if(!BPackgeFile)//如果不是包文件
	{
		if(FileHandle==INVALID_HANDLE_VALUE){
			return 0;
		}
	}

	if(Depth>=XML_MAX_DEPTH){
		return 0;
	}

	if(taglen > XML_MAX_TAG_LEN){
		return 0;
	}

	if(Depth==0){//第一个标记
		findb=0;
		finde=BufferSize;
	}else{
		findb=Tag[Depth-1].Begin + strlen(Tag[Depth-1].Tag) + XML_HEAD_ADDED_LEN;
		finde=Tag[Depth-1].End;
	}

	if(!SearchTag(tag, taglen, findb, finde, &b,&e, index)){
		return 0;
	}

	strcpy(Tag[Depth].Tag,tag);

	Tag[Depth].Begin=b;
	Tag[Depth].End=e;
	Tag[Depth].Index=index;
	Depth++;

	return 1;
}

char CDTXML::Back(int step)
{
	if(!BPackgeFile)//如果不是包文件
	{
		if(FileHandle==INVALID_HANDLE_VALUE){
			return 0;
		}
	}

	if(step>Depth){
		return 0;
	}

	Depth-=step;
	return 1;
}

long CDTXML::GetInteger(const char * tag, long defaultvalue, unsigned int index)
{
	char buf[25];
	unsigned long outlen;
	if(!GetString(tag,25,buf,outlen,index)){
		return defaultvalue;
	}

	if(outlen<=0){
		return defaultvalue;
	}

	return atol(buf);
}

char CDTXML::GetInteger(long * value, const char * tag, unsigned int index)
{
	char buf[25];
	unsigned long outlen;
	if(!GetString(tag,25,buf,outlen,index)){
		return 0;
	}

	if(outlen<=0){
		return 0;
	}

	*value=atol(buf);
	return 1;
}

char * CDTXML::GetString(const char * tag, const char * defaultvalue, unsigned int index)
{
	long b,e;
	long findb, finde;
	
	if(ReturnBuffer){
		delete []ReturnBuffer;
		ReturnBuffer=0;
	}

	if(tag==NULL){
		ReturnBuffer=NewCopy(defaultvalue);
		return ReturnBuffer;
	}
	
	int taglen=strlen(tag);


	if(!BPackgeFile)//如果是包文件
	{
		if(FileHandle==INVALID_HANDLE_VALUE){
			ReturnBuffer=NewCopy(defaultvalue);
			return ReturnBuffer;
		}
	}


	if(taglen > XML_MAX_TAG_LEN){
		ReturnBuffer=NewCopy(defaultvalue);
		return ReturnBuffer;
	}

	if(Depth==0){//第一个标记
		findb=0;
		finde=BufferSize;
	}else{
		findb=Tag[Depth-1].Begin + strlen(Tag[Depth-1].Tag) + XML_HEAD_ADDED_LEN;
		finde=Tag[Depth-1].End;
	}

	if(!SearchTag(tag,taglen,findb, finde, &b,&e, index)){
		ReturnBuffer=NewCopy(defaultvalue);
		return ReturnBuffer;
	}

	b+=LastSearchedTagLen+XML_HEAD_ADDED_LEN;

	if(e-b<=0){
		ReturnBuffer=NewCopy(defaultvalue);
		return ReturnBuffer;
	}

	ReturnBuffer=new char[e-b+1];
	strncpy(ReturnBuffer,Buffer+b,e-b);
	ReturnBuffer[e-b]=0;

	return ReturnBuffer;
}

char CDTXML::GetString(const char * tag, unsigned int bufsize, char * buf, unsigned long & outlen, unsigned int index)
{
	long b,e;
	long findb, finde;

	if(tag==NULL){
		return 0;
	}

	int taglen=strlen(tag);

	if(!BPackgeFile)
	{
		if(FileHandle==INVALID_HANDLE_VALUE){
		return 0;
		}
	}

	if(taglen > XML_MAX_TAG_LEN){
		return 0;
	}

	if(Depth==0){//第一个标记
		findb=0;
		finde=BufferSize;
	}else{
		findb=Tag[Depth-1].Begin + strlen(Tag[Depth-1].Tag) + XML_HEAD_ADDED_LEN;
		finde=Tag[Depth-1].End;
	}

	if(!SearchTag(tag, taglen,findb, finde, &b,&e,index)){
		return 0;
	}

	if(e-b>=(long)bufsize){
		return 0;
	}

	buf[0]=0;
	b+=LastSearchedTagLen+XML_HEAD_ADDED_LEN;

	if(e-b<=0){
		return 0;
	}

	strncpy(buf,Buffer+b,e-b);
	buf[e-b]=0;

	outlen=e-b;

	return 1;
}

char * CDTXML::GetStringC(const char * tag, const char * tagadded, const char * defaultvalue, unsigned int index)
{
	long b,e;
	long findb, finde;
	char combined_tag[XML_MAX_TAG_LEN + XML_MAX_TAGADDED_LEN + XML_TAIL_ADDED_LEN + 1];
	
	if(ReturnBuffer){
		delete []ReturnBuffer;
		ReturnBuffer=0;
	}

	if(tag==NULL){
		ReturnBuffer=NewCopy(defaultvalue);
		return ReturnBuffer;
	}
	
	int taglen=strlen(tag);
	int tagaddedlen=0;

	if(!BPackgeFile)
	{
		if(FileHandle==INVALID_HANDLE_VALUE){
			ReturnBuffer=NewCopy(defaultvalue);
			return ReturnBuffer;
		}
		
	}

	if(taglen > XML_MAX_TAG_LEN){
		ReturnBuffer=NewCopy(defaultvalue);
		return ReturnBuffer;
	}


	strcpy(combined_tag, tag);
	if(tagadded){
		tagaddedlen=strlen(tagadded);
		if(tagaddedlen>0){
			if(tagaddedlen > XML_MAX_TAGADDED_LEN){
				ReturnBuffer=NewCopy(defaultvalue);
				return ReturnBuffer;
			}	
			strcat(combined_tag, tagadded);
		}
	}
	
	if(Depth==0){//第一个标记
		findb=0;
		finde=BufferSize;
	}else{
		findb=Tag[Depth-1].Begin + strlen(Tag[Depth-1].Tag) + XML_HEAD_ADDED_LEN;
		finde=Tag[Depth-1].End;
	}
	
	if(!SearchTag(combined_tag,taglen+tagaddedlen, findb, finde, &b,&e, index)){
		ReturnBuffer=NewCopy(defaultvalue);
		return ReturnBuffer;
	}

	b+=LastSearchedTagLen+XML_HEAD_ADDED_LEN;

	if(e-b<=0){
		ReturnBuffer=NewCopy(defaultvalue);
		return ReturnBuffer;
	}
	
	ReturnBuffer=new char[e-b+1];
	
	CopyMemory(ReturnBuffer,Buffer+b,e-b);
	ReturnBuffer[e-b]=0;
	
	return ReturnBuffer;

}


PTCHAR CDTXML::GetStringCU(const char * tag, const char * tagadded, LPCWSTR defaultvalue, unsigned int index)
{
	long b,e;
	long findb, finde;
	char combined_tag[XML_MAX_TAG_LEN + XML_MAX_TAGADDED_LEN + XML_TAIL_ADDED_LEN + 1];

	if(ReturnBuffer){
		delete []ReturnBuffer;
		ReturnBuffer=0;
	}

	if(tag==NULL){
		ReturnBuffer=(char *)NewCopyU(defaultvalue);
		return (PTCHAR)ReturnBuffer;
	}

	int taglen=strlen(tag);
	int tagaddedlen=0;
	
	if(!BPackgeFile)
	{
		if(FileHandle==INVALID_HANDLE_VALUE){
			ReturnBuffer=(char *)NewCopyU(defaultvalue);
			return (PTCHAR)ReturnBuffer;
		}
		
	}

	if(taglen > XML_MAX_TAG_LEN){
		ReturnBuffer=(char *)NewCopyU(defaultvalue);
		return (PTCHAR)ReturnBuffer;
	}


	strcpy(combined_tag, tag);
	if(tagadded){
		tagaddedlen=strlen(tagadded);
		if(tagaddedlen>0){
			if(tagaddedlen > XML_MAX_TAGADDED_LEN){
				ReturnBuffer=(char *)NewCopyU(defaultvalue);
				return (PTCHAR)ReturnBuffer;
			}
			strcat(combined_tag, tagadded);
		}
	}

	if(Depth==0){//第一个标记
		findb=0;
		finde=BufferSize;
	}else{
		findb=Tag[Depth-1].Begin + strlen(Tag[Depth-1].Tag) + XML_HEAD_ADDED_LEN;
		finde=Tag[Depth-1].End;
	}
	
	if(!SearchTag(combined_tag, taglen+tagaddedlen, findb, finde, &b,&e, index)){
		ReturnBuffer=(char *)NewCopyU(defaultvalue);
		return (PTCHAR)ReturnBuffer;
	}
	
	b+=LastSearchedTagLen+XML_HEAD_ADDED_LEN;

	if(e-b<=0){
		ReturnBuffer=(char *)NewCopyU(defaultvalue);
		return (PTCHAR)ReturnBuffer;
	}

	ReturnBuffer=(char *) new TCHAR[e-b+1];

	int rt=MultiByteToWideChar(CP_UTF8,0,Buffer+b,e-b,(PWCHAR)ReturnBuffer,e-b);
	((PTCHAR)ReturnBuffer)[rt]=0;

	return (PTCHAR)ReturnBuffer;
}

char CDTXML::GetStringCU(const char * tag, const char * tagadded, unsigned int bufsize, PWCHAR buf, unsigned int index)
{
	long b,e;
	long findb, finde;
	char combined_tag[XML_MAX_TAG_LEN + XML_MAX_TAGADDED_LEN + XML_TAIL_ADDED_LEN + 1];

	if(!BPackgeFile)
	{
		if(FileHandle==INVALID_HANDLE_VALUE){
			return 0;
		}
	}


	if(tag==0){
		return 0;
	}
	
	int taglen=strlen(tag);
	int tagaddedlen=0;

	if(taglen > XML_MAX_TAG_LEN){
		return 0;
	}

	strcpy(combined_tag, tag);
	if(tagadded){
		tagaddedlen=strlen(tagadded);
		if(tagaddedlen>0){
			if(tagaddedlen > XML_MAX_TAG_LEN-1){
				return 0;
			}
			strcat(combined_tag, tagadded);
		}
	}	

	if(Depth==0){//第一个标记
		findb=0;
		finde=BufferSize;
	}else{
		findb=Tag[Depth-1].Begin + strlen(Tag[Depth-1].Tag) + XML_HEAD_ADDED_LEN;
		finde=Tag[Depth-1].End;
	}
	
	if(!SearchTag(combined_tag, taglen+tagaddedlen, findb, finde, &b,&e,index)){
		return 0;
	}

	b+=LastSearchedTagLen+XML_HEAD_ADDED_LEN;

	if(e-b>=(long)bufsize){
		return 0;
	}

	if(e-b<=0){
		return 0;
	}

	MultiByteToWideChar(CP_UTF8,0,Buffer+b,e-b,buf,bufsize);

	return 1;
}



unsigned int CDTXML::GetChildCount(const char * tag)
{
	long i;
	unsigned int found=0;
	long begin, end;
	char head[XML_MAX_TAG_LEN + XML_TAIL_ADDED_LEN + XML_MAX_TAGADDED_LEN + 1];
	size_t headlen;
	int enteredsub=0;
	long bcount=0;

	if(!BPackgeFile)
	{
		if(FileHandle==INVALID_HANDLE_VALUE){
			return 0;
		}

	}

	if(strlen(tag) > XML_MAX_TAG_LEN){
		return 0;
	}

	sprintf(head,XML_HEAD_FORMAT,tag);
	headlen=strlen(head);

	if(Depth==0){
		begin=0;
		end=BufferSize;
	}else{
		begin=Tag[Depth-1].Begin+headlen;
		end=Tag[Depth-1].End;
	}

	for(i=begin; i<end; i++){
		if(Buffer[i]==XML_TAG_FIRST_CHAR){
			if(Buffer[i+1]!='!' && Buffer[i+1]!='?'){
				if(strncmp(Buffer+i,head,headlen)==0){
					if(enteredsub==0){
						bcount++;
					}
				}

				if(Buffer[i+1]==XML_TAIL_TAG_SECOND_CHAR){
					enteredsub--;
				}else{
					enteredsub++;
				}
			}
		}
	}

	return bcount;
}


char CDTXML::SearchTag(const char * tag, int taglen, long begin, long end, long * b, long * e, unsigned int index)
{
	if(TagMatchCase){
		if(TagSupportWildcard){
			if(!SearchTagMatchCaseWithWildcard(tag,taglen,begin,end,b,e,index)){
				return 0;
			}
		}else{
			if(!SearchTagMatchCaseWithoutWildcard(tag,taglen,begin,end,b,e,index)){
				return 0;
			}
		}
	}else{
		if(TagSupportWildcard){
			if(!SearchTagUnmatchCaseWithWildcard(tag,taglen,begin,end,b,e,index)){
				return 0;
			}
		}else{
			if(!SearchTagUnmatchCaseWithoutWildcard(tag,taglen,begin,end,b,e,index)){
				return 0;
			}
		}
	}


	//记录LastSearchedTag


	return 1;
}

char CDTXML::SearchTagMatchCaseWithoutWildcard(const char * tag, int taglen, long begin, long end, long * b, long * e, unsigned int index)
{
	long i;
	//int headlen, taillen;
	int enteredsub=0;
	long foundb,founde;
	//char head[XML_MAX_TAG_LEN + XML_HEAD_ADDED_LEN + XML_MAX_TAGADDED_LEN + 1];
	//char tail[XML_MAX_TAG_LEN + XML_TAIL_ADDED_LEN + XML_MAX_TAGADDED_LEN + 1];
	long bcount=-1,ecount=-1;	//找到的INDEX
	
	int actualtaglen;	//实际在文件中的标记长度

	//if(strlen(tag) > XML_MAX_TAG_LEN){
	//	return 0;
	//}

	//sprintf(head,XML_HEAD_FORMAT,tag);
	//sprintf(tail,XML_TAIL_FORMAT,tag);
	//headlen=strlen(head);
	//taillen=strlen(tail);

	for(i=begin; i<end; i++){
		if(Buffer[i]==XML_TAG_FIRST_CHAR){
			if(Buffer[i+1]!='!' && Buffer[i+1]!='?'){
				if(Buffer[i+1]==XML_TAIL_TAG_SECOND_CHAR){
					enteredsub--;
				}else{

					//找出实际在文件中的标记长度
					for(actualtaglen=1; actualtaglen<XML_MAX_TAG_LEN; actualtaglen++){
						if(Buffer[i+actualtaglen]==XML_TAG_LAST_CHAR){
							break;
						}
					}
					actualtaglen--;
					

					if(actualtaglen==taglen){
						if(enteredsub==0){
							if(strncmp(tag,Buffer+i+1,taglen)==0){
								foundb=i;
								bcount++;
								if(bcount>=(long)index){
									break;
								}
							}
						}
					}

					enteredsub++;
				}
			}
		}
	}

	if(bcount!=(long)index){
		return 0;
	}

	//找尾标记时，找到foundb后面的第一个就就可以了
	for(i=foundb+taglen+2; i<end; i++){
		if(Buffer[i]==XML_TAG_FIRST_CHAR){
			if(Buffer[i+1]!='!' && Buffer[i+1]!='?'){
				if(Buffer[i+1]==XML_TAIL_TAG_SECOND_CHAR){

					//找出实际在文件中的标记长度
					for(actualtaglen=2; actualtaglen<XML_MAX_TAG_LEN; actualtaglen++){
						if(Buffer[i+actualtaglen]==XML_TAG_LAST_CHAR){
							break;
						}
					}
					actualtaglen-=2;
					
					
					if(actualtaglen==taglen){					
						if(enteredsub==0){
							if(strncmp(tag,Buffer+i+2,taglen)==0){
								founde=i;
								ecount++;
								break;
							}
						}
					}

					enteredsub--;
				}else{
					enteredsub++;
				}
			}
		}
	}

	//判断是否找到foundb后的第一个
	if(ecount!=0){
		return 0;
	}

	*b=foundb;
	*e=founde;

	//记录LastSearchedTag
	LastSearchedTag=Buffer+foundb+1;
	LastSearchedTagLen=taglen;
	
	return 1;
}

char CDTXML::SearchTagUnmatchCaseWithoutWildcard(const char * tag, int taglen, long begin, long end, long * b, long * e, unsigned int index)
{
	long i;
	//int headlen, taillen;
	int enteredsub=0;
	long foundb,founde;
	//char head[XML_MAX_TAG_LEN + XML_HEAD_ADDED_LEN + XML_MAX_TAGADDED_LEN + 1];
	//char tail[XML_MAX_TAG_LEN + XML_TAIL_ADDED_LEN + XML_MAX_TAGADDED_LEN + 1];
	long bcount=-1,ecount=-1;

	int actualtaglen;	//实际在文件中的标记长度

	//if(taglen > XML_MAX_TAG_LEN){
	//	return 0;
	//}
	
	//sprintf(head,XML_HEAD_FORMAT,tag);
	//sprintf(tail,XML_TAIL_FORMAT,tag);
	
	//headlen=strlen(head);
	//taillen=strlen(tail);
	
	for(i=begin; i<end; i++){
		if(Buffer[i]==XML_TAG_FIRST_CHAR){
			if(Buffer[i+1]!='!' && Buffer[i+1]!='?'){				
				if(Buffer[i+1]==XML_TAIL_TAG_SECOND_CHAR){
					enteredsub--;
				}else{

					//找出实际在文件中的标记长度
					for(actualtaglen=1; actualtaglen<XML_MAX_TAG_LEN; actualtaglen++){
						if(Buffer[i+actualtaglen]==XML_TAG_LAST_CHAR){
							break;
						}
					}
					actualtaglen--;

					if(actualtaglen==taglen){
						if(enteredsub==0){
							if(_strnicmp(tag,Buffer+i+1,taglen)==0){
								foundb=i;
								bcount++;
								if(bcount>=(long)index){
									break;
								}
							}
						}
					}

					enteredsub++;
				}
			}
		}
	}
	
	if(bcount!=(long)index){
		return 0;
	}
	
	//找尾标记时，找到foundb后面的第一个就就可以了
	for(i=foundb+taglen+2; i<end; i++)
	{
		if(Buffer[i]==XML_TAG_FIRST_CHAR)
		{
			if(Buffer[i+1]!='!' && Buffer[i+1]!='?')
			{				
				if(Buffer[i+1]==XML_TAIL_TAG_SECOND_CHAR){
					//找出实际在文件中的标记长度
					for(actualtaglen=2; actualtaglen<XML_MAX_TAG_LEN; actualtaglen++)
					{
						if(Buffer[i+actualtaglen]==XML_TAG_LAST_CHAR)
						{
							break;
						}
					}
					actualtaglen-=2;


					if(actualtaglen==taglen)
					{
						if(enteredsub==0)
						{
							if(_strnicmp(tag,Buffer+i+2,taglen)==0)
							{
								founde=i;
								ecount++;
								break;
							}
						}
					}

					enteredsub--;
				}else{
					enteredsub++;
				}
			}
		}
	}
	
	//判断是否找到foundb后的第一个
	if(ecount!=0)
	{
		return 0;
	}
	
	*b=foundb;
	*e=founde;

	//记录LastSearchedTag
	LastSearchedTag=Buffer+foundb+1;
	LastSearchedTagLen=taglen;

	return 1;
}

char CDTXML::SearchTagMatchCaseWithWildcard(const char * tag, int taglen, long begin, long end, long * b, long * e, unsigned int index)
{
	long i;
	//int headlen, taillen;
	int enteredsub=0;
	long foundb,founde;
	//char head[XML_MAX_TAG_LEN + XML_HEAD_ADDED_LEN + XML_MAX_TAGADDED_LEN + 1];
	//char tail[XML_MAX_TAG_LEN + XML_TAIL_ADDED_LEN + XML_MAX_TAGADDED_LEN + 1];
	long bcount=-1,ecount=-1;
	
	int actualtaglen;	//实际在文件中的标记长度
	
	//if(strlen(tag) > XML_MAX_TAG_LEN){
	//	return 0;
	//}
	
	//sprintf(head,XML_HEAD_FORMAT,tag);
	//sprintf(tail,XML_TAIL_FORMAT,tag);
	//headlen=strlen(head);
	//taillen=strlen(tail);

	for(i=begin; i<end; i++){
		if(Buffer[i]==XML_TAG_FIRST_CHAR){
			if(Buffer[i+1]!='!' && Buffer[i+1]!='?'){				
				if(Buffer[i+1]==XML_TAIL_TAG_SECOND_CHAR){
					enteredsub--;
				}else{
	
					//找出实际在文件中的标记长度
					for(actualtaglen=1; actualtaglen<XML_MAX_TAG_LEN; actualtaglen++){
						if(Buffer[i+actualtaglen]==XML_TAG_LAST_CHAR){
							break;
						}
					}
					actualtaglen--;
					
					if(actualtaglen>0){
						if(enteredsub==0){
							if(WildcardMatch(tag, taglen, Buffer+i+1,actualtaglen)){
								foundb=i;
								bcount++;
								if(bcount>=(long)index){
									break;
								}
							}
						}
					}

					enteredsub++;
				}
			}
		}
	}
	
	//如果没有找到指定index的那一个
	if(bcount!=(long)index){
		return 0;
	}


	//找尾标记时，找到foundb后面的第一个就就可以了
	for(i=foundb+taglen+2; i<end; i++){
		if(Buffer[i]==XML_TAG_FIRST_CHAR){
			if(Buffer[i+1]!='!' && Buffer[i+1]!='?'){
				if(Buffer[i+1]==XML_TAIL_TAG_SECOND_CHAR){

					//找出实际在文件中的标记长度
					for(actualtaglen=2; actualtaglen<XML_MAX_TAG_LEN; actualtaglen++){
						if(Buffer[i+actualtaglen]==XML_TAG_LAST_CHAR){
							break;
						}
					}
					actualtaglen-=2;
				
					if(actualtaglen>0){
						if(enteredsub==0){
							if(WildcardMatch(tag,taglen,Buffer+i+2,actualtaglen)){
								founde=i;
								ecount++;
								break;
							}
						}
					}

					enteredsub--;
				}else{
					enteredsub++;
				}
			}
		}
	}
	
	//判断是否找到foundb后的第一个
	if(ecount!=0){
		return 0;
	}
	
	*b=foundb;
	*e=founde;

	//记录LastSearchedTag
	LastSearchedTag=Buffer+foundb+1;
	LastSearchedTagLen=actualtaglen;

	return 1;
}


char CDTXML::SearchTagUnmatchCaseWithWildcard(const char * tag, int taglen, long begin, long end, long * b, long * e, unsigned int index)
{
	long i;
	//int headlen, taillen;
	int enteredsub=0;
	long foundb,founde;
	//char head[XML_MAX_TAG_LEN + XML_HEAD_ADDED_LEN + XML_MAX_TAGADDED_LEN + 1];
	//char tail[XML_MAX_TAG_LEN + XML_TAIL_ADDED_LEN + XML_MAX_TAGADDED_LEN + 1];
	long bcount=-1,ecount=-1;

	int actualtaglen;	//实际在文件中的标记长度
	
	//if(strlen(tag) > XML_MAX_TAG_LEN){
	//	return 0;
	//}
	
	//sprintf(head,XML_HEAD_FORMAT,tag);
	//sprintf(tail,XML_TAIL_FORMAT,tag);
	
	//headlen=strlen(head);
	//taillen=strlen(tail);
	
	for(i=begin; i<end; i++){
		if(Buffer[i]==XML_TAG_FIRST_CHAR){
			if(Buffer[i+1]!='!' && Buffer[i+1]!='?'){
		
				if(Buffer[i+1]==XML_TAIL_TAG_SECOND_CHAR){
					enteredsub--;
				}else{
					//找出实际在文件中的标记长度
					for(actualtaglen=1; actualtaglen<XML_MAX_TAG_LEN; actualtaglen++){
						if(Buffer[i+actualtaglen]==XML_TAG_LAST_CHAR){
							break;
						}
					}
					actualtaglen--;

					if(actualtaglen>0){
						if(enteredsub==0){
							if(WildcardMatchI((char*)tag,taglen,Buffer+i+1,actualtaglen)){
								foundb=i;
								bcount++;
								if(bcount>=(long)index){
									break;
								}
							}
						}
					}

					enteredsub++;
				}
			}
		}
	}
	
	if(bcount!=(long)index){
		return 0;
	}
	
	//找尾标记时，找到foundb后面的第一个就就可以了
	for(i=foundb+taglen+2; i<end; i++){
		if(Buffer[i]==XML_TAG_FIRST_CHAR){
			if(Buffer[i+1]!='!' && Buffer[i+1]!='?'){
				
				if(Buffer[i+1]==XML_TAIL_TAG_SECOND_CHAR){
					//找出实际在文件中的标记长度
					for(actualtaglen=2; actualtaglen<XML_MAX_TAG_LEN; actualtaglen++){
						if(Buffer[i+actualtaglen]==XML_TAG_LAST_CHAR){
							break;
						}
					}
					actualtaglen-=2;

					if(actualtaglen>0){
						if(enteredsub==0){
							if(WildcardMatchI(tag,taglen,Buffer+i+2,actualtaglen)){
								founde=i;
								ecount++;
								break;
							}
						}
					}

					enteredsub--;
				}else{
					enteredsub++;
				}
			}
		}
	}
	
	//判断是否找到foundb后的第一个
	if(ecount!=0){
		return 0;
	}
	
	*b=foundb;
	*e=founde;

	//记录LastSearchedTag
	LastSearchedTag=Buffer+foundb+1;
	LastSearchedTagLen=actualtaglen;
	
	return 1;
}


char CDTXML::SetInteger(const char * tag, long l, unsigned int index)
{
	char buf[16];

	sprintf(buf,"%ld",l);

	if(!SetString(tag, buf, index)){
		return 0;
	}

	return 1;
}

double CDTXML::GetFloat(const char * tag, double defaultvalue, unsigned int index)
{
	double f;
	if(!GetFloat(tag, &f, index)){
		return defaultvalue;
	}

	return f;
}

char CDTXML::GetFloat(const char * tag, double *f, unsigned int index)
{
	char buf[32];
	unsigned long outlen;

	if(!GetString(tag,32,buf,outlen,index)){
		return 0;
	}

	if(outlen<=0){
		return 0;
	}

	*f=atof(buf);

	return 1;
}


char CDTXML::SetFloat(const char * tag, double f, unsigned int index)
{
	char buf[32];
	
	sprintf(buf,"%f",f);

	if(!SetString(tag,buf,index)){
		return 0;
	}

	return 1;
}

char CDTXML::SetStringC(const char * tag, const char * tagadded, PCHAR str, unsigned int index)
{
	char combined_tag[XML_MAX_TAG_LEN + XML_MAX_TAGADDED_LEN + XML_TAIL_ADDED_LEN + 1];
	char rt;
	
	if(BPackgeFile)
	{
		if(FileHandle==INVALID_HANDLE_VALUE){
			return 0;
		}
	}
	
	if(strlen(tag) > XML_MAX_TAG_LEN){
		return 0;
	}
	
	if(strlen(tagadded) > XML_MAX_TAGADDED_LEN){
		return 0;
	}
	
	strcpy(combined_tag, tag);
	if(tagadded){
		strcat(combined_tag,tagadded);
	}
		
	rt=SetString(combined_tag, str, index);
	
	return rt;
}

char CDTXML::SetStringCU(const char * tag, const char * tagadded, PWCHAR str, unsigned int index)
{
	char * p=0;
	INT len;
	char combined_tag[XML_MAX_TAG_LEN + XML_MAX_TAGADDED_LEN + XML_TAIL_ADDED_LEN + 1];
	char rt;
	
	if(!BPackgeFile)
	{
		if(FileHandle==INVALID_HANDLE_VALUE){
			return 0;
		}

	}

	if(strlen(tag) > XML_MAX_TAG_LEN){
		return 0;
	}
	
	if(strlen(tagadded) > XML_MAX_TAGADDED_LEN){
		return 0;
	}
	
	strcpy(combined_tag, tag);
	if(tagadded){
		strcat(combined_tag,tagadded);
	}

	len=wcslen(str);
	p=new char[(len+1)*4];	//Unicode转UTF8,最大4倍字数.
	if(p==0){
		return 0;
	}

	WideCharToMultiByte(CP_UTF8,0,str,len,p,(len+1)*4,NULL,NULL);

	rt=SetString(combined_tag, p, index);

	//Has been ASSERT before
	delete []p;

	return rt;
}

char CDTXML::SetString(const char * tag, const char * str, unsigned int index)
{
	INT tag_len;
	INT str_len;
	long b,e;
	char * p=0;
	INT newlen;
	INT i;
	unsigned char org_depth;
	long findb, finde;

	tag_len=strlen(tag);
	str_len=strlen(str);


	if(tag_len > XML_MAX_TAG_LEN){
		return 0;
	}

	if(Depth==0){//第一个标记
		findb=0;
		finde=BufferSize;
	}else{
		findb=Tag[Depth-1].Begin + strlen(Tag[Depth-1].Tag) + XML_HEAD_ADDED_LEN;
		finde=Tag[Depth-1].End;
	}

	if(!SearchTag(tag, tag_len, findb, finde, &b,&e, index)){
		b=finde;
		e=finde;
	}


	if(e==b){ //新增的元素

		//如果新建第二个以上的元素，看看，前面的是否存在。
		if(index>0){
			if(GetChildCount(tag)!=index){
				return 0;
			}
		}

		//     tag           content   tab     enter
		newlen=tag_len*2+5 + str_len+  Depth + 2;

		p=new char[b+newlen+(BufferSize-e)+1];	//加一个字符串操作0符
		if(p==0){
			return 0;
		}

		//将前一部分放入
		memcpy(p, Buffer, b);

		p[b]=0;

		if(Depth>0){
			strcat(p+b,XML_TAB);
		}

		strcat(p+b,XML_HEAD);
		strcat(p+b,tag);
		strcat(p+b,XML_TAIL);
		strcat(p+b,str);
		strcat(p+b,XML_HEAD2);
		strcat(p+b,tag);
		strcat(p+b,XML_TAIL);
		
		//if(Depth>0){	//如果0级时,未尾不加回车符
			strcat(p+b,XML_ENTER);	
		//}

		for(i=0; i<Depth-1; i++){
			strcat(p+b,XML_TAB);
		}

		//将后一部分放入
		memcpy(p+b+newlen,Buffer+e, BufferSize-e);
	}else{	//旧的元素

		//e为尾标记地址,e再向后偏移"尾标记"长度,b~e为将被替换的段
		e+=LastSearchedTagLen;
		e+=3;

		newlen=tag_len*2+str_len+5;

		p=new char[b+newlen+(BufferSize-e)+1];	//加一个字符串操作0符
		if(p==0){
			return 0;
		}

		//将前一部分放入
		memcpy(p, Buffer, b);

		p[b]=0;
		strcat(p+b,XML_HEAD);
		strcat(p+b,tag);
		strcat(p+b,XML_TAIL);
		strcat(p+b,str);
		strcat(p+b,XML_HEAD2);
		strcat(p+b,tag);
		strcat(p+b,XML_TAIL);

		memcpy(p+b+newlen,Buffer+e, BufferSize-e);
	}

	if(Buffer){
		delete []Buffer;
	}

	//重新设置文件Buffer
	Buffer=p;
	BufferSize=b+newlen+(BufferSize-e);

	//重新Enter前面的所有标记
	org_depth=Depth;
	if(!Back(Depth)){
		return 0;
	}

	for(i=0; i<org_depth; i++){
		if(!Enter(Tag[i].Tag, Tag[i].Index)){
			return 0;
		}
	}

	//重新写入文件中
	/*
	if(FileHandle==0){
		return 0;
	}

	fseek(FileHandle,b,SEEK_SET);
	fwrite(Buffer+b,1,BufferSize-b,FileHandle);
	chsize(fileno(FileHandle),BufferSize);
	fflush(FileHandle);
	*/

	return 1;
}


char CDTXML::Save()
{
	unsigned long written;

	if(!BPackgeFile)
	{
		if(FileHandle==INVALID_HANDLE_VALUE){
			return 0;
		}
		
	}

	if(IsEncrypted){
		EncryptAndSaveToFile();
	}else{
		SetFilePointer(FileHandle,0,NULL,FILE_BEGIN);
		WriteFile(FileHandle,Buffer,BufferSize,&written,NULL);	
		if(written!=BufferSize){
			return 0;
		}
		SetEndOfFile(FileHandle);
	}

	return 1;
}


char * CDTXML::NewCopy(const char * str)
{
	char * p;
	p=new char[strlen(str)+1];
	strcpy(p,str);
	return p;
}

PWCHAR CDTXML::NewCopyU(LPCWSTR str)
{
	PWCHAR p;
	p=new WCHAR[wcslen(str)+1];
	wcscpy(p,str);
	return p;
}

unsigned long CDTXML::GenSerial(unsigned long pos)
{
	unsigned long result;
	result=pos*pos*Key0+pos*Key1+Key2;
	return result;
}

BOOL CDTXML::IsValidTagChar(char c)
{
// 	if(
// 		(c>='0' && c<='9') ||
// 		(c>='a' && c<='z') ||
// 		(c>='A' && c<='Z') ||
// 		(c=='_')
// 		){
// 		return TRUE;
// 	}
	if(c=='<'){
		return FALSE;
	}
	return TRUE;
}


BOOL CDTXML::CheckFile(char * buffer128)
{
	UINT i,j;
	long b,e;

	char tag[XML_MAX_DEPTH][XML_MAX_TAG_LEN + XML_TAIL_ADDED_LEN + XML_MAX_TAGADDED_LEN + 1];
	int mem_line[XML_MAX_DEPTH];
	int mem_pos[XML_MAX_DEPTH];
	char buf[XML_MAX_TAG_LEN + XML_TAIL_ADDED_LEN + XML_MAX_TAGADDED_LEN + 1];

	BOOL have_mem;
	int curline=1;
	int curcol=1;
	int depth=0;


	
	for(i=0; i<BufferSize-2; i++){
		if(Buffer[i]==XML_TAG_FIRST_CHAR){
			i++;
			if(Buffer[i]=='!'){
			}else if(Buffer[i]=='?'){
			}else if(Buffer[i]=='/'){
				i++;
				curcol++;
				//Find out tag b e;
				b=i;
				e=i;
				for(j=b; j<BufferSize; j++){
					if(Buffer[j]==XML_TAG_LAST_CHAR){
						e=j;
						break;
					}
					if(!IsValidTagChar(Buffer[j])){
						sprintf(buffer128,"Err in TAIL, line %d col %d, below %s\r\n",curline, curcol, tag[depth-1]);
						return FALSE;
					}
				}
				if(e<=b){
					sprintf(buffer128,"TAIL miss END, line %d col %d, below %s\r\n",curline, curcol, tag[depth-1]);					
					return FALSE;
				}
				if(e-b>XML_MAX_TAG_LEN + XML_MAX_TAGADDED_LEN){
					sprintf(buffer128,"TAIL too long, MAX_TAG_LEN=%d, line %d col %d, below %s\r\n",XML_MAX_TAG_LEN + XML_MAX_TAGADDED_LEN,curline, curcol,tag[depth-1]);					
					return FALSE;					
				}
				strncpy(buf,Buffer+b,e-b);
				buf[e-b]=0;

				if(depth<1){
					sprintf(buffer128,"PLS write HEAD first, line %d col %d %s\r\n",curline, curcol, buf);					
					return FALSE;
				}
				if(strcmp(tag[depth-1],buf)!=0){

					//Check wheater have mem before...
					have_mem=0;
					if(depth>=2){
						for(j=0; j<(UINT)depth-1; j++){
							if(strcmp(tag[j],buf)==0){
								have_mem=1;
								break;
							}
						}
					}

					if(have_mem){ //report prev tag error
						sprintf(buffer128,"TAIL not matched, line %d col %d %s\r\n",mem_line[depth-1], mem_pos[depth-1], tag[depth-1]);					
					}else{ //report cur tat error
						sprintf(buffer128,"TAIL not matched, line %d col %d %s\r\n",curline, curcol, buf);					
					}
					return FALSE;
				}
				depth--;
			}else{
				//Find out tag b e;
				b=i;
				e=i;
				for(j=b; j<BufferSize; j++){
					if(Buffer[j]==XML_TAG_LAST_CHAR){
						e=j;
						break;
					}
					if(!IsValidTagChar(Buffer[j])){
						sprintf(buffer128,"Err in HEAD, line %d col %d\r\n",curline, curcol);
						return FALSE;
					}
				}
				if(e<=b){
					sprintf(buffer128,"HEAD miss END, line %d col %d\r\n",curline, curcol);					
					return FALSE;
				}
				if(e-b>XML_MAX_TAG_LEN + XML_MAX_TAGADDED_LEN){
					sprintf(buffer128,"HEAD too long, MAX_TAG_LEN=%d, line %d col %d\r\n",XML_MAX_TAG_LEN + XML_MAX_TAGADDED_LEN,curline, curcol);					
					return FALSE;					
				}

				if(depth>=XML_MAX_DEPTH){
					sprintf(buffer128,"Too deep, line %d col %d %s\r\n",curline, curcol, tag[depth]);					
					return FALSE;
				}

				strncpy(tag[depth], Buffer+b,e-b);
				mem_line[depth]=curline;
				mem_pos[depth]=curcol;
				tag[depth][e-b]=0;

				depth++;
			}
			
		}


		if(Buffer[i]=='\n'){
			curline++;		
			curcol=1;
		}

		curcol++;
	}

	if(depth!=0){
		sprintf(buffer128,"Tag not matched, line %d col %d\r\n",curline, curcol);					
		return FALSE;
	}

	return TRUE;
}

// long CDTXML::SearchStr(unsigned long b, unsigned long e, const char * str, unsigned int index)
// {
// 	unsigned long i;
// 	long pos;
// 	long found=-1;
// 	size_t len;
// 
// 	len=strlen(str);
// 
// 	if(len<1){
// 		return -1;
// 	}
// 
// 	for(i=b; i<e; i++){
// 		if(strncmp(Buffer+i,str,len)==0){
// 			pos=i;
// 			found++;
// 
// 			if(found>=(long)index){
// 				return pos;
// 			}
// 		}
// 	}
// 
// 	return -1;
// }


char CDTXML::AddChild(const char * tag, unsigned int index)
{
	char str[XML_MAX_TAG_LEN + XML_MAX_TAGADDED_LEN + XML_MAX_DEPTH];
	int i;

	//插入一内容为<TAG>回车TabTab</TAG>
	if(Depth>=XML_MAX_DEPTH){
		return 0;
	}

	strcpy(str,XML_ENTER);

	for(i=0; i<Depth; i++){
		strcat(str,XML_TAB);
	}

	return SetString(tag,str,index);
}

char CDTXML::RemoveChild(const char * tag, unsigned int index)
{
	INT tag_len;
	long b,e;
	char * p=0;
	INT i;
	unsigned char org_depth;
	long findb, finde;
	unsigned long written;

	tag_len=strlen(tag);


	if(tag_len > XML_MAX_TAG_LEN){
		return 0;
	}

	if(Depth==0){//第一个标记
		findb=0;
		finde=BufferSize;
	}else{
		findb=Tag[Depth-1].Begin + strlen(Tag[Depth-1].Tag) + XML_HEAD_ADDED_LEN;
		finde=Tag[Depth-1].End;
	}

	if(!SearchTag(tag, tag_len, findb, finde, &b,&e, index)){
		return 0;
	}


	//将B设置在标记前面的回车符前,删除部分==回车+n个tag+标记
	while(Buffer[b-1]!=XML_TAG_LAST_CHAR && b>0) b--;

	//e为尾标记地址,e再向后偏移"尾标记"长度,b~e为将被删除的段
	e+=LastSearchedTagLen;
	e+=3;

	p=new char[b+(BufferSize-e)];	//加一个字符串操作0符
	if(p==0){
		return 0;
	}

	memcpy(p, Buffer, b);
	memcpy(p+b,Buffer+e, BufferSize-e);
	
	if(Buffer){
		delete []Buffer;
	}

	//重新设置文件Buffer
	Buffer=p;
	BufferSize=b+(BufferSize-e);

	//重新Enter前面的所有标记
	org_depth=Depth;
	if(!Back(Depth)){
		return 0;
	}

	for(i=0; i<org_depth; i++){
		if(!Enter(Tag[i].Tag, Tag[i].Index)){
			return 0;
		}
	}

	//重新写入文件中
	if(!BPackgeFile)
	{
		if(FileHandle==INVALID_HANDLE_VALUE){
			return 0;
		}
		
	}

	//fseek(FileHandle,b,SEEK_SET);
	SetFilePointer(FileHandle,b,NULL,FILE_BEGIN);

	//fwrite(Buffer+b,1,BufferSize-b,FileHandle);
	WriteFile(FileHandle, Buffer+b, BufferSize-b, &written, NULL);

	//chsize(fileno(FileHandle),BufferSize);
	SetEndOfFile(FileHandle);	

	//fflush(FileHandle);

	return 1;
}


char CDTXML::IsOpen()
{
	if(!BPackgeFile)
	{
		if(FileHandle==INVALID_HANDLE_VALUE){
			return 0;
		}
		
	}

	return 1;
}


void CDTXML::SetEncrypt(char encrypt)
{
	IsEncrypted=encrypt; 
}

char CDTXML::GetEncrypt()
{
	return IsEncrypted;
}

char CDTXML::SetKey(long key0, long key1, long key2)
{
	Key0=key0;
	Key1=key1;
	Key2=key2;

	return TRUE;
}

//不能去除,否则保存后,注示就没了
//char CDTXML::RemoveHtmlStyleRemark()
//{
//	if(Buffer==0 || BufferSize==0){
//		return 0;			
//	}
//
//	long b=-1;
//	long e=-1;
//
//	unsigned long i;
//	unsigned long cursize=BufferSize;
//
//	for(i=0; i<cursize-1; i++){
//
//		//Find <!
//		if(Buffer[i]=='<' && Buffer[i+1]=='!'){
//			b=i;
//		}
//
//		//Find >
//		if(b>-1 && Buffer[i]=='>'){
//			e=i+1;
//			memcpy(Buffer+b, Buffer+e, cursize-e);
//			cursize-=(e-b);
//			i=b-1;
//			b=-1;
//			e=-1;
//		}
//	}
//
//	if(cursize<BufferSize){
//		char * p=0;
//		p=new char[cursize];
//		if(p==0){
//
//			BufferSize=cursize;
//			return 0;
//		}
//		memcpy(p,Buffer,cursize);
//
//		delete[]Buffer;
//		Buffer=p;
//		BufferSize=cursize;
//	}
//	
//	return 1;
//}

BOOL CDTXML::GetCurrentFilePath(CHAR * buffer, INT buffersize)
{
	if(!BPackgeFile)
	{
		if(FileHandle==INVALID_HANDLE_VALUE){
			return 0;
		}
		
	}
	
	if(CurFilePath==NULL){
		return FALSE;
	}

	return WideCharToMultiByte(CP_ACP,0,CurFilePath,-1,buffer,buffersize,NULL,NULL);

}

BOOL CDTXML::GetCurrentFilePathU(WCHAR * buffer, INT buffersize)
{
	if(!BPackgeFile)
	{
		if(FileHandle==INVALID_HANDLE_VALUE){
			return 0;
		}
		
	}

	if(CurFilePath==NULL){
		return FALSE;
	}

	if((INT)wcslen(CurFilePath)>=buffersize){
		return FALSE;
	}

	wcscpy(buffer,CurFilePath);

	return TRUE;
}

void CDTXML::SetTagMatchCase(BOOL matchcase)
{
	TagMatchCase=matchcase;
}

void CDTXML::SetTagSupportWildcard(BOOL supportwildcard)
{
	TagSupportWildcard=supportwildcard;
}

BOOL CDTXML::WildcardMatch(const char * wildcard, int wildcardn, const char * str, INT strn)
{
	char * pstr=NULL;
	BOOL result;
	
	if(wildcard==NULL){
		return FALSE;
	}
	
	if(wildcardn==0){
		return FALSE;
	}
	
	if(str==NULL){
		return FALSE;
	}
	
	if(strn==0){
		return FALSE;
	}
		
	
	pstr=new char[strn+1];
	if(pstr==NULL){
		return FALSE;
	}
	strncpy(pstr,str,strn);
	pstr[strn]='\0';

	
	CDTStr tempstr;
	result=tempstr.Match((char*)wildcard,pstr);
	
	delete []pstr;
	
	return result;
}


BOOL CDTXML::WildcardMatchI(const char * wildcard, int wildcardn, const char * str, INT strn)
{
	char * pwildcard=NULL;
	char * pstr=NULL;
	BOOL result;

	if(wildcard==NULL){
		return FALSE;
	}

	if(wildcardn==0){
		return FALSE;
	}

	if(str==NULL){
		return FALSE;
	}

	if(strn==0){
		return FALSE;
	}

	pwildcard=new char[wildcardn+1];
	if(pwildcard==NULL){
		return FALSE;
	}
	strncpy(pwildcard,wildcard,wildcardn);
	pwildcard[wildcardn]='\0';


	pstr=new char[strn+1];
	if(pstr==NULL){
		delete []pwildcard;
		return FALSE;
	}
	strncpy(pstr,str,strn);
	pstr[strn]='\0';


	_strupr(pwildcard);
	_strupr(pstr);
	
	CDTStr tempstr;
	result=tempstr.Match(pwildcard,pstr);

	delete []pwildcard;
	delete []pstr;

	return result;
}


BOOL CDTXML::GetLastSearchedTag(CHAR * buffer, INT bufsize)
{
	if(LastSearchedTagLen<=0){
		return FALSE;
	}

	if(LastSearchedTag==NULL){
		return FALSE;
	}

	if(LastSearchedTagLen>=bufsize){
		return FALSE;
	}

	strncpy(buffer,LastSearchedTag,LastSearchedTagLen);
	buffer[LastSearchedTagLen]=0;

	return TRUE;
}



