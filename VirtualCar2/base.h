#ifndef STRING_MATCH_H
#define STRING_MATCH_H

//²éÕÒ×Ö·û´®
int  FindingString(const char* lpszSour, const char* lpszFind, int nStart = 0);
//´øÍ¨Åä·ûµÄ×Ö·û´®Æ¥Åä
bool MatchingString(const char* lpszSour, const char* lpszMatch, bool bMatchCase = true);
//¶àÖØÆ¥Åä
bool MultiMatching(const char* lpszSour, const char* lpszMatch, int nMatchLogic = 0, bool bRetReversed = 0, bool bMatchCase = true);


char HexString2Buffer(const char * hexstring, unsigned char * outbuf, unsigned int bufsize, unsigned int * outcount);
char Buffer2HexString(const unsigned char * bytebuf, unsigned int count, char * outstring, unsigned int bufsize);
char Buffer2HexString(const unsigned char * bytebuf, unsigned int count, char * outstring, unsigned int bufsize, char * head, char * tail);
char MatchBufWithHexStr(const unsigned char * bytebuf, unsigned int count, const char * hexstring);
char TrimString(char * str, char count, ...);

char * NewCopy(const char * str);

void ReplaceString(char * str, char * org, char * replaceby);
void PC_DelayXms(int num);

#endif
