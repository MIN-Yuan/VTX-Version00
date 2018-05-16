#include "stdafx.h"

#include <string.h>
#include <stdarg.h>


//��  �ܣ���lpszSour�в����ַ���lpszFind��lpszFind�п��԰���ͨ���ַ���?��
//��  ����nStartΪ��lpszSour�е���ʼ����λ��
//����ֵ���ɹ�����ƥ��λ�ã����򷵻�-1
//ע  �⣺Called by ��bool MatchingString()��
int FindingString(const char* lpszSour, const char* lpszFind, int nStart /* = 0 */)
{
//	ASSERT(lpszSour && lpszFind && nStart >= 0);
	if(lpszSour == NULL || lpszFind == NULL || nStart < 0)
		return -1;

	int m = strlen(lpszSour);
	int n = strlen(lpszFind);

	if( nStart+n > m )
		return -1;

	if(n == 0)
		return nStart;

//KMP�㷨
	int* next = new int[n];
	//�õ������ַ�����next����
	{	n--;

		int j, k;
		j = 0;
		k = -1;
		next[0] = -1;

		while(j < n)
		{	
			if(k == -1 || lpszFind[k] == '?' || lpszFind[j] == lpszFind[k])
			{	j++;
				k++;
				next[j] = k;
			}
			else
				k = next[k];
		}

		n++;
	}

	int i = nStart, j = 0;
	while(i < m && j < n)
	{
		if(j == -1 || lpszFind[j] == '?' || lpszSour[i] == lpszFind[j])
		{	i++;
			j++;
		}
		else
			j = next[j];
	}

	delete []next;

	if(j >= n)
		return i-n;
	else
		return -1;
}

//��	  �ܣ���ͨ������ַ���ƥ��
//��	  ����lpszSour��һ����ͨ�ַ�����
//			  lpszMatch��һ���԰���ͨ������ַ�����
//			  bMatchCaseΪ0�������ִ�Сд���������ִ�Сд��
//��  ��  ֵ��ƥ�䣬����1�����򷵻�0��
//ͨ������壺
//		��*��	���������ַ������������ַ�����
//		��?��	��������һ���ַ�������Ϊ�գ�
//ʱ	  �䣺	2001.11.02	13:00
bool MatchingString(const char* lpszSour, const char* lpszMatch, bool bMatchCase /*  = true */)
{
//	ASSERT(AfxIsValidString(lpszSour) && AfxIsValidString(lpszMatch));
	if(lpszSour == NULL || lpszMatch == NULL)
		return false;

	if(lpszMatch[0] == 0)//Is a empty string
	{
		if(lpszSour[0] == 0)
			return true;
		else
			return false;
	}

	int i = 0, j = 0;

	//���ɱȽ�����ʱԴ�ַ���'szSource'
	char* szSource =
		new char[ (j = strlen(lpszSour)+1) ];

	if( bMatchCase )
	{	//memcpy(szSource, lpszSour, j);
		while( *(szSource+i) = *(lpszSour+i++) );
	}
	else
	{	//Lowercase 'lpszSour' to 'szSource'
		i = 0;
		while(lpszSour[i])
		{	if(lpszSour[i] >= 'A' && lpszSour[i] <= 'Z')
				szSource[i] = lpszSour[i] - 'A' + 'a';
			else
				szSource[i] = lpszSour[i];

			i++;
		}
		szSource[i] = 0;
	}

	//���ɱȽ�����ʱƥ���ַ���'szMatcher'
	char* szMatcher = new char[strlen(lpszMatch)+1];

	//��lpszMatch���������ġ�*������һ����*�����Ƶ�szMatcher��
	i = j = 0;
	while(lpszMatch[i])
	{
		szMatcher[j++] = (!bMatchCase) ?
								( (lpszMatch[i] >= 'A' && lpszMatch[i] <= 'Z') ?//Lowercase lpszMatch[i] to szMatcher[j]
										lpszMatch[i] - 'A' + 'a' :
										lpszMatch[i]
								) :
								lpszMatch[i];		 //Copy lpszMatch[i] to szMatcher[j]
		//Merge '*'
		if(lpszMatch[i] == '*')
			while(lpszMatch[++i] == '*');
		else
			i++;
	}
	szMatcher[j] = 0;

	//��ʼ����ƥ����

	int nMatchOffset, nSourOffset;

	bool bIsMatched = true;
	nMatchOffset = nSourOffset = 0;
	while(szMatcher[nMatchOffset])
	{
		if(szMatcher[nMatchOffset] == '*')
		{
			if(szMatcher[nMatchOffset+1] == 0)
			{	//szMatcher[nMatchOffset]�����һ���ַ�

				bIsMatched = true;
				break;
			}
			else
			{	//szMatcher[nMatchOffset+1]ֻ����'?'����ͨ�ַ�

				int nSubOffset = nMatchOffset+1;

				while(szMatcher[nSubOffset])
				{	if(szMatcher[nSubOffset] == '*')
						break;
					nSubOffset++;
				}

				if( strlen(szSource+nSourOffset) <
						size_t(nSubOffset-nMatchOffset-1) )
				{	//Դ�ַ���ʣ�µĳ���С��ƥ�䴮ʣ��Ҫ�󳤶�
					bIsMatched = false; //�ж���ƥ��
					break;			//�˳�
				}

				if(!szMatcher[nSubOffset])//nSubOffset is point to ender of 'szMatcher'
				{	//���ʣ�²����ַ��Ƿ�һһƥ��

					nSubOffset--;
					int nTempSourOffset = strlen(szSource)-1;
					//�Ӻ���ǰ����ƥ��
					while(szMatcher[nSubOffset] != '*')
					{
						if(szMatcher[nSubOffset] == '?')
							;
						else
						{	if(szMatcher[nSubOffset] != szSource[nTempSourOffset])
							{	bIsMatched = false;
								break;
							}
						}
						nSubOffset--;
						nTempSourOffset--;
					}
					break;
				}
				else//szMatcher[nSubOffset] == '*'
				{	nSubOffset -= nMatchOffset;

					char* szTempFinder = new char[nSubOffset];
					nSubOffset--;
					memcpy(szTempFinder, szMatcher+nMatchOffset+1, nSubOffset);
					szTempFinder[nSubOffset] = 0;

					int nPos = ::FindingString(szSource+nSourOffset, szTempFinder, 0);
					delete []szTempFinder;

					if(nPos != -1)//��'szSource+nSourOffset'���ҵ�szTempFinder
					{	nMatchOffset += nSubOffset;
						nSourOffset += (nPos+nSubOffset-1);
					}
					else
					{	bIsMatched = false;
						break;
					}
				}
			}
		}		//end of "if(szMatcher[nMatchOffset] == '*')"
		else if(szMatcher[nMatchOffset] == '?')
		{
			if(!szSource[nSourOffset])
			{	bIsMatched = false;
				break;
			}
			if(!szMatcher[nMatchOffset+1] && szSource[nSourOffset+1])
			{	//���szMatcher[nMatchOffset]�����һ���ַ���
				//��szSource[nSourOffset]�������һ���ַ�
				bIsMatched = false;
				break;
			}
			nMatchOffset++;
			nSourOffset++;
		}
		else//szMatcher[nMatchOffset]Ϊ�����ַ�
		{
			if(szSource[nSourOffset] != szMatcher[nMatchOffset])
			{	bIsMatched = false;
				break;
			}
			if(!szMatcher[nMatchOffset+1] && szSource[nSourOffset+1])
			{	bIsMatched = false;
				break;
			}
			nMatchOffset++;
			nSourOffset++;
		}
	}

	delete []szSource;
	delete []szMatcher;
	return bIsMatched;
}

