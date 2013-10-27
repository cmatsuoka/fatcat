#include <cstdio>
#include <ctype.h>
#include <cstdlib>
#include <iostream>
#include <string.h>
#include "FatEntry.h"
#include "utils.h"

using namespace std;

FatEntry::FatEntry()
    : hasData(false),
      address(0)
{
}

void FatEntry::setData(string data_)
{
    hasData = true;
    data = data_;
}
        
void FatEntry::updateData()
{
    FAT_WRITE_LONG(data, FAT_FILESIZE, size&0xffffffff);
    FAT_WRITE_SHORT(data, FAT_CLUSTER_LOW, cluster&0xffff);
    FAT_WRITE_SHORT(data, FAT_CLUSTER_HIGH, (cluster>>16)&0xffff);
}
        
string FatEntry::getFilename()
{
    if (longName != "") {
        return longName;
    } else {
        string name;
        string ext = trim(shortName.substr(8,3));
        string base = trim(shortName.substr(0,8));

        if (isErased()) {
            base = base.substr(1);
        }

        name = base;

        if (ext != "") {
            name += "." + ext;
        }

        return name;
    }
}
        
bool FatEntry::isDirectory()
{
    return attributes&FAT_ATTRIBUTES_DIR;
}

bool FatEntry::isHidden()
{
    return attributes&FAT_ATTRIBUTES_HIDE;
}

bool FatEntry::isErased()
{
    return ((shortName[0]&0xff) == FAT_ERASED);
}

bool FatEntry::isZero()
{
    for (int i=0; i<data.size(); i++) {
        if (data[i] != 0) {
            return false;
        }
    }

    return true;
}

bool FatEntry::isCorrect()
{
    if (attributes && !(attributes&FAT_ATTRIBUTES_DIR) && !(attributes&FAT_ATTRIBUTES_FILE)) {
        return false;
    }

    for (int i=1; i<11; i++) {
        if (!isprint(data[i])) {
            return false;
        }
    }

    if (cluster == 0 && getFilename()!="..") {
        return false;
    }

    return true;
}