#include "files.h"
#include "diff.h"
#include <vector>
#include <iostream>
#include <string>
#include <cstring>

using namespace std;
bool compareRows(char *row1, int size1, char *row2, int size2)
{
    if (size1 != size2)
        return false;
    for (int i = 0; i < size1; i++)
    {
        if (row1[i] != row2[i])
            return false;
    }
    return true;
}

void diff(File *src, File *dest)
{
    int size1, size2, max, min;
    char **rows1 = read_lines(src, &size1);
    char **rows2 = read_lines(dest, &size2);
    // src, dest
    // src, is the original file, the older file
    // dest, is the newer file, that is being compared

    // need to find removed, added, changed rows.
    // row that is removed is like changed, and marked as -
    // row that is added will be marked as +

    // find bigger size
    max = size1 > size2 ? size1 : size2;
    min = size1 > size2 ? size2 : size1;

    std::vector<std::string> v1(max);
    std::vector<std::string> v2(max);
    for (int i = 0; i < min; i++)
    {
        if (!compareRows(rows1[i], strlen(rows1[i]), rows2[i], strlen(rows2[i])))
        {
            std::string rowIndex = std::to_string(i + 1);
            v1.push_back("+ " + rowIndex + " " + rows1[i]);
            v2.push_back("- " + rowIndex + " " + rows2[i]);
        }
    }

    if (max == size1)
    {
        for (int i = min; i < max; i++)
        {
            std::string rowIndex = std::to_string(i + 1);
            v1.push_back("+ " + rowIndex + " " + rows1[i]);
        }
    }
    if (max == size2)
    {
        for (int i = min; i < max; i++)
        {
            std::string rowIndex = std::to_string(i + 1);
            v2.push_back("- " + rowIndex + " " + rows2[i]);
        }
    }
    // print differences
    for (string str1 : v1)
        std::cout << str1 ;
    cout << "\n\n\n";
    for (string str2 : v2)
        std::cout << str2 ;
    cout << "\n\n\n";
}
