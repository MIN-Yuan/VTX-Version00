/////////////////////////////////////////////////////////////////////////////
//Module: XML file read & write & check
//Platform: Win32, DOS, PureC/C++
//Authur: Deng.Ting ting@ode.cn
//Edition: 2008-02-25
//
//2008-02-25: Apped GetLastSearchedTag()供外部使用,当使用Wildcard Tag Enter 或 Get之后,外部获取实际的标记名.
//2008-02-24: Append SetTagSupportWildcard() & SetTagmatchcase(), 为了独立性,把CDTStr中的Match()和Scan抄过来了
//2008-01-10: Append SetStringC()
//2008-01-10: Improved Open()' Share para
//2007-07-20: Improved CheckFile It could locate Line and Col.
//2007-07-10: Modified GetStringU support addedtag==NULL
//2007-06-28: Modified to use std c type
//2007-06-21: OpenAlways(), If file isn't exist, crate file UTF-8
//2007-06-20: For Unicode <-> UTF8 GetStringU SetStringU
//2007-06-19: File Operating By API
//2007-05-28: XML GetChildCount BUG
//2007-05-23: BUG fixed, GetString中没有将buf[0]=0,就strncpy
//2007-05-21: 增加了IsOpen
//2007-05-21: CheckFile 匹配，进入/退出 测试
//2007-05-19: SearchTag()修改了求能父子同名的BUG
//2007-02-25: Append Save() method
//2007-02-08: CheckFile On Open.
/////////////////////////////////////////////////////////////////////////////
#ifndef K_DT_XML_H
#define K_DT_XML_H


//////////////////////////////////////////////////////////////////////////
#define XML_MAX_TAG_LEN	64
#define XML_MAX_DEPTH	32

//附加标记最大长度
#define XML_MAX_TAGADDED_LEN 3

#define XML_HEAD_ADDED_LEN 2
#define XML_TAIL_ADDED_LEN 3

typedef struct XML_TAG
{
	unsigned long Begin;
	unsigned long End;
	unsigned int Index;
	char Tag[XML_MAX_TAG_LEN + XML_MAX_TAGADDED_LEN + XML_TAIL_ADDED_LEN + 1];
}XML_TAG;

UINT XMLTestThreadProc(LPVOID lParam) ;
// #define _ENCRTYP_LZW 1

class CDTXML
{
public:
	CDTXML();
	~CDTXML();


	//文件是否打开
	char IsOpen();
	//打开文件，UNICODE参数
	char OpenU(const WCHAR * file, unsigned long create_desp=NULL);
	//打开文件
	char Open(const char * file,  unsigned long create_desp=NULL);

	//add by fanze 
	char OpenF(const char * file ,unsigned long create_desp=NULL) ;
	BOOL AttachBuf(char *pBuf ,int nLen) ;
	VOID SetPackgeFlage(BOOL bPackge = FALSE ) ;


	//////////////////////////////////////////////////////////////////////////
	void SetTagMatchCase(BOOL matchcase);
	void SetTagSupportWildcard(BOOL supportwildcard);

	char Close();
	BOOL CheckFile(char * buffer128);
	char Save();

	unsigned int GetChildCount(const char * tag);

	char Enter(const char * tag, unsigned int index);
	char Enter(const char * tag){return Enter(tag,0);}	

	char Back(int step);
	char Back(){return Back(1);}

	char AddChild(const char * tag, unsigned int index);
	char AddChild(const char * tag){return AddChild(tag,0);}

	char * GetString(const char * tag, const char * defaultvalue, unsigned int index);
	char * GetString(const char * tag, const char * defaultvalue){return GetString(tag,defaultvalue,0);}
		
	char GetString(const char * tag, unsigned int bufsize, char * buf, unsigned long & outlen, unsigned int index);
	char GetString(const char * tag, unsigned int bufsize, char * buf, unsigned long & outlen){return GetString(tag,bufsize,buf,outlen,0);}

	//For Unicode
	PTCHAR GetStringCU(const char * tag, const char * tagadded, LPCWSTR defaultvalue, unsigned int index);
	PTCHAR GetStringCU(const char * tag, const char * tagadded, LPCWSTR defaultvalue){return GetStringCU(tag,tagadded,defaultvalue,0);}

	char GetStringCU(const char * tag, const char * tagadded, unsigned int bufsize, PWCHAR buf, unsigned int index);
	char GetStringCU(const char * tag, const char * tagadded, unsigned int bufsize, PWCHAR buf){return GetStringCU(tag,tagadded,bufsize,buf,0);}

