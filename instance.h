#ifndef INSTANCE_H
#define INSTANCE_H

#include "attribute.h"
#include <string>
#include <vector>
#include <iostream>

using namespace std;

class Instance{
 public:
//Get and Set functions for attributes and attribute values 
  void Set_Attributes(const vector<Attribute>& c);
  string Get_Nominal_Value(int attribute_num);
  double Get_Numeric_Value(int attribute_num);
  string Get_Attribute_Name(int attribute_num);
  int Get_Num_Attributes(); 
//Adds a value to a particular attribute after checking if its a valid attribute or not 
  bool Add_Value(int attribute_num, string val);
//Checks if the Attribute is numeric or not 
  bool Is_Numeric_Attribute(int attribute_num);

 protected:
//Checks if the attribute is a valid one 
  bool Is_Valid_Attribute(int attribute_num);
 private:
  
  vector<Attribute> Attribute_List;
  vector<string> Element_List;
};

#endif
