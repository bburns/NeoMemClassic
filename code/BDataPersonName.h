
// BDataPersonName.h

#pragma once


#include "BData.h"



class BDataPersonName : public BData  
{
	DECLARE_SERIAL(BDataPersonName)

public:
	BDataPersonName();
	~BDataPersonName();

public:
	virtual BData* CreateCopy();
	virtual BOOL FindReferences(BObject* pobjFind);
	virtual ULONG GetMemoryUsed(BOOL bRecursive);
	virtual CString GetBDataText(BDoc* pDoc=NULL, ULONG lngPropertyID=0, BOOL bMachineVersion=FALSE);
	virtual void ResetData(); // use this to reset m_bCacheValid flag
	virtual void Serialize(CArchive& ar);
	virtual BOOL SetBDataText(const CString& str, BObject* pobjPropertyDef = 0, BOOL bShowErrorMessage = TRUE);


	virtual BOOL UIEditValue(CUI& ui, BObject* pobj = NULL, BObject* pobjPropertyDef = NULL);

public:
	// These are selected in global options combo
	enum eNameFormats {nfNone = 0, nfFirstLast = 1, nfLastFirst = 2, nfFirstLAST = 3, nfLASTFirst = 4};

private:
	CString m_strTitle;
	CString m_strFirst;
	CString m_strMiddle;
	CString m_strLast;
	CString m_strNickname;
	CString m_strSuffix;

	// Flag indicating the m_strText contains the full name and is valid
	BOOL m_bCacheValid;

	// These store the lengths of the strings to help speed 
//	int m_nTitle;
//	int m_nFirst;
//	int m_nMiddle;
//	int m_nLast;
//	int m_nNickname;
//	int m_nSuffix;

	// what if instead of all this, we just calculate the new name any time one of the components
	// changes, and store the result in m_strText?
	// just need a flag indicating it's current
	// then when name format changes, we walk through all objects, resetting the flag for any name
	// properties, and redisplay all views, which will call gettext and recalculate the name
	// this would be faster than any concatenating code we could come up with
	// though should use good concat code anyway to speed file loading.
};




