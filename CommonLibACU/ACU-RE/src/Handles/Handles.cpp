#include "pch.h"

/*
* Thank you, kamzik123, for the AnvilToolKit and its FileLists.

To convert the 8-byte handles into readable strings, this module utilizes an external "handles map" file
that was created from the File Lists that AnvilToolKit provides (thank you, kamzik123).
The "handles map" file is a binary file optimized for binary searching through the handles.
Its structure is simply as follows:

    numHandles,
    handle, offsetInFile
    handle, offsetInFile
    ...
    handle, offsetInFile
    Cstring, Cstring, Cstring, ...

with handles sorted in ascending order.
*/

#include "Handles/Handles.h"


#define DICT_OF_HANDLES_FILENAME "HandlesLUT-ACUnity.handlesmapcpp"
const char* g_HandlesMapFilename = DICT_OF_HANDLES_FILENAME;


namespace ACU::Handles
{
static fs::path GetThisDLLAbsoluteFilepath()
{
    auto GetThisDLLHandle = []() -> HMODULE
        {
            // A variable within the module.
            static char dummyChar;
            HMODULE dllHandle = NULL;
            GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                (LPCSTR)&dummyChar,
                &dllHandle);
            return dllHandle;
        };

    fs::path result;
    HMODULE dllHandle = GetThisDLLHandle();
    if (dllHandle == NULL) return result;
    wchar_t path[MAX_PATH];

    GetModuleFileNameW(dllHandle, path, (DWORD)std::size(path));
    return fs::path(path);
}
static fs::path AbsolutePathInThisDLLDirectory(const fs::path& filenameRel)
{
    fs::path result;
    static fs::path thisDLLPath = GetThisDLLAbsoluteFilepath();
    if (thisDLLPath.empty()) return result;
    return thisDLLPath.parent_path() / filenameRel;
}


class HandlesMap
{
public:
    const char* FindNameForHandle(uint64 handle) { return FindNameForHandle_binsearchinmemory(handle); }
    const char* FindNameForHandle_binsearchinmemory(uint64 handle);
    static HandlesMap& GetSingleton() {
        static HandlesMap singleton;
        return singleton;
    }
};
const char* HandleToText(uint64 handle)
{
    return HandlesMap::GetSingleton().FindNameForHandle(handle);
}
} // namespace ACU::Handles
namespace ACU::Handles
{
#pragma pack(push, 1)
struct HandleDescriptor
{
    uint64 handle;
    uint32 offsetInFileWhereCorrespondingStringStarts;
};
#pragma pack(pop)
assert_sizeof(HandleDescriptor, 12);
struct HandlesMapInfo
{
    std::vector<char> m_fileContents;
    uint32 numHandles;
    HandleDescriptor* whereHandlesStart;
    HandlesMapInfo(std::vector<char>&& fileContents)
        : m_fileContents(std::move(fileContents))
    {
        numHandles = (uint32&)m_fileContents[0];
        whereHandlesStart = (HandleDescriptor*)(&m_fileContents[0 + sizeof(numHandles)]);
    }
};
static std::optional<HandlesMapInfo> g_LoadedHandlesmap;
void LoadHandlesmapFile()
{
    std::vector<char> resultWholeFile;
    fs::path filepath = AbsolutePathInThisDLLDirectory(g_HandlesMapFilename);
    std::ifstream infile(filepath, std::ios_base::binary);
    if (!infile)
    {
        return;
    }
    infile.seekg(0, infile.end);
    auto filesize = infile.tellg();
    resultWholeFile.resize(filesize);
    infile.seekg(0, infile.beg);
    infile.read(&resultWholeFile[0], filesize);

    g_LoadedHandlesmap.emplace(std::move(resultWholeFile));
};
const char* MakeHandleString_HandlesmapNotLoaded(uint64 handle)
{
    return "<HANDLE NOT RECOGNIZED: \"" DICT_OF_HANDLES_FILENAME "\" FILE NOT LOADED>";
}
const char* MakeHandleString_HandleNotRecognized(uint64 handle)
{
    return "<HANDLE NOT RECOGNIZED>";
}
enum class HandlesDictionaryNotLoadedWhy
{
    AttemptedToLoad_WasntFound = 0,
    DidntAttemptToLoadYet,
};
HandlesDictionaryNotLoadedWhy g_HandlesDictState = HandlesDictionaryNotLoadedWhy::DidntAttemptToLoadYet;
const char* HandlesMap::FindNameForHandle_binsearchinmemory(uint64 handle)
{
    if (!g_LoadedHandlesmap)
    {
        switch (g_HandlesDictState)
        {
        case HandlesDictionaryNotLoadedWhy::AttemptedToLoad_WasntFound:
            return MakeHandleString_HandlesmapNotLoaded(handle);
        case HandlesDictionaryNotLoadedWhy::DidntAttemptToLoadYet:
            LoadHandlesmapFile();
            if (g_LoadedHandlesmap)
                break;
            g_HandlesDictState = HandlesDictionaryNotLoadedWhy::AttemptedToLoad_WasntFound;
            return MakeHandleString_HandlesmapNotLoaded(handle);
        default:
            return MakeHandleString_HandlesmapNotLoaded(handle);
        }
    }
    auto FindStringForHandleInFile = [&]() -> const char*
    {
        const char* result = nullptr;
        int left = 0;
        int right = g_LoadedHandlesmap->numHandles;
        while (left <= right)
        {
            uint32 mid = (left + right) / 2;
            const HandleDescriptor& curHandle = g_LoadedHandlesmap->whereHandlesStart[mid];
            if (curHandle.handle == handle)
            {
                char* whereCorrespondingStringStarts = &g_LoadedHandlesmap->m_fileContents[curHandle.offsetInFileWhereCorrespondingStringStarts];
                result = whereCorrespondingStringStarts;
                return result;
            }
            if (curHandle.handle < handle)
            {
                left = mid + 1;
            }
            else
            {
                right = mid - 1;
            }
        }
        return result;
    };
    const char* foundName = FindStringForHandleInFile();
    if (foundName)
    {
        return foundName;
    }
    return MakeHandleString_HandleNotRecognized(handle);
}

} // namespace ACU::Handles
