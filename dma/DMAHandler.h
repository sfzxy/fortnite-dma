#include <string>
#include <Windows.h>
#include <vmmdll.h>
#include <array>
#include <unordered_map>
#include <deque>

// set to FALSE if you dont want to track the total read size of the DMA
#define COUNT_TOTAL_READSIZE FALSE
class DMAHandler
{

	// Static variables, shared over all instances

	struct LibModules
	{
		HMODULE VMM = nullptr;
		HMODULE FTD3XX = nullptr;
		HMODULE LEECHCORE = nullptr;
	};

	static inline LibModules modules{};

	static inline VMM_HANDLE DMA_HANDLE = nullptr;

	// Counts the size of the reads in total. Reset every frame preferrably for memory tracking
	static inline DWORD64 readSize = 0;

	// Nonstatic variables, different for each class object on purpose, in case the user tries to access
	// multiple processes
	struct BaseProcessInfo
	{
		DWORD pid = 0;
		std::string name;
		const wchar_t* wname;
		ULONG64 base = 0;
	};

	BaseProcessInfo processInfo{};

	BOOLEAN PROCESS_INITIALIZED = FALSE;


	// Private log function used by the DMAHandler class
	static void log(const char* fmt, ...);

	// Will always throw a runtime error if PROCESS_INITIALIZED or DMA_INITIALIZED is false
	void assertNoInit() const;

	// Wow we have friends
	template<typename> friend class DMAScatter;

	static void retrieveScatter(VMMDLL_SCATTER_HANDLE handle, void* buffer, void* target, SIZE_T size);

	static bool DumpMemoryMap();

	std::string GameName;
	std::wstring WGameName;

	std::unordered_map<std::wstring, ULONG64> Modules;

	DWORD winlogonPid = 0;
	uintptr_t gafAsyncKeyStateBase = 0;

	std::array<uint8_t, 64> key_state;

	bool isKeyDown(std::array<uint8_t, 64>& keyState, uint8_t vkKey)
	{
		size_t index = vkKey * 2 / 8;
		size_t bitOffset = vkKey % 4 * 2;
		return (keyState[index] & (1 << bitOffset)) != 0;
	}

public:

	uint64_t gafAsyncKeyStateExport = 0;
	uint8_t state_bitmap[64]{ };
	uint8_t previous_state_bitmap[256 / 8]{ };
	uint64_t win32kbase = 0;

	DWORD win_logon_pid = 0;

	enum class e_registry_type
	{
		none = REG_NONE,
		sz = REG_SZ,
		expand_sz = REG_EXPAND_SZ,
		binary = REG_BINARY,
		dword = REG_DWORD,
		dword_little_endian = REG_DWORD_LITTLE_ENDIAN,
		dword_big_endian = REG_DWORD_BIG_ENDIAN,
		link = REG_LINK,
		multi_sz = REG_MULTI_SZ,
		resource_list = REG_RESOURCE_LIST,
		full_resource_descriptor = REG_FULL_RESOURCE_DESCRIPTOR,
		resource_requirements_list = REG_RESOURCE_REQUIREMENTS_LIST,
		qword = REG_QWORD,
		qword_little_endian = REG_QWORD_LITTLE_ENDIAN
	};

	std::string QueryValue(const char* path, e_registry_type type)
	{

		BYTE buffer[0x128];
		DWORD _type = static_cast<DWORD>(type);
		DWORD size = sizeof(buffer);

		if (!VMMDLL_WinReg_QueryValueExU(DMA_HANDLE, const_cast<LPSTR>(path), &_type, buffer, &size))
		{
			return "";
		}

		std::wstring wstr = std::wstring(reinterpret_cast<wchar_t*>(buffer));
		return std::string(wstr.begin(), wstr.end());
	}

	std::vector<int> GetPidListFromName(std::string name) {
		PVMMDLL_PROCESS_INFORMATION process_info = NULL;
		DWORD total_processes = 0;
		std::vector<int> list = { };

		if (!VMMDLL_ProcessGetInformationAll(DMA_HANDLE, &process_info, &total_processes))
		{
			return list;
		}

		for (size_t i = 0; i < total_processes; i++)
		{
			auto process = process_info[i];
			if (strstr(process.szNameLong, name.c_str()))
				list.push_back(process.dwPID);
		}

		return list;
	}

	/**
	 * \brief Constructor takes a wide string of the process.
	 * Expects that all the libraries are in the root dir
	 * \param wname process name
	 * \param memMap whether the memory map should get dumped to file.
	 */
	DMAHandler(const wchar_t* wname, bool memMap = true);
	bool FixDTB();
	// Whether the DMA and Process are initialized
	bool IsInitialized() const;

	// Gets the PID of the process
	DWORD GetPID() const;

	// Gets the Base address of the process
	ULONG64 GetBaseAddress();

