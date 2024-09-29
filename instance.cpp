#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include "instance.h"
#include "attribute.h"

using namespace std;

//Get and Set functions for attributes and attribute values
void Instance :: Set_Attributes(const vector<Attribute>& c){
	for(int i = 0; i < c.size(); i++){
		Attribute_List.push_back(c[i]);
	}
	
	Element_List.resize(c.size());
	
}

string Instance :: Get_Nominal_Value(int attribute_num){
	if(Is_Valid_Attribute(attribute_num)){
		//if(!Is_Numeric_Attribute(attribute_num)){
			return Element_List[attribute_num]; 
		//}
	}
}

double Instance :: Get_Numeric_Value(int attribute_num){
	if(Is_Valid_Attribute(attribute_num)){
		//if(Is_Numeric_Attribute(attribute_num)){
			return stod(Element_List[attribute_num]); 
		//}
	}
}

string Instance :: Get_Attribute_Name(int attribute_num){
	return Attribute_List[attribute_num].Get_Name(); 
}

int Instance :: Get_Num_Attributes(){
	int count = 0; 
	for(int i = 0; i < Attribute_List.size(); i++){
		count ++; 
	}
	
	return count; 
  }
//Adds a value to a particular attribute after checking if its a valid attribute or not 
bool Instance :: Add_Value(int attribute_num, string val){
	if(Is_Valid_Attribute(attribute_num)){
		Element_List[attribute_num] = val; 
		if(Attribute_List[attribute_num].In_Domain(val)){
		return true;	 
		}
	}
	return false; 
}
//Checks if the Attribute is numeric or not 
bool Instance :: Is_Numeric_Attribute(int attribute_num){
	if(Is_Valid_Attribute(attribute_num)){
		if(Element_List[attribute_num] >= "0" &&  Element_List[attribute_num] <= "9"){
			return true; 
		}
	}
	return false; 	
		
}
//Checks if the attribute is a valid one 
bool Instance :: Is_Valid_Attribute(int attribute_num){
	if( 0 <= attribute_num && attribute_num <= (Attribute_List.size() -1)){
		return true; 
	}
	return false; 
  }

	