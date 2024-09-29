//Lauren Hollinger
//Homework 5

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <cmath>
#include "attribute.h"
#include "instance.h"

using namespace std;


// parses a line of data
void Parse_Data(string s, const vector<Attribute>&  attribute_list,
		vector<Instance>& example_list){
  Instance cur_instance;
  cur_instance.Set_Attributes(attribute_list);

  stringstream ss;
  ss.str(s);
  int cur_attribute = 0;
  string cur_val;
  while(ss >> cur_val){
    if(cur_val[cur_val.size()-1] == ',')
      cur_val = cur_val.substr(0, cur_val.size()-1); // trim off comma
    cur_instance.Add_Value(cur_attribute, cur_val);
    cur_attribute++;
  }
  if(cur_attribute != attribute_list.size()){
    cout << "ERROR: Wrong number of attributes on line: " << s << endl;
    exit(1);
  }
  example_list.push_back(cur_instance);
  
}

// gets a new attribute from an @attribute line
void New_Attribute(string s, vector<Attribute>& attribute_list){
	Attribute att1; 
	stringstream ss; 
	ss.str(s); 
	string info_type;
    ss >> info_type;
	
	string att_name;
    ss >> att_name;
	if (att_name[att_name.size() - 1] == ','){
			att_name = att_name.substr(0, att_name.size() - 1);
	}
	att1.Set_Name(att_name);
	
	string val_type;
    ss >> val_type;
	if (val_type[val_type.size() - 1] == ','){
			val_type = val_type.substr(0, val_type.size() - 1);
	}

	if(val_type == "numeric"){
		att1.Set_Numeric(true);
	}
	else if(val_type == "{"){
		att1.Set_Numeric(false);
		string category;
	    while(ss >> category){
			if(category!= "}"){
				if (category[category.size() - 1] == ',')
					category = category.substr(0, category.size() - 1);
				att1.Add_Category(category);
			}
		}
	}
  attribute_list.push_back(att1);
}
     
void Print_Data(vector<Attribute>& attribute_list, vector<Instance>& examples){
  char choice = 'y';
  while(choice == 'y'){
    cout << "We have " << examples.size() << " examples.  Which would you like? ";
    int cur_example;
    cin >> cur_example;

    cout << "Type 0 to see the all attributes" << endl;
    for(int i = 0; i < attribute_list.size(); i++){
      cout << "Type " << i+1 << " to see value of attribute " << attribute_list[i].Get_Name() << endl;
    }
    int which_attribute;
    cin >> which_attribute;
    for(int i = 0; i < attribute_list.size(); i++){
      if(which_attribute == 0 || which_attribute == i+1){
	if(attribute_list[i].Numeric())
	  cout << attribute_list[i].Get_Name() << ": "
	       << examples[cur_example].Get_Numeric_Value(i) << endl;
	else
	  cout << attribute_list[i].Get_Name() << ": "
	       << examples[cur_example].Get_Nominal_Value(i) << endl;
      }
    }
    cout << "Another? (y/n) ";
    cin >> choice;
  }
}

double Distance (Instance first, Instance second, const vector<Attribute>& attribute_list) {
	double dist = 0;
	for (int i =0; i<attribute_list.size()-1; i++){ //for all of the attributes but the classification
		if (!first.Is_Numeric_Attribute(i)){ //if the attribute is nominal
			if (first.Get_Nominal_Value(i) != second.Get_Nominal_Value(i)) //if the nominal values are different the distance is 1
				dist = dist + 1;
			else // if they are the same the distance is 0
				dist = dist + 0; 
		}
		else { //the attribute is numeric
			double val = (first.Get_Numeric_Value(i) - second.Get_Numeric_Value(i)) * (first.Get_Numeric_Value(i) - second.Get_Numeric_Value(i));
			dist = dist + val; //calculate the distance formula, without taking the squareroot
		}
	}
	dist = sqrt(dist); //now take the squareroot, after we have gone through all the intended attributes
	return dist; 	
}