	inline static bool isGuarded(uintptr_t pointer) noexcept
	{
		static constexpr uintptr_t filter = 0xFFFFFFF000000000;
		uintptr_t result = pointer & filter;
		return result == 0x8000000000 || result == 0x10000000000;
	}

	uintptr_t guard_address = 0;

	inline uintptr_t validatePointer(uintptr_t address) {
		return isGuarded(address) ? guard_address + (address & 0xFFFFFF) : address;
	}

	inline bool isKernel(uintptr_t ptr) {
		return (ptr & 0xFFF0000000000000) == 0xFFF0000000000000;
	}

	uintptr_t Read2(uintptr_t address, bool cache = false) {
		uintptr_t buffer{};
		int pid = 4;

		if (!isKernel(address))
			pid = processInfo.pid;

		//std::cout << "read from pid: " << std::to_string(pid).c_str() << std::endl;

		auto flags = VMMDLL_FLAG_NOCACHE | VMMDLL_FLAG_NOPAGING | VMMDLL_FLAG_NOCACHEPUT | VMMDLL_FLAG_NOPAGING_IO;
		if (cache) {
			flags &= ~VMMDLL_FLAG_NOCACHE;
			flags &= ~VMMDLL_FLAG_NOCACHEPUT;
		}

		VMMDLL_MemReadEx(DMA_HANDLE, pid, (ULONG64)address, reinterpret_cast<PBYTE>(&buffer), sizeof(buffer), NULL, flags);

		return validatePointer(buffer);
	}

	bool Read3(ULONG64 address, void* buffer, SIZE_T size)
	{
		//assertNoInit();
		DWORD dwBytesRead = 0;

#if COUNT_TOTAL_READSIZE
		readSize += size;
#endif

		//VMMDLL_MemReadEx(DMA_HANDLE, processInfo.pid, address, reinterpret_cast<PBYTE>(buffer), size, &dwBytesRead, VMMDLL_FLAG_NOCACHE | VMMDLL_FLAG_NOPAGING | VMMDLL_FLAG_ZEROPAD_ON_FAIL | VMMDLL_FLAG_NOPAGING_IO);

		int pid = 4;

		if (!((address & 0xFFF0000000000000) == 0xFFF0000000000000))
			pid = processInfo.pid;

		//std::cout << "read from pid: " << std::to_string(pid).c_str() << std::endl;

		auto flags = VMMDLL_FLAG_NOCACHE | VMMDLL_FLAG_NOPAGING | VMMDLL_FLAG_NOCACHEPUT | VMMDLL_FLAG_NOPAGING_IO;

		VMMDLL_MemReadEx(DMA_HANDLE, pid, (ULONG64)address, reinterpret_cast<PBYTE>(buffer), size, NULL, flags);

		if (pid == 4) return true;
		else return false;
	}

	ULONG64 GetGuardedRegion()
	{
		PVMMDLL_MAP_POOL pPool;

		VMMDLL_Map_GetPool(DMA_HANDLE, &pPool, NULL);

		for (DWORD i = 0; i < pPool->cMap; ++i) {
			PVMMDLL_MAP_POOLENTRY pEntry = &pPool->pMap[i];

			if (pEntry->cb == 0x200000 && memcmp(pEntry->szTag, "ConT", 4) == 0) {

				//std::cout << "tag: " << pEntry->szTag << " type: " << std::dec << pEntry->tpPool << std::endl;

				ULONG64 result = pEntry->va;

				VMMDLL_MemFree(pPool);

				guard_address = result;
				return result;
			}
		}

		VMMDLL_MemFree(pPool);
		return NULL;

		//uintptr_t vgk = VMMDLL_ProcessGetModuleBaseU(DMA_HANDLE, 4, const_cast<LPSTR>("vgk.sys"));

		//std::cout << "vgk.sys: 0x" << std::hex << vgk << std::endl;

		//guard_address = Read2(vgk + 0x7FCE0);

		//return guard_address;
	}

	ULONG64 GetModuleAddress(std::wstring modulename);
	void Read(ULONG64 address, ULONG64 buffer, SIZE_T size) const;

	bool prepareGetKey(const char* winlogonStr, const char* gafStr)
	{
		VMMDLL_PidGetFromName(DMA_HANDLE, (LPSTR)winlogonStr, &winlogonPid);

		winlogonPid |= 0x80000000;

		gafAsyncKeyStateBase = VMMDLL_ProcessGetProcAddressU(DMA_HANDLE, winlogonPid, (LPSTR)"win32kbase.sys", (LPSTR)gafStr);

		return gafAsyncKeyStateBase;
	}

