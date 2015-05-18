#ifndef CONFIGREADER_H
#define CONFIGREADER_H

#include <vector>
#include <string>

#include "server.h"

class ConfigReader
{
public:
    ConfigReader();
    ~ConfigReader();
    void readSettings();

    std::wstring getHost() const;
    std::wstring getProgId() const;
    std::vector<ItemOPC> getItems() const;
private:
    std::wstring host;
    std::wstring progid;
    std::vector<ItemOPC> items;

};

#endif // CONFIGREADER_H