void Find_Nearest_Neighbor(vector<Instance>& training_data, Instance new_inst, const vector<Attribute>& attribute_list, vector<int>& knn_index_vec){
	vector<double> knn_dist_vec; //initalize a new vector that will hold the distances parallel to the index vector
	knn_dist_vec.resize(knn_index_vec.size()); //resize to be the same size
	
	for (int i=0; i<training_data.size(); i++){ //go through all the traing data
		double dist = Distance(new_inst, training_data[i], attribute_list); //find the distance
		if (i < knn_index_vec.size()){ //for the first k elements in the training data
			knn_dist_vec[i] = dist; //initalize the vector with those distances as a starting point
			knn_index_vec[i] = i; // and their corresponding indexes will be held here
			
		}
		
		if (i >= knn_index_vec.size()){ //for the rest of the training data
			bool larger_dist_found = false;
			double largest_dist;
			int largest_dist_index = 0;
			for (int j =0; j< knn_dist_vec.size(); j++){ //now go through the distances vector (two goals: find largest distance in vector, and see if our current distance is smaller than any element in the vector)
				if (j == 0){
					largest_dist = knn_dist_vec[j]; //automatically initize the first distance as the largest
				}
				else { //for all other elements
					if (knn_dist_vec[j] > largest_dist){ //if the current element in dist vec is greater than largest_dist
						largest_dist = knn_dist_vec[j]; //make it the new largest dist
						largest_dist_index = j; //save the index of the new largest dist
					}
				}
				if (dist < knn_dist_vec[j]) {//if the current distance is less than one of the distances in the vector, set bool to true
					larger_dist_found = true;
				}
			}
			
			if (larger_dist_found){ //if we succesfully found a larger distance than our current in the vector than our current distance
				knn_dist_vec[largest_dist_index] = dist;
				knn_index_vec[largest_dist_index] = i;
			}	
			
		}
	}
			
	//after going through all the training data the knn_index_vec will hold the values of the indexes of the k nearest neighbors	
}


void Find_Min_Max (vector<Instance>& examples, vector<double>& max_vec, vector<double>& min_vec, vector<Attribute>& attribute_list){ //takes a vector of examples and the attribute list and puts the min and max of each attribute in a respective vector
	for (int i = 0; i < attribute_list.size() -1; i++){// for each attribute in attribute list besides the classification
		double min = 0;
		double max = 0;
		if (attribute_list[i].Numeric()){ //if the attribute is numeric find the min and the max, and if it isn't this if statement will be skipped and the 0's will be pushed back
			for (int j =0; j < examples.size(); j++){ //go through the examples
				if (j == 0) { //for the first example, set the min and the max to the value for that attribute
					min = examples[j].Get_Numeric_Value(i);
					max = examples[j].Get_Numeric_Value(i);
				}
				else { //if it is not the first example
					if (examples[j].Get_Numeric_Value(i) > max) //if the value is greater than the max
						max = examples[j].Get_Numeric_Value(i); //it becomes the new max
					if (examples[j].Get_Numeric_Value(i) < min) //if the value is less than the min
						min = examples[j].Get_Numeric_Value(i);//it becomes the new min
				}
			}
		}
		//after going through all the examples, pushback your min and max values into their respective vectors
		min_vec.push_back(min);
		max_vec.push_back(max);
		//cout << min_vec.size() << " " << max_vec.size() << endl;
	}
}

void Scale (Instance cur_inst, vector<double>& min, vector<double>& max, vector<Attribute>& attribute_list){
	for (int i=0; i<attribute_list.size() -1; i++){ //go through all of the attributes, but the last and scale them for the given Instance (we don't need to do anything if nominal or min=max)
		if (cur_inst.Is_Numeric_Attribute(i) && min[i] != max[i]){ //if the attribute is numeric and the min and max are not the same, scale the value
			double scaled_val = (cur_inst.Get_Numeric_Value(i) - min[i]) / (max[i] - min[i]);
			string str_scaled_val = to_string(scaled_val);
			cur_inst.Add_Value(i, str_scaled_val); //Overwrite the current value with the new scaled value
		}
	}
}

