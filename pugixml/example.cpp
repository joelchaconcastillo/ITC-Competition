#include "pugixml.hpp"
#include <iostream>
using namespace std;

int main()
{
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file("iku-fal17.xml");
    if (!result)
        return -1;
    //for (pugi::xml_node tool: doc.child("Profile").child("Tools").children("Tool"))
   cout << doc.child("problem").child("rooms").child("room").attribute("capacity").value() <<endl;
//    cout << doc.child("problem").attribute("nrWeeks").value();
    for (pugi::xml_node tool: doc.child("problem").child("rooms"))
    {
    cout << tool.attribute("id").value() <<endl;
  //      if (timeout > 0)
  //          std::cout << "Tool " << tool.attribute("Filename").value() << " has timeout " << timeout << "\n";
    }
    cout << doc.child("problem").child("rooms").child("room").find_child_by_attribute("id", "capacity", "55");

    pugi::xml_document doc2;
    pugi::xml_node node = doc2.append_child("node");
    pugi::xml_node descr = node.append_child("description");
    descr.append_child(pugi::node_pcdata).set_value("Simple node");
    // add param node before the description
    pugi::xml_node param = node.insert_child_before("param", descr);
    
    // add attributes to param node
    param.append_attribute("name") = "version";
    param.append_attribute("value") = 1.1;
    param.insert_attribute_after("type", param.attribute("name")) = "float";
    doc2.save_file("save_file_output.xml");
}
