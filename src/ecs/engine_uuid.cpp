#include "Engine.h"

#ifdef EWIN
#include <windows.h>
#include <rpcdce.h>
#elif  EMAC
#include <CoreFoundation/CoreFoundation.h>
#endif


namespace Engine {

	EUUID::EUUID()
	{
		f64[0] = 0;
		f64[1] = 0;
	}
	EUUID::EUUID(u32 a, u32 b, u32 c, u32 d)
	{
		f32[0] = a;
		f32[1] = b;
		f32[2] = c;
		f32[3] = d;
	}

	EUUID::EUUID(const EUUID& src)
	{
		for (int i = 0; i < 16; i++)
			fData[i] = src.fData[i];
	}
	
	EUUID::EUUID(const u8 data [16]) 
	{
		memcpy(fData, data, sizeof(u8) * 16);
	}

	EUUID::~EUUID()
	{

	}

	EUUID& EUUID::operator=(const EUUID& src)
	{
		f64[0] = src.f64[0];
		f64[1] = src.f64[1];
		return *this;
	}

	bool EUUID::operator==(const EUUID& id) const
	{
		return f64[0] == id.f64[0] && f64[1] == id.f64[1];
	}

	bool EUUID::operator!=(const EUUID& id) const
	{
		return !operator==(id);
	}


	const EUUID& EUUID::CreateNew()
	{
#ifdef EWIN
		GUID	guid;
		UuidCreateSequential(&guid);

#pragma warning (disable: 4333)
		fData[0] = (u8)(guid.Data1 & 0xff000000) >> 24;
		fData[1] = (u8)(guid.Data1 & 0x00ff0000) >> 16;
		fData[2] = (u8)(guid.Data1 & 0x0000ff00) >> 8;
		fData[3] = (u8)(guid.Data1 & 0x000000ff);

		fData[4] = (guid.Data2 & 0xff00) >> 8;
		fData[5] = (guid.Data2 & 0x00ff);


		fData[6] = (guid.Data3 & 0xff00) >> 8;
		fData[7] = (guid.Data3 & 0x00ff);


		fData[8] = guid.Data4[0];
		fData[9] = guid.Data4[1];
		fData[10] = guid.Data4[2];
		fData[11] = guid.Data4[3];
		fData[12] = guid.Data4[4];
		fData[13] = guid.Data4[5];
		fData[14] = guid.Data4[6];
		fData[14] = guid.Data4[7];
#else
	auto newId = CFUUIDCreate(kCFAllocatorDefault);
	auto bytes = CFUUIDGetUUIDBytes(newId);
	CFRelease(newId);

	fData[0]  =	bytes.byte0;
	fData[1]  =	bytes.byte1;
	fData[2]  =	bytes.byte2;
	fData[3]  =	bytes.byte3;
	fData[4]  =	bytes.byte4;
	fData[5]  =	bytes.byte5;
	fData[6]  =	bytes.byte6;
	fData[7]  =	bytes.byte7;
	fData[8]  =	bytes.byte8;
	fData[9]  =	bytes.byte9;
	fData[10] =	bytes.byte10;
	fData[11] =	bytes.byte11;
	fData[12] =	bytes.byte12;
	fData[13] =	bytes.byte13;
	fData[14] =	bytes.byte14;
	fData[14] =	bytes.byte15;

	#endif

		return *this;
	}

	void EUUID::GetUUID(u32& out1, u32& out2, u32& out3, u32& out4) const
	{
		out1 = f32[0];
		out2 = f32[1];
		out3 = f32[2];
		out4 = f32[3];
	}

	bool EUUID::operator < (const EUUID& iid) const
	{
		bool	isLess	= true;
		bool	isEqual	= true;

		if ( isEqual ) 
		{
			isEqual		= isEqual && f64[0] ==  iid.f64[0];
			if ( ! isEqual ) { isLess	= (f64[0] < iid.f64[0]); }
	
			if( isEqual )
			{
				isEqual		= isEqual && f64[1] == iid.f64[1];
				if ( ! isEqual ) { isLess	= (f64[1] < iid.f64[1]); }
			}
		}

		return isLess && ! isEqual;
	}