	bool getKey(uint8_t vkKey)
	{
		std::array<uint8_t, 64> data;

		if (VMMDLL_MemReadEx(DMA_HANDLE, winlogonPid, gafAsyncKeyStateBase, PBYTE(&data), sizeof(std::array<uint8_t, 64>), 0, VMMDLL_FLAG_NOCACHE)) {
			key_state = data;
		}

		return isKeyDown(key_state, vkKey);
	}

	template <typename T>
	T Read(void* address)
	{
		T buffer{};
		memset(&buffer, 0, sizeof(T));
		Read(reinterpret_cast<ULONG64>(address), reinterpret_cast<ULONG64>(&buffer), sizeof(T));

		return buffer;
	}
	void* ReadString(ULONG64 address, void* buf, uint64_t length)
	{

		Read(address, reinterpret_cast<ULONG64>(buf), length);

		return buf;
	}
	template <typename T>
	T Read(ULONG64 address)
	{
		return Read<T>(reinterpret_cast<void*>(address));
	}
	bool ReadBool(const ULONG64 address, const ULONG64 buffer, const SIZE_T size) const;
	bool Write(ULONG64 address, ULONG64 buffer, SIZE_T size) const;

	template <typename T>
	bool Write(ULONG64 address, T* buffer)
	{
		return Write(address, reinterpret_cast<ULONG64>(buffer), sizeof(T));
	}

	template <typename T>
	bool Write(void* address, T* buffer)
	{
		return Write(reinterpret_cast<ULONG64>(address), reinterpret_cast<ULONG64>(buffer), sizeof(T));
	}

	template <typename T>
	bool Write(ULONG64 address, T value)
	{
		return Write(address, reinterpret_cast<ULONG64>(&value), sizeof(T));
	}

	template <typename T>
	bool Write(void* address, T value)
	{
		return Write(reinterpret_cast<ULONG64>(address), reinterpret_cast<ULONG64>(&value), sizeof(T));
	}
	template <typename T>
	bool QueueScatterWriteEx(VMMDLL_SCATTER_HANDLE handle, uint64_t addr, T value) const
	{
		assertNoInit();

		bool ret = VMMDLL_Scatter_PrepareWrite(handle, addr, reinterpret_cast<PBYTE>(&value), sizeof(value));
		if (!ret)
		{
			log("failed to prepare scatter write at 0x%p\n", addr);
		}
		return ret;
	}

	//Handle Scatter
	void QueueScatterReadEx(VMMDLL_SCATTER_HANDLE handle, uint64_t addr, void* bffr, size_t size) const;
	void ExecuteScatterRead(VMMDLL_SCATTER_HANDLE handle) const;

	void QueueScatterWriteEx(VMMDLL_SCATTER_HANDLE handle, uint64_t addr, void* bffr, size_t size) const;
	void ExecuteScatterWrite(VMMDLL_SCATTER_HANDLE handle) const;

	VMMDLL_SCATTER_HANDLE CreateScatterHandle(int pid) const;
	void CloseScatterHandle(VMMDLL_SCATTER_HANDLE& handle) const;


	/**
	 * \brief pattern scans the text section and returns 0 if unsuccessful
	 * \param pattern the pattern
	 * \param mask the mask
	 * \param returnCSOffset in case your pattern leads to a xxx, cs:offset, it will return the address of the global variable instead
	 * \return the address
	 */
	ULONG64 PatternScan(const char* pattern, const std::string& mask, bool returnCSOffset = true);

	/**
	 * \brief closes the DMA and sets DMA_INITIALIZED to FALSE. Do not call on every object, only at the end of your program.
	 */
	static void CloseDMA();

