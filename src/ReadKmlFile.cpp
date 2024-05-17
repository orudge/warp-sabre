
#include "ReadKmlFile.h"
#include "StringUtils.h"

//pkg-config libxml++-2.6 --cflags --libs
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <iostream>
#include <string>
#include <vector>
#include <stdlib.h>
using namespace std;

void print_indentation(unsigned int indentation)
{
  //for(unsigned int i = 0; i < indentation; ++i)
  //  std::cout << " ";
}

class ProcessKml
{
public:
	string imgFilename;
	class Tile tile;

	ProcessKml() {};
	void ProcessNode(const xmlNodePtr node, vector<string> nodeNames, unsigned int indentation = 0);
};

void ProcessKml::ProcessNode(const xmlNodePtr node, vector<string> nodeNames, unsigned int indentation)
{
  //std::cout << std::endl; //Separate nodes by an empty line.
  
  if( XML_TEXT_NODE == node->type && xmlIsBlankNode( node )) //Let's ignore the indenting - you don't always want to do this.
    return;
    
  const xmlChar* nodename = node->name;

  if( XML_TEXT_NODE != node->type && XML_COMMENT_NODE != node->type && nodename) //Let's not say "name: text".
  {
	nodeNames.push_back(reinterpret_cast<const char*>( nodename ));
    print_indentation(indentation);
    //std::cout << "Node name = " << node->get_name() << std::endl;
    //std::cout << "Node name = " << nodename << std::endl;
  }
  else if(XML_TEXT_NODE == node->type) //Let's say when it's text. - e.g. let's say what that white space is.
  {
	//for(unsigned int i=0;i<nodeNames.size();i++)
	//{
	//	cout << nodeNames[i] << ",";
	//}
	//cout << endl;
    print_indentation(indentation);
    //std::cout << "Text Node" << std::endl;
  }

  //Treat the various node types differently: 
  if(XML_TEXT_NODE == node->type)
  {
    print_indentation(indentation);
    //std::cout << "text = \"" << nodeText->get_content() << "\"" << std::endl;

	vector<string> pathImgFilename;
	pathImgFilename.push_back("kml");
	pathImgFilename.push_back("Folder");
	pathImgFilename.push_back("GroundOverlay");
	pathImgFilename.push_back("Icon");
	pathImgFilename.push_back("href");

	if(pathImgFilename == nodeNames)
	{
		imgFilename = reinterpret_cast<char*>( node->content );
	}

	vector<string> latLonBoxPath;
	latLonBoxPath.push_back("kml");
	latLonBoxPath.push_back("Folder");
	latLonBoxPath.push_back("GroundOverlay");
	latLonBoxPath.push_back("LatLonBox");
	int match = 1;
	//cout << latLonBoxPath.size() << endl;
	if(nodeNames.size() != 5) match = 0;
	else
	for(unsigned int i=0;i<latLonBoxPath.size() && i<nodeNames.size();i++)
	{
		if(latLonBoxPath[i] != nodeNames[i]) {match = 0; break;}
	}
	//cout << "match" << match << endl;
	if(match)
	{
		double val = atof(reinterpret_cast<const char *>(node->content));
		if(nodeNames[4] == "west") {tile.lonmin = val; }
		if(nodeNames[4] == "east") {tile.lonmax = val; }
		if(nodeNames[4] == "south") {tile.latmin = val; }
		if(nodeNames[4] == "north") {tile.latmax = val;}
	}


  }
  else if(XML_COMMENT_NODE == node->type)
  {
    print_indentation(indentation);
    //std::cout << "comment = " << node->content << std::endl;
  }
  else if(XML_TEXT_NODE == node->type)
  {
    print_indentation(indentation);
    //std::cout << "content = " << node->content << std::endl;
  }
  else if(XML_ELEMENT_NODE == node->type) 
  {
    //A normal Element node:
    xmlElementPtr nodeElement = ( xmlElementPtr ) node;

    //line() works only for ElementNodes.
    print_indentation(indentation);
    //std::cout << "     line = " << node->get_line() << std::endl;

    //Print attributes:
    for(xmlAttributePtr attribute = nodeElement->attributes; attribute != NULL; attribute = (xmlAttributePtr) attribute->next )
    {
     // const xmlpp::Attribute* attribute = *iter;
     // print_indentation(indentation);
     // std::cout << "  Attribute " << attribute->get_name() << " = " << attribute->get_value() << std::endl;

     string attrName = reinterpret_cast<const char*>( attribute->name );

     if( "title" == attrName )
     {
        //std::cout << "title found: =" << attribute->get_value() << std::endl;
     }
    }
   
    //Recurse through child nodes:
    for( xmlNodePtr nodeChild = node->children; nodeChild != NULL; nodeChild = nodeChild->next )
    {
      ProcessNode(nodeChild, nodeNames, indentation + 2); //recursive
    }
  }
}


int ReadKmlFile(const char *filename, class Tile &tileOut, string &imgFilenameOut)
{
	imgFilenameOut = "";

	try
	{
	xmlDocPtr doc = xmlReadFile( filename, NULL, 0 );

	if(doc)
	{
		//Walk the tree:
                xmlNodePtr pNode = doc->children;

		class ProcessKml processKml;
		vector<string> empty;
		processKml.ProcessNode(pNode, empty);

		imgFilenameOut = RemoveFilePath( processKml.imgFilename.c_str() );
		tileOut = processKml.tile;

                xmlFreeDoc( doc );
	}
	}
	catch(const std::exception& ex)
	{
		std::cout << "Exception caught: " << ex.what() << std::endl;
		return -1;
	}

	return 1;
}