//��  �ܣ�����ƥ�䣬��ͬƥ���ַ���֮���á�,������
//			�磺��*.h,*.cpp��������ƥ�䡰*.h���͡�*.cpp��
//��  ����nMatchLogic = 0, ��ͬƥ�����else���룻bMatchCase, �Ƿ��С����
//����ֵ�����bRetReversed = 0, ƥ�䷵��true������ƥ�䷵��true
//ʱ  �䣺2001.11.02  17:00
bool MultiMatching(const char* lpszSour, const char* lpszMatch, int nMatchLogic /* = 0 */, bool bRetReversed /* = 0 */, bool bMatchCase /* = true */)
{
//	ASSERT(AfxIsValidString(lpszSour) && AfxIsValidString(lpszMatch));
	if(lpszSour == NULL || lpszMatch == NULL)
		return false;

	char* szSubMatch = new char[strlen(lpszMatch)+1];
	bool bIsMatch;

	if(nMatchLogic == 0)//���
	{	bIsMatch = 0;
		int i = 0;
		int j = 0;
		while(1)
		{	if(lpszMatch[i] != 0 && lpszMatch[i] != ',')
				szSubMatch[j++] = lpszMatch[i];
			else
			{	szSubMatch[j] = 0;
				if(j != 0)
				{
					bIsMatch = MatchingString(lpszSour, szSubMatch, bMatchCase);
					if(bIsMatch)
						break;
				}
				j = 0;
			}

			if(lpszMatch[i] == 0)
				break;
			i++;
		}
	}
	else//����
	{	bIsMatch = 1;
		int i = 0;
		int j = 0;
		while(1)
		{	if(lpszMatch[i] != 0 && lpszMatch[i] != ',')
				szSubMatch[j++] = lpszMatch[i];
			else
			{	szSubMatch[j] = 0;

				bIsMatch = MatchingString(lpszSour, szSubMatch, bMatchCase);
				if(!bIsMatch)
					break;

				j = 0;
			}

			if(lpszMatch[i] == 0)
				break;
			i++;
		}
	}

	delete []szSubMatch;

	if(bRetReversed)
		return !bIsMatch;
	else
		return bIsMatch;
}

//-------------ʹ������----------------//

