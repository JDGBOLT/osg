/**********************************************************************
 *
 *    FILE:            ProxyNode.cpp
 *
 *    DESCRIPTION:    Read/Write osg::ProxyNode in binary format to disk.
 *
 *    CREATED BY:        Auto generated by iveGenerate
 *                    and later modified by Rune Schmidt Jensen.
 *
 *    HISTORY:        Created 24.3.2003
 *
 *    Copyright 2003 VR-C
 **********************************************************************/

#include <osgDB/WriteFile>
#include <osgDB/ReadFile>
#include <osgDB/FileUtils>
#include <osgDB/FileNameUtils>

#include "Exception.h"
#include "ProxyNode.h"
#include "Node.h"

using namespace ive;
/*
for(osgDB::FilePathList::iterator itr=osgDB::getDataFilePathList().begin(); itr!=osgDB::getDataFilePathList().end(); ++itr)
printf("#1######%s\n", itr->c_str());
for(osgDB::FilePathList::const_iterator itrO=in->getOptions()->getDatabasePathList().begin(); itrO!=in->getOptions()->getDatabasePathList().end(); ++itrO)
printf("#2######%s\n", itrO->c_str());
namespace osgDB {
class PushAndPopDataPath
{
    public:
        PushAndPopDataPath(const std::string& path)
        {
            getDataFilePathList().push_front(path);
        }

        ~PushAndPopDataPath()
        {
            getDataFilePathList().pop_front();
        }
};
}
*/

void ProxyNode::write(DataOutputStream* out)
{
    // Write ProxyNode's identification.
    out->writeInt(IVEPROXYNODE);

    // If the osg class is inherited by any other class we should also write this to file.
    osg::Node* node = dynamic_cast<osg::Node*>(this);
    if(node)
    {
        static_cast<ive::Node*>(node)->write(out);
    }
    else
        throw Exception("ProxyNode::write(): Could not cast this osg::ProxyNode to an osg::Node.");

    out->writeFloat(getRadius());
    out->writeInt(getCenterMode());
    out->writeVec3(getCenter());

    out->writeUInt(getNumFileNames());
    unsigned int numChildrenToWriteOut = 0;
    unsigned int i;
    for(i=0; i<getNumFileNames(); i++)
    {
        if (getFileName(i).empty())
        {
            out->writeString("");
            ++numChildrenToWriteOut;
        }
        else
        {
            if(out->getUseOriginalExternalReferences())
            {
                out->writeString(getFileName(i));
            }
            else
            {
                std::string ivename = osgDB::getFilePath(getFileName(i)) +"/"+ osgDB::getStrippedName(getFileName(i)) +".ive";
                out->writeString(ivename);
            }
        }
    }

    if(out->getIncludeExternalReferences()) //--------- inlined mode
    {
        out->writeUInt(getNumChildren());
        for(i=0; i<getNumChildren(); i++)
        {
            out->writeNode(getChild(i));
        }
    }
    else //----------------------------------------- no inlined mode
    {
        out->writeUInt(numChildrenToWriteOut);

        for(i=0; i<getNumFileNames(); i++)
        {
            if (getFileName(i).empty())
            {
                out->writeNode(getChild(i));
            }
            else if(out->getWriteExternalReferenceFiles())
            {
                if(out->getUseOriginalExternalReferences())
                {
                    osgDB::writeNodeFile(*getChild(i), getFileName(i));
                }
                else
                {
                    std::string ivename = osgDB::getFilePath(getFileName(i)) +"/"+ osgDB::getStrippedName(getFileName(i)) +".ive";
                    osgDB::writeNodeFile(*getChild(i), ivename);
                }
            }
        }
    }
}

void ProxyNode::read(DataInputStream* in)
{
    // Peek on ProxyNode's identification.
    int id = in->peekInt();
    if(id == IVEPROXYNODE)
    {
        // Read ProxyNode's identification.
        id = in->readInt();
        // If the osg class is inherited by any other class we should also read this from file.
        osg::Node* node = dynamic_cast<osg::Node*>(this);
        if(node)
        {
            ((ive::Node*)(node))->read(in);
        }
        else
            throw Exception("ProxyNode::read(): Could not cast this osg::ProxyNode to an osg::Node.");

        if (in->getOptions() && !in->getOptions()->getDatabasePathList().empty())
        {
            const std::string& path = in->getOptions()->getDatabasePathList().front();
            if (!path.empty()) 
            {
                setDatabasePath(path);
            }
        }

        setRadius(in->readFloat());
        setCenterMode((osg::ProxyNode::CenterMode)in->readInt());
        setCenter(in->readVec3());

        unsigned int numFileNames = in->readUInt();
        unsigned int i;
        for(i=0; i<numFileNames; i++)
        {
            setFileName(i, in->readString());
        }

        unsigned int numChildren = in->readUInt();
        for(i=0; i<numChildren; i++)
        {
            osgDB::FilePathList& fpl = ((osgDB::ReaderWriter::Options*)in->getOptions())->getDatabasePathList();
            fpl.push_front( fpl.empty() ? osgDB::getFilePath(getFileName(i)) : fpl.front()+'/'+ osgDB::getFilePath(getFileName(i)));
            addChild(in->readNode());            
            fpl.pop_front();
        }

        for(i=0; i<numFileNames; i++)
        {
            if(i>=numChildren && !getFileName(i).empty())
            {
                osgDB::FilePathList& fpl = ((osgDB::ReaderWriter::Options*)in->getOptions())->getDatabasePathList();
                fpl.push_front( fpl.empty() ? osgDB::getFilePath(getFileName(i)) : fpl.front()+'/'+ osgDB::getFilePath(getFileName(i)));
                osg::Node *node = osgDB::readNodeFile(getFileName(i), in->getOptions());
                fpl.pop_front();
                if(node)
                {
                    insertChild(i, node);
                }
            }
        }
    }
    else
    {
        throw Exception("ProxyNode::read(): Expected ProxyNode identification.");
    }
}
