// LZW.cpp: implementation of the LZW class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "malloc.h"
#include "BinaryTree.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//////////////////////////////////////////////////////////////////////
class CLZWBuffer
{
public:
	CLZWBuffer()	{}
	CLZWBuffer(BYTE* pBuffer, int nLength) { m_pBuffer = pBuffer, m_nLength = nLength;	}
	CLZWBuffer(const CLZWBuffer& buffer) { *this = buffer;	}

public:
	BYTE* m_pBuffer;
	int m_nLength;
	inline int compare(const CLZWBuffer* buffer)
	{
		int nResult = memcmp(m_pBuffer, buffer->m_pBuffer, min(m_nLength, buffer->m_nLength));
		if(nResult != 0 || m_nLength == buffer->m_nLength)
			return nResult;
		return m_nLength > buffer->m_nLength ? 1 : -1;
	}
	inline void operator=(const CLZWBuffer* buffer) { m_pBuffer = buffer->m_pBuffer;	m_nLength = buffer->m_nLength;	}
};

// 
// void *AllocPtr(int nSize)
// {
// 	void *p = malloc(nSize);
// 	return memset(p, 0, _msize(p));
// }
// 
// void *ReAllocPtr(void *p, int nSize)
// {
// 	int nOldSize = _msize(p);
// 	p = realloc(p, nSize);
// 	if(nSize > nOldSize)
// 	{
// 		nSize = _msize(p);
// 		memset((char*)p+nOldSize, 0, nSize-nOldSize);
// 	}
// 	return p;
// }
// 
// void FreePtr(void *p)
// {
// 	free(p);
// }

bool CompressLZW(BYTE *pSrc, int nSrcLen, BYTE *&pDes, int &nDesLen, int nBitsPerSample)
{
	nDesLen = (sizeof(DWORD)+1)<<3;

	// allocate buffer for destination buffer
	int nAllocLength = nSrcLen*2;

	//Dengting
	//pDes = (BYTE*)AllocPtr(nAllocLength);
	pDes = NULL;
	pDes = new BYTE[nAllocLength+255];
	if(pDes==NULL){
		return FALSE;
	}
	memset(pDes,0,nAllocLength+255);

	
	// save source buffer length at the first DWORD
	*(DWORD*)pDes = nSrcLen;
	*(pDes+sizeof(DWORD)) = nBitsPerSample;
	int nSample = *pSrc;
	int nMaxSamples = 1 << nBitsPerSample;
	// dictionary hash table
	CBinaryTree<CLZWBuffer, CLZWBuffer*, int, int> dictionary;
	dictionary.NoRepeat = true;
	// keep first 256 IDs for ascii Samples
	dictionary.Serial = 256;
	// tree node to keep last success search to start with
	CBinaryTreeNode<CLZWBuffer, int>* pNode = dictionary.Root;
	// left dictionary Samples points to the source buffer
	CLZWBuffer node(pSrc, 2);
	// scan the input buffer
	while(nSrcLen-- > 0)
	{
		if(dictionary.Serial == nMaxSamples)
		{
			dictionary.RemoveAll();
			dictionary.Serial = 256;
		}
		pNode = dictionary.Insert(&node, -1, pNode);
		if(pNode->Count > 1)
			// (repeated Sample), save success Sample to be used next fail
			nSample = pNode->ID, node.m_nLength++;
		else
		{	
			
			//Dengting:
			// write last success Sample
			//if((nDesLen>>3)+(int)sizeof(DWORD) >= nAllocLength){
				//pDes = (BYTE*)ReAllocPtr(pDes, nAllocLength += 100);
			//}

			if((nDesLen>>3)+(int)sizeof(DWORD) >= nAllocLength){
				delete [] pDes;
				pDes=NULL;
				return FALSE;
			}
			
			
			*(DWORD*)(pDes+(nDesLen>>3)) |= nSample << (nDesLen&7);
			nDesLen += nBitsPerSample;
			// initialize node to next Sample
			node.m_pBuffer += node.m_nLength-1;
			node.m_nLength = 2;
			// copy first byte of the node as a new Sample
			if(nSrcLen > 0)
				nSample = *node.m_pBuffer;
			// initialize search root
			pNode = dictionary.Root;
		}
	}
	nDesLen = (nDesLen+7)/8;

	//Dengting
	//pDes = (BYTE*)ReAllocPtr(pDes, nDesLen);

	return true;
}