	bool InitKeyboard()
	{
		std::string win = QueryValue("HKLM\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\CurrentBuild", e_registry_type::sz);
		int Winver = 0;
		if (!win.empty())
			Winver = std::stoi(win);
		else
			return false;

		VMMDLL_PidGetFromName(DMA_HANDLE, const_cast<char*>(std::string("winlogon.exe").c_str()), &this->win_logon_pid);

		if (Winver > 22000)
		{
			auto pids = GetPidListFromName("csrss.exe");


			for (size_t i = 0; i < pids.size(); i++)
			{
				auto pid = pids[i];
				uintptr_t tmp = VMMDLL_ProcessGetModuleBaseU(DMA_HANDLE, pid, const_cast<LPSTR>("win32ksgd.sys"));
				uintptr_t g_session_global_slots = tmp + 0x3110;
				DWORD size;
				uintptr_t addy1;
				VMMDLL_MemReadEx(DMA_HANDLE, pid, g_session_global_slots, reinterpret_cast<PBYTE>(&addy1), 8, &size, VMMDLL_FLAG_NOCACHE | VMMDLL_FLAG_NOPAGING | VMMDLL_FLAG_ZEROPAD_ON_FAIL | VMMDLL_FLAG_NOPAGING_IO);
				uintptr_t addy2;
				VMMDLL_MemReadEx(DMA_HANDLE, pid, addy1, reinterpret_cast<PBYTE>(&addy2), 8, &size, VMMDLL_FLAG_NOCACHE | VMMDLL_FLAG_NOPAGING | VMMDLL_FLAG_ZEROPAD_ON_FAIL | VMMDLL_FLAG_NOPAGING_IO);
				uintptr_t user_session_state;
				VMMDLL_MemReadEx(DMA_HANDLE, pid, addy2, reinterpret_cast<PBYTE>(&user_session_state), 8, &size, VMMDLL_FLAG_NOCACHE | VMMDLL_FLAG_NOPAGING | VMMDLL_FLAG_ZEROPAD_ON_FAIL | VMMDLL_FLAG_NOPAGING_IO);
				gafAsyncKeyStateExport = user_session_state + 0x3690;
				if (gafAsyncKeyStateExport > 0x7FFFFFFFFFFF)
					break;
			}
			if (gafAsyncKeyStateExport > 0x7FFFFFFFFFFF)
				return true;
			return false;
		}
		else
		{
			PVMMDLL_MAP_EAT eat_map = NULL;
			PVMMDLL_MAP_EATENTRY eat_map_entry;
			DWORD winlogpid;
			VMMDLL_PidGetFromName(DMA_HANDLE, const_cast<char*>(std::string("winlogon.exe").c_str()), &winlogpid);
			bool result = VMMDLL_Map_GetEATU(DMA_HANDLE, winlogpid | VMMDLL_PID_PROCESS_WITH_KERNELMEMORY, const_cast<LPSTR>("win32kbase.sys"), &eat_map);
			if (!result)
				return false;

			if (eat_map->dwVersion != VMMDLL_MAP_EAT_VERSION)
			{
				VMMDLL_MemFree(eat_map);
				eat_map_entry = NULL;
				return false;
			}

			for (int i = 0; i < eat_map->cMap; i++)
			{
				eat_map_entry = eat_map->pMap + i;
				if (strcmp(eat_map_entry->uszFunction, "gafAsyncKeyState") == 0)
				{
					gafAsyncKeyStateExport = eat_map_entry->vaFunction;

					break;
				}
			}

			VMMDLL_MemFree(eat_map);
			eat_map = NULL;
			if (gafAsyncKeyStateExport > 0x7FFFFFFFFFFF)
				return true;
			return false;
		}
	}

	void UpdateKeys()
	{
		uint8_t previous_key_state_bitmap[64] = { 0 };
		memcpy(previous_key_state_bitmap, state_bitmap, 64);

		VMMDLL_MemReadEx(DMA_HANDLE, this->win_logon_pid | VMMDLL_PID_PROCESS_WITH_KERNELMEMORY, gafAsyncKeyStateExport, reinterpret_cast<PBYTE>(&state_bitmap), 64, NULL, VMMDLL_FLAG_NOCACHE);
		for (int vk = 0; vk < 256; ++vk)
			if ((state_bitmap[(vk * 2 / 8)] & 1 << vk % 4 * 2) && !(previous_key_state_bitmap[(vk * 2 / 8)] & 1 << vk % 4 * 2))
				previous_state_bitmap[vk / 8] |= 1 << vk % 8;
	}
	
	bool IsKeyDown(uint32_t virtual_key_code)
	{
		if (gafAsyncKeyStateExport < 0x7FFFFFFFFFFF)
			return false;
		return state_bitmap[(virtual_key_code * 2 / 8)] & 1 << virtual_key_code % 4 * 2;
	}

#if COUNT_TOTAL_READSIZE

	static DWORD64 GetTotalReadSize();

	static void ResetReadSize();

#endif
};

template <typename T>
class DMAScatter
{
	T value;
	void* address;
	DMAHandler* DMA;
	VMMDLL_SCATTER_HANDLE handle;
	bool unknown = true;

	void prepare()
	{
		DMA->queueScatterReadEx(handle, reinterpret_cast<uint64_t>(address), &value, sizeof(T));
	}
public:
	DMAScatter(DMAHandler* DMAHandler, VMMDLL_SCATTER_HANDLE handle, void* address)
		: address(address), DMA(DMAHandler), handle(handle)
	{
		if (!handle) DMAHandler::log("Invalid handle!");

		memset(&value, 0, sizeof(T));

		prepare();
	}

	DMAScatter(DMAHandler* DMAHandler, VMMDLL_SCATTER_HANDLE handle, uint64_t address)
		: address(reinterpret_cast<void*>(address)), DMA(DMAHandler), handle(handle)
	{
		if (!handle) DMAHandler::log("Invalid handle!");

		memset(&value, 0, sizeof(T));

		prepare();
	}


	T& operator*()
	{
		return value;
	}
};
