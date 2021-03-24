#pragma once

namespace Engine {

	class E_API EUUID
	{
	public:
		EUUID();
		// the bits are reversed
		//	for example:
		//	{09E95D97-364C-43d5-8ADF-FF4CE0EC41A7}
		//	'975DE909' 'd5434C36' '4CFFDF8A' 'A741ECE0'
		EUUID(const u8 data [16]);
		EUUID(u32  a, u32  b, u32  c, u32  d);
		// copy the bytes one by one
		EUUID(const EUUID& src);
		~EUUID();

		EUUID& operator=(const EUUID& src);
		bool		operator==(const EUUID& id) const;
		bool		operator!=(const EUUID& id) const;
		bool    	operator < (const EUUID& iid) const;
		size_t		operator()() const;

		EString			ToString() const;
		bool			FromString(const EString& id);


		// create four 32 Bit integer from the components of the GUID structure
		// the bits are reversed
		//	for example:
		//	{09E95D97-364C-43d5-8ADF-FF4CE0EC41A7}
		//	'975DE909' 'd5434C36' '4CFFDF8A' 'A741ECE0'
		void			GetUUID(u32& out1, u32& out2, u32& out3, u32& out4) const;
		const EUUID& 	CreateNew();
		bool 			IsValid() const;
		const u8* 		GetData() const;
		u8* 			GetData();
	protected:
		// {09E95D97-364C-43d5-8ADF-FF4CE0EC41A7}
		union
		{
			u8		fData[16];
			u32		f32[4];
			u64		f64[2];
		};
	};

	class UuidHash
	{
	public:
		UuidHash(){}
		size_t operator()(const EUUID& x) const
		{
			static const unsigned long MAX_PARAMETER = 50000;
			static const unsigned long THIS_SIZE = 16;

			unsigned long t = 0;
			size_t h = 0;

			const u8* key = x.GetData();

			// step 1: summation
			for(int i=0; i<THIS_SIZE; i++)
				t += key[i];

			// step 2: reduction
			while(t > MAX_PARAMETER)
				t = t % (t % (10 * ( t % 10)));

			h = t;
			return h;
		}
	};

}