	static char GetPieceAsChar(u8 piece)
	{
		//IR_ASSERT(piece <= 0x0F, "Failed to get piece of EUUID");
		char	ch;
		if (piece <= 9)
			ch = '0' + piece;
		else
			ch = 'A' + (piece - 0x0A);

		return ch;
	}

	static EString GetPieceAsText(u8 piece)
	{
		char	buffer[3];
		buffer[0] = GetPieceAsChar(piece >> 4);
		buffer[1] = GetPieceAsChar(piece & 0x0F);
		buffer[2] = 0x00;

		EString str = buffer;
		return str;
	}

	EString EUUID::ToString() const
	{
		EString str;
		str += "{";
		str += GetPieceAsText(fData[0]);
		str += GetPieceAsText(fData[1]);
		str += GetPieceAsText(fData[2]);
		str += GetPieceAsText(fData[3]);
		str += "-";
		str += GetPieceAsText(fData[4]);
		str += GetPieceAsText(fData[5]);
		str += "-";
		str += GetPieceAsText(fData[6]);
		str += GetPieceAsText(fData[7]);
		str += "-";
		str += GetPieceAsText(fData[8]);
		str += GetPieceAsText(fData[9]);
		str += "-";
		str += GetPieceAsText(fData[10]);
		str += GetPieceAsText(fData[11]);
		str += GetPieceAsText(fData[12]);
		str += GetPieceAsText(fData[13]);
		str += GetPieceAsText(fData[14]);
		str += GetPieceAsText(fData[15]);
		str += "}";
		return str;
	}

	static u8 GetDigitForChar(char ch)
	{
		if (ch >= '0' && ch <= '9') { return ch - '0'; }
		if (ch >= 'A' && ch <= 'F') { return 0x0A + (ch - 'A'); }
		if (ch >= 'a' && ch <= 'f') { return 0x0A + (ch - 'a'); }

		return 0;
	}

	static u8 GetDigitForPiece(const char* pIRString)
	{
		u8 a = GetDigitForChar(*(pIRString + 0));
		u8 b = GetDigitForChar(*(pIRString + 1));

		u8 res = a << 4 | (b & 0x0F);
		return res;
	}

	bool EUUID::FromString(const EString& str)
	{
		// {09E95D97-364C-43d5-8ADF-FF4CE0EC41A7}
		bool b = str.length() == 38;
		if (b)
		{
			bool	b1 = str[0] == '{';
			bool	b2 = str[9] == '-';
			bool	b3 = str[14] == '-';
			bool	b4 = str[19] == '-';
			bool	b5 = str[24] == '-';
			bool	b6 = str[37] == '}';

			b = b1 && b2 && b3 && b4 && b5 && b6;
		}

		if (!b)
		{
			// create new EUUID if this is not a valid EUUID
			EUUID newEUUID;
			*this = newEUUID;
		}

		if (b)
		{
			const char* pIRString = str.c_str();

			fData[0] = GetDigitForPiece(pIRString + 1);
			fData[1] = GetDigitForPiece(pIRString + 3);
			fData[2] = GetDigitForPiece(pIRString + 5);
			fData[3] = GetDigitForPiece(pIRString + 7);
			fData[4] = GetDigitForPiece(pIRString + 10);
			fData[5] = GetDigitForPiece(pIRString + 12);
			fData[6] = GetDigitForPiece(pIRString + 15);
			fData[7] = GetDigitForPiece(pIRString + 17);
			fData[8] = GetDigitForPiece(pIRString + 20);
			fData[9] = GetDigitForPiece(pIRString + 22);
			fData[10] = GetDigitForPiece(pIRString + 25);
			fData[11] = GetDigitForPiece(pIRString + 27);
			fData[12] = GetDigitForPiece(pIRString + 29);
			fData[13] = GetDigitForPiece(pIRString + 31);
			fData[14] = GetDigitForPiece(pIRString + 33);
			fData[15] = GetDigitForPiece(pIRString + 35);
		}

		return b;
	}

	bool EUUID::IsValid() const
	{
		bool valid = false;
		for (int i = 0; i < 16; i++)
		{
			if (fData[i] != 0)
			{
				valid = true;
			}
		}
		return valid;
	}

	const u8* EUUID::GetData() const
	{
		return &*fData;
	}

	u8* EUUID::GetData()
	{
		return &*fData;
	}

}