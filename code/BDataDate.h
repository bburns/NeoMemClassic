
// BDataDate
// This class stores date/time data.
//-----------------------------------------------------------------------------------------------------------------



#pragma once


#include "BData.h"


/*
// Unfortunately, can't do this because these classes have constructors.
// C++ will not let you put classes with constructors in a struct or union.
union uDateString
{
	COleDateTime dt;
	CString str;
};
*/


// Structure for m_lngFlags2 variable
//, why can't you make this private?
struct sDateFlags
{
	UINT Type : 4;
	UINT Relationship : 3;
	UINT Modifiers : 10;
	UINT Season : 15;
};


class BDataDate : public BData  
{
	DECLARE_SERIAL(BDataDate)

public:
	BDataDate();
	virtual ~BDataDate();

public:
	virtual BData* CreateCopy();
	virtual BOOL EditValue(BObject* pobj, BObject* pobjPropertyDef);
	virtual BOOL FindReferences(BObject* pobjFind);
	virtual ULONG GetMemoryUsed(BOOL bRecursive);
	virtual LPCTSTR GetBDataText(CNeoDoc* pDoc, ULONG lngPropertyID, BOOL bMachineVersion=FALSE);
	virtual void Serialize(CArchive &ar);
	virtual BOOL SetBDataText(const CString& str, BObject* pobjPropertyDef = 0, BOOL bShowErrorMessage = TRUE);

public:
	// COleDateTime objects can be used to represent dates between January 1, 100, and 
	// December 31, 9999. COleDateTime objects are floating point values, with an approximate 
	// resolution of 1 millisecond
	// The month-calendar control is based on the Gregorian calendar, which was introduced 
	// in 1753. It will not calculate dates that are consistent with the Julian calendar that 
	// was in use prior to 1753. 
	COleDateTime m_odt; // January 1, 100 to December 31, 9999

	// Bug: Assumed that ^ was the power operator... it's xor!!!

	sDateFlags m_lngFlags;

	// Only one type allowed. 4 bits = 16 values
	enum eDateTypes {flagYear = 1, flagMonthYear, flagSeasonYear, flagString, flagUnknown, flagBlank};

	// Only one relationship type allowed. 3 bits = 8 values
	enum eDateRelationships {flagRange = 1, flagPlusOrMinus, flagAnd, flagOr};

	// Modifiers may be combined. 10 bits = 10 values 
	// Note: Want to leave room for more modifiers in string table, so make usetime, plural and uncertain last.
	enum eDateModifiers {flagAfter = 1, flagBefore = 2, flagCirca = 4, 
										flagUseTime = 128, flagPlural = 256, flagUncertain = 512}; // (2^10 is 1024)

	// Only one season allowed. 15 bits = 32768 values
	// Note: Season enums must match order or strings in string table!!!
	// Had to switch spring break and spring because parsing "spring" was chopping off "spring" from
	// "spring break" - longer entries must come first!!
	enum eDateSeasons {flagSpringBreak = 1, flagSummer, flagAutumn, flagWinter, flagNewYearsEve,
										flagNewYearsDay, flagEaster, flagChristmas, flagThanksgiving,
										flagSpring, flagHalloween};
	
	//, Version 2.0 could use our own format like this...
	// Note: Float stores 3.4E�38 to 3.4E+38, 6 � 7 significant digits
//	float m_sngYear; // Offset from fictional year 0 A.D., truncate to integer portion
//	long m_lngSeconds; // Number of seconds into the year (0 to 31,536,000)

	// Note: CTime is too small to use - 1970 to 2036 or something
	// SYSTEMTIME structure provides the year since 0 AD�in a word-sized member. 
	// Windows does not support dates prior to 1601. See FILETIME for details. 

};


