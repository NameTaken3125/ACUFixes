#include "pch.h"

/*
* Thank you, kamzik123, for the AnvilToolKit and its FileLists.

To convert the 8-byte handles into readable strings, this utilizes an external "handles map" file
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
    std::string FindNameForHandle_fromfile(uint64 handle);
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
}
namespace ACU::Handles
{
std::string HandlesMap::FindNameForHandle_binsearchinmemory(uint64 handle)
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
    static HandlesMapInfo wholeFile = []()
    {
        std::vector<char> resultWholeFile;
        fs::path filepath = AbsolutePathInThisDLLDirectory(g_HandlesMapFilename);
        std::ifstream infile(filepath, std::ios_base::binary);
        infile.seekg(0, infile.end);
        auto filesize = infile.tellg();
        resultWholeFile.resize(filesize);
        infile.seekg(0, infile.beg);
        infile.read(&resultWholeFile[0], filesize);

        HandlesMapInfo result(std::move(resultWholeFile));

        return result;
    }();
    auto FindStringForHandleInFile = [&]() -> std::string
    {
        std::string result;
        int left = 0;
        int right = wholeFile.numHandles;
        while (left <= right)
        {
            uint32 mid = (left + right) / 2;
            const HandleDescriptor& curHandle = wholeFile.whereHandlesStart[mid];
            if (curHandle.handle == handle)
            {
                char* whereCorrespondingStringStarts = &wholeFile.m_fileContents[curHandle.offsetInFileWhereCorrespondingStringStarts];
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
    return "";
}
std::string HandlesMap::FindNameForHandle_fromfile(uint64 handle)
{
    fs::path filepath = AbsolutePathInThisDLLDirectory("cppmap.handlesmap2");
    auto FindStringForHandleInFile = [&]() -> std::string
    {
        std::string result;
        std::ifstream infile(filepath, std::ios_base::binary);
        uint32 numHandles;
        infile.read(reinterpret_cast<char*>(&numHandles), sizeof(numHandles));
        uint32 whereHandlesStart = 0 + sizeof(numHandles);
        uint32 sizeOfHandleDescriptor = 8 + 4; // handle (8 bytes) + offsetInFileWhereTheCorrespondingStringStarts (4 bytes)
        uint32 whereStringsStart = whereHandlesStart + numHandles * sizeOfHandleDescriptor;
        uint32 left = 0;
        uint32 right = numHandles;
        while (left < right)
        {
            uint32 mid = (left + right) / 2;
            infile.seekg(whereHandlesStart + mid * sizeOfHandleDescriptor);
            uint64 curHandle;
            infile.read(reinterpret_cast<char*>(&curHandle), sizeof(curHandle));
            if (curHandle == handle)
            {
                uint32 offsetInFileWhereTheCorrespondingStringStarts;
                infile.read(reinterpret_cast<char*>(&offsetInFileWhereTheCorrespondingStringStarts), sizeof(offsetInFileWhereTheCorrespondingStringStarts));
                infile.seekg(offsetInFileWhereTheCorrespondingStringStarts);
                std::getline(infile, result, '\0');
                return result;
            }
            if (curHandle < handle)
            {
                left = mid;
            }
            else
            {
                right = mid;
            }
        }
        return result;
    };
    std::string foundName = FindStringForHandleInFile();
    if (!foundName.empty())
    {
        return foundName;
    }
    return "";
}

}