int main(){
  //read in the training data
  string filename;
  cout << "Enter the filename for your training data: ";
  cin >> filename;

  ifstream fin;
  fin.open(filename.data());
  while(!fin){
    cout << "File not found" << endl;
    cout << "Enter the filename: ";
    cin >> filename;
    fin.clear();
    fin.open(filename.data());
  }
  vector<Attribute> attribute_list;
  vector<Instance> examples;
  // file is open
  bool data_mode = false;
  string s;
  int num_read = 0;
  while(getline(fin, s)){
	  // cout << s << endl;
	  // cout << num_read << endl;
    if(s[s.size()-1] == '\r') 
      s = s.substr(0,s.size()-1);
    
    if(s.size() > 0 && s[0] != '%'){ // ignore comments
      if(data_mode){ // it's a line of data
	Parse_Data(s, attribute_list, examples);
      }
      else{
	// then it had better start with an '@'
	if(s[0] != '@'){
	  cout << "ERROR: ILLEGAL LINE: " << s << endl;
	  exit(1);
	}
	// is it @attribute?
	else if(s.substr(0,10) == "@attribute")
	  New_Attribute(s, attribute_list);
	//is it @data?
	else if(s.substr(0,5) == "@data")
	  data_mode = true;
	else{
	  cout << "ERROR: ILLEGAL LINE: " << s << endl;
	  exit(1);
	}
      }
    }
    num_read++;
    //cout << num_read << endl;
  }
  
	//After the training data is read, find the min and max for all attributes (except the classification)
	vector <double> min_vec;
	vector <double> max_vec;
	Find_Min_Max(examples, max_vec, min_vec, attribute_list);
		
		//cout << "Find Min max don" <<endl;
	//Scale the training data
	for (int j =0; j< examples.size(); j++){ //go through each of the data
		Scale(examples[j], min_vec, max_vec, attribute_list);
	}
  

  // cout << examples.size() <<endl; 
  // vector <string> Class = attribute_list[attribute_list.size()-1].Get_Category_List();
  // cout << Class.size() << endl; 
	
  //read in a new file of validation data
	cout << "Enter the filename for your validation data: ";
	cin >> filename;
	
	ifstream newfin;
    newfin.open(filename.data());
	while(!newfin){
		cout << "File not found" << endl;
		cout << "Enter the filename for your validation data: ";
		cin >> filename;
		newfin.clear();
		newfin.open(filename.data());
	}
	vector<Attribute> attribute_vec;
	vector<Instance> examples_vec;
	//file is open
	data_mode = false;
	num_read = 0;
	while(getline(newfin, s)){
    if(s[s.size()-1] == '\r')
      s = s.substr(0,s.size()-1);
    
    if(s.size() > 0 && s[0] != '%'){ // ignore comments
      if(data_mode){ // it's a line of data
	Parse_Data(s, attribute_vec, examples_vec);
      }
      else{
	// then it had better start with an '@'
	if(s[0] != '@'){
	  cout << "ERROR: ILLEGAL LINE: " << s << endl;
	  exit(1);
	}
	// is it @attribute?
	else if(s.substr(0,10) == "@attribute")
	  New_Attribute(s, attribute_vec);
	//is it @data?
	else if(s.substr(0,5) == "@data")
	  data_mode = true;
	else{
	  cout << "ERROR: ILLEGAL LINE: " << s << endl;
	  exit(1);
	}
      }
    }
    num_read++;
    //cout << num_read << endl;
  }
  
	

  
	//Scale the validation data
	for (int j =0; j< examples_vec.size(); j++){ //go through each of the data
		Scale(examples_vec[j], min_vec, max_vec, attribute_list);
	}
	
	//ask for user input for K
	
	vector<int> odd_k{1, 3, 5, 7, 9, 11};
	vector<double> k_errors(odd_k.size());
	
for (int a = 0; a < odd_k.size(); a++) {
	
	int k = odd_k[a]; //use the current k in the k odd vector
	
	vector<int> knn_index_vec;
	knn_index_vec.resize(k);
	
	//cout << examples_vec.size() <<endl; 
	double error_rate = 0;
		for (int i=0; i<examples_vec.size(); i++){//loop through the test data
			if (i%50 == 0) {
				cout << "Currently on example " << i << " from the test data." << endl;
			}
			//cout << "here" <<endl;
			Find_Nearest_Neighbor(examples, examples_vec[i], attribute_list, knn_index_vec); //find the nearest neighbor
			//cout << "here2" <<endl;
			vector<string> Class_Categories; //vector to hold the classification categories
			Class_Categories = attribute_list[attribute_list.size()-1].Get_Category_List();
			
			vector<int> Class_Totals(Class_Categories.size(), 0); //vector to hold the number of classification for each category from the knn's (size same as previous vector and fill with 0's)
			//cout << "here4" <<endl;
			for (int j =0; j < knn_index_vec.size(); j++){ //go through all the found k nearest neighbors
			//cout << "here3" <<endl;
				string knn_class = examples[knn_index_vec[j]].Get_Nominal_Value(attribute_list.size()-1); //find their class
				//cout << Class_Categories.size() <<endl;
				for (int p =0; p< Class_Categories.size(); p++){ //go through the categories vector
					if (Class_Categories[p] == knn_class) //if the current category we're looking at = the nn category
						Class_Totals[p] = Class_Totals[p] + 1; //add one to the same index(k) in the totals vector
				}
			}
			// cout << "here4" <<endl;
			// cout << Class_Totals.size() <<endl;
			//after the totals vector has been filled, we need to find out which classification has the highest total
			int highest_total = -1;
			int highest_total_index = -1;
			for (int j=0; j< Class_Totals.size(); j++){ //go through the vector
			//cout << "here5" <<endl;
				if (Class_Totals[j] > highest_total){
					highest_total = Class_Totals[j]; //save highest total for comparison
					highest_total_index = j; //save index
				}
				//cout << "here6" <<endl;	
			}
			
			//after looking through the whole vector: highest_total_index now holds the value of the index of the classification category that will be our guessed outcome for this current example
			
			string classification = Class_Categories[highest_total_index];//what is the found classification?
			string true_classification = examples_vec[i].Get_Nominal_Value(attribute_list.size()-1); //what is the true classification
			if (classification != true_classification){//if the classification does not match the true outcome
				error_rate = error_rate + 1; //add one to the error_rate
				cout << "Test Data " << i << " was misclassified. True class: " << true_classification << " Missclassified as: " << classification << " The Nearest Neighbors were: ";
				for (int j =0; j< knn_index_vec.size(); j++){
					cout << knn_index_vec[j] << " ";
				}
				cout << endl;
			}
		}
	
	cout << endl;
	double kerror = error_rate/examples_vec.size();
	cout << "The error rate for K: " << odd_k[a] << " is: " << kerror << endl;
	k_errors[a] = kerror;
	
	}

//cout << "here"  << endl; 
double smallest_er = k_errors[0]; //the smallest distnace will start as the first k_errors
double best_k = odd_k[0];
for (int i =1; i<k_errors.size(); i++) {//go through the k errors vector to find the k with the lowest ER, we dont need to check the first vector element since it is already set as the smallest
	if (smallest_er > k_errors[i]){
		smallest_er = k_errors[i];
		best_k = odd_k[i];
	}
}

//now the best k has been found, use it on the test data

 //Reading in the validation data 
  string validation_file;
  cout << "Enter the filename for your test data: ";
  cin >> validation_file;
  fin.close();
  fin.open(validation_file.data());
  while(!fin){
    cout << "File not found" << endl;
    cout << "Enter the filename for your test data: ";
    cin >> validation_file;
    fin.clear();
    fin.open(validation_file.data());
  }
  
vector<Instance> validation_data;
data_mode = false; 
s;

//Reads the file, gets the attributes and parses through the data. Shows an error if the file is empty or something is wrong
  while(getline(fin, s)){
	  if(s.size() > 0 && s[0] != '%'){
		if(data_mode){
			if(s[0] == '@'){
				cout<<"This is a command not data!"<<endl; 
			}
			else{
				Parse_Data(s, attribute_list, validation_data);
			}			
		}
	  else{
		  if(s.substr(0,5) == "@data"){
			  data_mode = true;
		  }
		  else if(s.
		  substr(0,10) == "@attribute"){
			// Attribute_List.push_back(New_Attribute(s)); 
		  }
		  else{
		  cout<<"I dont know what command this is! :/"<<endl; 
		  }
	  }
	  }
  }
  
	//Scale the test data
	for (int j =0; j< examples_vec.size(); j++){ //go through each of the data
		Scale(examples_vec[j], min_vec, max_vec, attribute_list);
	}

	int k = best_k; //initalize k to best k
	
	vector<int> knn_index_vec;
	knn_index_vec.resize(k);
	
	
	double error_rate = 0;
		for (int i=0; i<examples_vec.size(); i++){//loop through the test data
			if (i%50 == 0) {
				cout << "Currently on example " << i << " from the test data." << endl;
			}
			Find_Nearest_Neighbor(examples, examples_vec[i], attribute_list, knn_index_vec); //find the nearest neighbor
			
			vector<string> Class_Categories; //vector to hold the classification categories
			Class_Categories = attribute_list[attribute_list.size()-1].Get_Category_List();
			vector<int> Class_Totals(Class_Categories.size(), 0); //vector to hold the number of classification for each category from the knn's (size same as previous vector and fill with 0's)
			
			for (int j =0; j < knn_index_vec.size(); j++){ //go through all the found k nearest neighbors
			
				string knn_class = examples[knn_index_vec[j]].Get_Nominal_Value(attribute_list.size()-1); //find their class
				for (int p =0; p< Class_Categories.size(); p++){ //go through the categories vector
					if (Class_Categories[p] == knn_class) //if the current category we're looking at = the nn category
						Class_Totals[p] = Class_Totals[p] + 1; //add one to the same index(k) in the totals vector
				}
			}
			
			//after the totals vector has been filled, we need to find out which classification has the highest total
			int highest_total = -1;
			int highest_total_index = -1;
			for (int j=0; j< Class_Totals.size(); j++){ //go through the vector
				if (Class_Totals[j] > highest_total){
					highest_total = Class_Totals[j]; //save highest total for comparison
					highest_total_index = j; //save index
				}
					
			}
			
			//after looking through the whole vector: highest_total_index now holds the value of the index of the classification category that will be our guessed outcome for this current example
			
			string classification = Class_Categories[highest_total_index];//what is the found classification?
			string true_classification = examples_vec[i].Get_Nominal_Value(attribute_list.size()-1); //what is the true classification
			if (classification != true_classification){//if the classification does not match the true outcome
				error_rate = error_rate + 1; //add one to the error_rate
				cout << "Test Data " << i << " was misclassified. True class: " << true_classification << " Missclassified as: " << classification << " The Nearest Neighbors were: ";
				for (int j =0; j< knn_index_vec.size(); j++){
					cout << knn_index_vec[j] << " ";
				}
				cout << endl;
			}
		}
	
	cout << endl;
cout << "The Error Rate for the test data using the best K(" << best_k << ") is: " << error_rate/examples_vec.size() << endl;
						
			
  return 0;
}

    