	char * GetStringC(const char * tag, const char * tagadded, const char * defaultvalue, unsigned int index);
	char * GetStringC(const char * tag, const char * tagadded, const char * defaultvalue){return GetStringC(tag,tagadded,defaultvalue,0);};

	long GetInteger(const char * tag, long defaultvalue, unsigned int index);
	long GetInteger(const char * tag, long defaultvalue){return GetInteger(tag,defaultvalue,0);}

	char GetInteger(long * value, const char * tag, unsigned int index);
	char GetInteger(long * value, const char * tag){return GetInteger(value,tag,0);}

	double GetFloat(const char * tag, double defaultvalue, unsigned int index);
	double GetFloat(const char * tag, double defaultvalue){return GetFloat(tag,defaultvalue,0);}

	char GetFloat(const char * tag, double *f, unsigned int index);
	char GetFloat(const char * tag, double *f){return GetFloat(tag,f,0);}

	char SetString(const char * tag, const char * str, unsigned int index);
	char SetString(const char * tag, const char * str){return SetString(tag,str,0);}

	char SetStringC(const char * tag, const char * tagadded, PCHAR str, unsigned int index);
	char SetStringC(const char * tag, const char * tagadded, PCHAR str){return SetStringC(tag,tagadded,str,0);}

	//For Unicode
	char SetStringCU(const char * tag, const char * tagadded, PWCHAR str, unsigned int index);
	char SetStringCU(const char * tag, const char * tagadded, PWCHAR str){return SetStringCU(tag,tagadded,str,0);}


	char SetInteger(const char * tag, long l, unsigned int index);
	char SetInteger(const char * tag, long l){return SetInteger(tag,l,0);}

	char SetFloat(const char * tag, double f, unsigned int index);
	char SetFloat(const char * tag, double f){return SetFloat(tag,f,0);}

	char RemoveChild(const char * tag, unsigned int index);
	char RemoveChild(const char * tag){return RemoveChild(tag,0);}

	void SetEncrypt(char encrypt);
	char GetEncrypt();
	char SetKey(long key0, long key1, long key2);

	BOOL GetCurrentFilePath(CHAR * buffer, INT buffersize);
	BOOL GetCurrentFilePathU(WCHAR * buffer, INT buffersize);

	BOOL GetLastSearchedTag(CHAR * buffer, INT bufsize);

	WCHAR * GetCurFilePath()
	{
		return CurFilePath ;
	}


private:
	char SearchTag(const char * tag, int taglen, long begin, long end, long * b, long * e, unsigned int index);
	char SearchTagMatchCaseWithWildcard(const char * tag, int taglen, long begin, long end, long * b, long * e, unsigned int index);
	char SearchTagMatchCaseWithoutWildcard(const char * tag, int taglen, long begin, long end, long * b, long * e, unsigned int index);
	char SearchTagUnmatchCaseWithWildcard(const char * tag, int taglen, long begin, long end, long * b, long * e, unsigned int index);
	char SearchTagUnmatchCaseWithoutWildcard(const char * tag, int taglen, long begin, long end, long * b, long * e, unsigned int index);


	//long SearchStr(unsigned long b, unsigned long e, const char * str, unsigned int index);
	
	char * NewCopy(const char * str);
	PWCHAR NewCopyU(LPCWSTR str);
	
	
	//Check file utilities
	BOOL IsValidTagChar(char c);

	
	//For Encode & Decode
	unsigned long GenSerial(unsigned long pos);
	char DecryptToBuffer();
	char EncryptAndSaveToFile();

	CHAR * LastSearchedTag;	//指向Buffer中的“最后一次SearchTag匹配的标记名”
	INT	   LastSearchedTagLen;	//该标记的长度

	BOOL WildcardMatch(const char * wildcard, int wildcardn, const char * str, INT strn);
	BOOL WildcardMatchI(const char * wildcard, int wildcardn, const char * str, INT strn);

protected:
	ULONG Key1;
	ULONG Key0;
	ULONG Key2;
	BOOL IsEncrypted;

	XML_TAG * Tag;
	INT Depth;
	HANDLE FileHandle;

	WCHAR * CurFilePath;

	CHAR * ReturnBuffer;
	CHAR * Buffer;
	ULONG BufferSize;
	
	BOOL TagMatchCase;
	BOOL TagSupportWildcard;

	BOOL BPackgeFile ;
};


#endif