void ClearDictionary(vector<CLZWBuffer *>& dictionary)
{
	for(vector<CLZWBuffer*>::iterator i = dictionary.begin(); i != dictionary.end(); i++)
		delete (*i);
	dictionary.clear();
}

/////////////////////// COMPRESSED DATA FORMAT ////////////////////////////
// 4B Data original size
// 1B Bits per sample
// 

bool DecompressLZW(BYTE *pSrc, int nSrcLen, BYTE *&pDes, int &nDesLen)
{
	// first two DWORDS (final buffer length, Samples sizes bitmap buffer start)
	// copy destination final length (4B)
	nDesLen = *(DWORD*)pSrc; //此条语句在wince下导致内存错误，程序自动退出


	// copy bits pre Sample (1B)
	int nBitsPerSample = *(pSrc+sizeof(DWORD));

	// allocate buffer for decompressed buffer
	//Dengting:
	//pDes = (BYTE*)malloc(nDesLen+1);

	pDes=NULL;
	pDes=new BYTE[nDesLen+256];//modify by fanze:申请内存加大256字节，避免出现内存错误：DAMAGE:after Normal
	if(pDes==NULL){
		return FALSE;
	}
	memset(pDes,0,nDesLen+256);


	// copy first char from source to destination (1B)
	*pDes = *(pSrc+sizeof(DWORD)+1);

	int nMaxSamples = 1 << nBitsPerSample;

	// Dengting: 调，喜欢卖弄C操作符，Points to first sample
	int nSample, nSrcIndex = ((sizeof(DWORD)+1)<<3) + nBitsPerSample;

	// dictionary array
	vector<CLZWBuffer *> dictionary;

	// let dictionary Samples points to the destination buffer
	CLZWBuffer node(pDes, 2);
	CLZWBuffer *pNodeSample;
	
	int nDesIndex = 1;
	int nDesIndexSave = 0;
	int nSampleLen;

	while(nDesIndex < nDesLen)
	{
		//Dengting: 调, Get a xBit sample to nSample
		nSample = (*(DWORD*)(pSrc+(nSrcIndex>>3)))>>(nSrcIndex&7) & (nMaxSamples-1);

		nSrcIndex += nBitsPerSample;

		if(dictionary.size() == nMaxSamples-256)
			ClearDictionary(dictionary);

		if(nSample >= 256){
			if(nSample-256 < (int)dictionary.size()){

				// normal case, valid dictionary Sample
				nDesIndexSave = nDesIndex;
				pNodeSample = dictionary.at(nSample-256);
				nSampleLen = pNodeSample->m_nLength+1;

				// copy dictionary node buffer to decompressed buffer
				memcpy(pDes+nDesIndex, pNodeSample->m_pBuffer, pNodeSample->m_nLength);
				nDesIndex += pNodeSample->m_nLength;

			}else{
				// out of range Sample
				nSampleLen = nDesIndex-nDesIndexSave+2;

				// copy previous decompressed Sample as a new one + ...
				memcpy(pDes+nDesIndex, pDes+nDesIndexSave, nDesIndex-nDesIndexSave);
				nDesIndex += nDesIndex-nDesIndexSave;

				// add first char of the previous decompressed Sample
				*(pDes+nDesIndex++) = *(pDes+nDesIndexSave);
				nDesIndexSave += nSampleLen-2;
			}
		}else{
			nDesIndexSave = nDesIndex;
			*(pDes+nDesIndex) = (BYTE)nSample;
			nDesIndex++;
			nSampleLen = 2;
		}

		// add current segment to the dictionary
		dictionary.push_back(new CLZWBuffer(node));

		// increment next node pointer to the last char of the added Sample
		node.m_pBuffer += node.m_nLength-1;
		node.m_nLength = nSampleLen;
	}

	// free dictionary Samples
	ClearDictionary(dictionary);

	return true; 
}
