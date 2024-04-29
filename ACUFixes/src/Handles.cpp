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

#include "Handles.h"

namespace fs = std::filesystem;

const char* g_HandlesMapFilename = "HandlesLUT-ACUnity.handlesmapcpp";

namespace ACU::Handles
{
class HandlesMap
{
public:
    std::string FindNameForHandle(uint64 handle) { return FindNameForHandle_binsearchinmemory(handle); }
    std::string FindNameForHandle_binsearchinmemory(uint64 handle);
    static HandlesMap& GetSingleton() {
        static HandlesMap singleton;
        return singleton;
    }
};
HandleString HandleToText(uint64 handle)
{
    HandleString result;
    std::string foundText = HandlesMap::GetSingleton().FindNameForHandle(handle);
    memcpy_s(&result.m_buffer[0], result.m_buffer.size(), foundText.c_str(), foundText.size() + 1);
    return result;
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
std::string MakeHandleString_HandlesmapNotLoaded(uint64 handle)
{
    return "";
}
std::string MakeHandleString_HandleNotRecognized(uint64 handle)
{
    return "";
}
std::string HandlesMap::FindNameForHandle_binsearchinmemory(uint64 handle)
{
    if (!g_LoadedHandlesmap)
    {
        LoadHandlesmapFile();
        if (!g_LoadedHandlesmap)
        {
            return MakeHandleString_HandlesmapNotLoaded(handle);
        }
    }
    auto FindStringForHandleInFile = [&]() -> std::string
    {
        std::string result;
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
    std::string foundName = FindStringForHandleInFile();
    if (!foundName.empty())
    {
        return foundName;
    }
    return MakeHandleString_HandleNotRecognized(handle);
}

} // namespace ACU::Handles
