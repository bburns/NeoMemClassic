
// BDataNumber
// This class stores floating point data.
//-----------------------------------------------------------------------------------------------------------------



#pragma once


#include "BData.h"
#include "BObject.h"


class BDataNumber : public BData  
{
	DECLARE_SERIAL(BDataNumber)

public:
	BDataNumber();
	virtual ~BDataNumber();

public:
	virtual BData* CreateCopy();
	virtual BOOL EditValue(BObject* pobj, BObject* pobjPropertyDef);
	virtual BOOL FindReferences(BObject* pobjFind);
	virtual ULONG GetMemoryUsed(BOOL bRecursive);
	virtual LPCTSTR GetBDataText(CNeoDoc* pDoc, ULONG lngPropertyID, BOOL bMachineVersion=FALSE);
	virtual BOOL ReplaceReferences(BObject* pobjFind, BObject* pobjNew = 0);
	void Serialize(CArchive &ar);
	virtual BOOL SetBDataText(const CString& str, BObject* pobjPropertyDef = 0, BOOL bShowErrorMessage = TRUE);

public:
	double m_dblValue; // numerical value
	ULONG m_lngUnitID; // units associated with the value 

	//, store date here also?? then wouldn't need bdatadatedmeasurement
	//, store a display format here?
	// global settings will determine display format, like " or in. or inches,
	// or ' or ft or feet
	// or  m or meters
	// what about unit conversions? not there yet
};


