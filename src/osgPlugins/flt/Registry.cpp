
#include <osg/Node>
#include <osg/Group>
#include <osg/Notify>

#include <algorithm>
#include <set>

#include "Record.h"
#include "Input.h"
#include "FltFile.h"
#include "Registry.h"
/*
#ifdef OSG_USE_IO_DOT_H
//#include <ostream.h>
#else
#include <iostream>
using namespace std;
#endif
*/
//#include <stdio.h>

using namespace flt;


// static
Registry* Registry::instance()
{
    static Registry s_nodeFactory;
    return &s_nodeFactory;
}


void Registry::addPrototype(Record* rec)
{
    if (rec==0L) return;

    osg::notify(osg::INFO) << "flt::Registry::addPrototype("<< rec->className()<<")\n";
    int op = rec->classOpcode();
    _recordProtoMap[op] = rec;
}


Record* Registry::getPrototype(const int opcode)
{
    RecordProtoMap::iterator itr = _recordProtoMap.find(opcode);
    if (itr != _recordProtoMap.end())
        return (*itr).second.get();

    return NULL;

}

///////////////////////////////////////////////////////////////////


void Registry::addTexture(const std::string& name, osg::Texture* texture)
{
    if (texture == NULL) return;
    _textureMap[name] = texture;
}


osg::Texture* Registry::getTexture(const std::string name)
{
    TextureMap::iterator itr = _textureMap.find(name);
    if (itr != _textureMap.end())
        return (*itr).second.get();

    return NULL;
}


void Registry::addFltFile(const std::string& name, FltFile* file)
{
    if (file == NULL) return;
    _fltFileMap[name] = file;
}


FltFile* Registry::getFltFile(const std::string& name)
{
    FltFileMap::iterator itr = _fltFileMap.find(name);
    if (itr != _fltFileMap.end())
        return (*itr).second.get();

    return NULL;
}