/*
#include <stdio.h>

void main()
{
	char* str = "String match";
	bool bMatch;
	char* sMatch;

	sMatch = "*ing??*c*";
	bMatch = MatchingString(str, sMatch);//��ƥ��
	printf("\"%s\" %s match with \"%s\"\n\n", str, bMatch ? "is" : "is not", sMatch);

	sMatch = "*ing??*c*?";
	bMatch = MatchingString(str, sMatch);//��ƥ��
	printf("\"%s\" %s match with \"%s\"\n\n", str, bMatch ? "is" : "is not", sMatch);

	sMatch = "*.cpp,*tch*";
	bMatch = MultiMatching("stsrtcht", sMatch);//�����߼�ƥ��
	printf("\"%s\" %s match with \"%s\"\n\n", str, bMatch ? "is" : "is not", sMatch);

	getchar();
}
*/

char HexString2Buffer(const char * hexstring, unsigned char * outbuf, unsigned int bufsize, unsigned int * outcount)
{
	unsigned int i;
	unsigned int len;
	char buf[3];
	unsigned long ul;
	char * endpos;
	unsigned int pos;

	len=strlen(hexstring);

	if(len>bufsize*2){
		return 0;
	}

	pos=0;
	for(i=0; i<len; i+=2){
		buf[0]=hexstring[i];
		buf[1]=hexstring[i+1];
		buf[2]=0;
		ul=strtoul(buf,&endpos,16);
		outbuf[pos++]=(unsigned char)ul;
	}

	outcount[0]=pos;

	return 1;
}


char Buffer2HexString(const unsigned char * bytebuf, unsigned int count, char * outstring, unsigned int bufsize)  
{
	unsigned int i;
	char buf[16];

	if(count*2>bufsize){
		return 0;
	}

	outstring[0]=0;
	for(i=0; i<count; i++){
		ltoa(bytebuf[i],buf,16);
		if(strlen(buf)<2){
			buf[1]=buf[0];
			buf[0]='0';
			buf[2]=0;
		}
		strcat(outstring,buf);
	}

	return 1;
}


char Buffer2HexString(const unsigned char * bytebuf, unsigned int count, char * outstring, unsigned int bufsize, char * head, char * tail)  
{
	unsigned int i;
	char buf[16];

	if(count*2>bufsize){
		return 0;
	}

	outstring[0]=0;
	for(i=0; i<count; i++){
		ltoa(bytebuf[i],buf,16);
		if(strlen(buf)<2){
			buf[1]=buf[0];
			buf[0]='0';
			buf[2]=0;
		}
		strcat(outstring,head);
		strcat(outstring,buf);
		strcat(outstring,tail);
	}

	return 1;
}


char MatchBufWithHexStr(const unsigned char * bytebuf, unsigned int count, const char * hexstring)
{
	char strbuf[255];

	if(Buffer2HexString(bytebuf,count,strbuf,255)==0){
		return 0;
	}

	return 	MatchingString(strbuf,hexstring,0);
}


char TrimString(char * str, char count, ...)
{
	int len;
	char * temp=0;
	int i;
	int j;
	int c;
	char uselesschar;
	va_list pa;

	char chars[32];
	
	if(count>32){
		return 0;
	}

	va_start(pa,count);
	for(c=0; c<count; c++){
		chars[c]=va_arg(pa,char);
	}
	va_end(pa);

	len=strlen(str);

	temp=new char[len+1];
	if(temp==0){
		return 0;
	}

	j=0;
	for(i=0; i<=len; i++){

		//judge weather is useless char.
		uselesschar=0;
		for(c=0; c<count; c++){
			if(str[i]==chars[c]){
				uselesschar=1;
				break;
			}
		}
		if(uselesschar){
			continue;
		}

		temp[j++]=str[i];
	}

	strcpy(str, temp);
	delete [] temp;

	return 1;
}


char * NewCopy(const char * str)
{	char * p;
	p=new far char[strlen(str)+1];
	strcpy(p,str);
	return p;
}


void ReplaceString(char * str, char * org, char * replaceby)
{
	int slen;
	int olen;
	int pos;
	char strbuf[256];

	slen=strlen(str);
	olen=strlen(org);

	if(slen==0){
		return;
	}

	if(olen==0){
		return;
	}
	
	while((pos=FindingString(str,org,0))!=-1){
		strcpy(strbuf,str+pos+olen);
		str[pos]=0;
		strcat(str,replaceby);
		strcat(str,strbuf);
	}

}

void PC_DelayXms(int num)
{
	LARGE_INTEGER  litmp;
	LONGLONG       QPart1,QPart2;
	double         dfMinus, dfFreq, dfTim;

	QueryPerformanceFrequency(&litmp);
	dfFreq = (double)litmp.QuadPart; 		// ��ü�������ʱ��Ƶ��
	QueryPerformanceCounter(&litmp);
	QPart1 = litmp.QuadPart; 				// ��ó�ʼֵ
	do{
		QueryPerformanceCounter(&litmp);
		QPart2 = litmp.QuadPart;			// �����ֵֹ
		dfMinus = (double)(QPart2-QPart1);
		dfTim = dfMinus / dfFreq; 			// ��ö�Ӧ��ʱ��ֵ����λΪ��
    }while(dfTim<(0.001*num));
}