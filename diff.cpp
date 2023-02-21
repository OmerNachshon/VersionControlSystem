#include "files.h"
#include "diff.h"
#include <vector>
#include <iostream>
#include <string>

using namespace std;
bool compareRows(char *row1, int size1, char *row2, int size2)
{
    if (size1 != size2)
        return false;
    for (size_t i = 0; i < size1; i++)
    {
        if (row1[i] != row2[i])
            return false;
    }
    return true;
}

void diff(File *f1, File *f2)
{
    int size1, size2, max, min;
    char **rows1 = read_lines(f1, &size1);
    char **rows2 = read_lines(f2, &size2);
    if (size1 > size2)
    {
        max = size1;
        min = size2;
    }
    else
    {
        max = size2;
        min = size1;
    }
    std::vector<std::string> v1(max);
    std::vector<std::string> v2(max);
    for (size_t i = 0; i < min; i++)
    {
        if (!compareRows(rows1[i], size1, rows2[i], size2))
        {
            std::string rowIndex = std::to_string(i + 1);
            v1.push_back("+ " + rowIndex + " " + rows1[i]);
            v2.push_back("- " + rowIndex + " " + rows2[i]);
        }
    }
    if (max == size1)
    {
        for (size_t i = min; i < max; i++)
        {
            std::string rowIndex = std::to_string(i + 1);
            v1.push_back("+ " + rowIndex + " " + rows1[i]);
        }
    }
    if (max == size2)
    {
        for (size_t i = min; i < max; i++)
        {
            std::string rowIndex = std::to_string(i + 1);
            v2.push_back("- " + rowIndex + " " + rows2[i]);
        }
    }
    // print differences
    for (string str1 : v1)
        std::cout << str1 << '\n';
    cout << "\n\n\n";
    for (string str2 : v2)
        std::cout << str2 << '\n';
    cout << "\n\n\n";
}

int main(int argc, char const *argv[]) // for testing
{
    /* code */
    return 0;
}
