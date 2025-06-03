#pragma once

#include <vector>
#include <fstream>

// Thanks, https://stackoverflow.com/questions/15138353/how-to-read-a-binary-file-into-a-vector-of-unsigned-chars/21802936#21802936
static std::vector<byte> readFile(const fs::path& filename)
{
    std::vector<byte> vec;

    // open the file:
    std::ifstream file(filename, std::ios::binary);
    if (!file) return vec;

    // Stop eating new lines in binary mode!!!
    file.unsetf(std::ios::skipws);

    // get its size:
    std::streampos fileSize;

    file.seekg(0, std::ios::end);
    fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // reserve capacity
    vec.reserve(fileSize);

    // read the data:
    vec.insert(vec.begin(),
        std::istream_iterator<byte>(file),
        std::istream_iterator<byte>());

    return vec;
}
