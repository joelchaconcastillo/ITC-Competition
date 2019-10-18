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
}
