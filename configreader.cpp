#include "configreader.h"

#include <QDebug>
#include <QFile>
#include <vector>
#include <iostream>
#include <QCoreApplication>
#include "boost/property_tree/ptree.hpp"
#include "boost/property_tree/xml_parser.hpp"


using boost::property_tree::wptree;

template<class Ptree>
inline const Ptree &empty_ptree()
{
    static Ptree pt;
    return pt;
}


ConfigReader::ConfigReader()
{

}

ConfigReader::~ConfigReader()
{

}

void ConfigReader::readSettings()
{

    QString configname = qApp->applicationDirPath().append("/conf.xml");
    try {
        wptree treeSettings;
        boost::property_tree::read_xml(configname.toStdString(), treeSettings);

        host = treeSettings.get<std::wstring>(L"settings.<xmlattr>.host");
        progid = treeSettings.get<std::wstring>(L"settings.<xmlattr>.progid");
        foreach (const wptree::value_type& sensor, treeSettings.get_child(L"settings", empty_ptree<wptree>()))
        {
            const wptree &tree = sensor.second;
            if(sensor.first != L"<xmlattr>")
            {
                items.push_back(ItemOPC(tree.get<std::wstring>(L"<xmlattr>.itemid")));
            }
        }
    }
    catch (const boost::property_tree::ptree_error&) {
        std::cout << ">  conf.xml read error" << std::endl;
        exit(0);
    }
}

std::wstring ConfigReader::getHost() const
{
    return host;
}

std::wstring ConfigReader::getProgId() const
{
    return progid;
}

std::vector<ItemOPC> ConfigReader::getItems() const
{
    return items;
}

