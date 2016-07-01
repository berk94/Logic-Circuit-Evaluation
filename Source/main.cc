//============================================================================
// Name        : CMPE_250_Project_2.cpp
// Author      : Kemal Berk Kocabagli
// Version     : 0.0.0
// Copyright   : © Bogazici University CMPE Department
// Description : Solving a Logic Circuit, Ansi-style
//============================================================================

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
using namespace std;
//represents each node in the logic circuit
struct element {
	int tvalue; // the truth value of the node
	int vertex; // the vertex of the node
	char identity; // type of the element (P (preposition),A,R,N,X)
	vector <element> inDegs; // the indegrees of this element
	vector <element> outDegs; // the outdegrees of this element
				 
	//constructor	
	element (int vert, int val, char id) {
		vertex = vert; 
		tvalue =val;
		identity = id;

	}


};

vector <element>nodes; // the list of every elements in the circuit
vector <int> no_InDeg; // the list of indegree number for each element

void printNodes(); //prints info about every node in the circuit, for debugging purposes
void solve(); //solves the logic circuit with the help of calculateTV method
bool isDAG(); //checks whether the system is a Directed Acyclic Graph. Returns true if yes.
int main(int argc, char *argv[]){
	std::string inFile = argv[1]; //input file to be read
	std::string outFile = argv[2]; //output file to be created

	int P=0; //number of prepositions
	int G=0; //number of gates
	int O=0; //number of outputs

	nodes.reserve(P+G+O);

	ifstream myFile (inFile.c_str());
	ofstream outPut;
	outPut.open(outFile.c_str()); //create output file

	//cout << "reading file..." << endl;
	int lineNo =1;
	while (myFile.good()) {
		string line;
		while (getline (myFile, line)) {

			stringstream ss;
			ss<<line;
			//cout << "line " <<lineNo <<": " << line << endl;
			if (lineNo==1) {
				//read the number of prepositions, operations and outputs
				//cout << "reading first line" << endl;
				ss >> P;
				ss >> G;
				ss >> O;
				for (int i=0 ;i <P+G+O; i++) {
					no_InDeg.push_back(0);
				}
				lineNo ++;
			}
			else if (lineNo ==2) {
				// read the prepositions
				//cout << "reading second line" << endl;
				for (int i=0; i<P; i++) {
					int tv;
					ss >> tv;
					element current(i,tv, 'P');
					nodes.push_back(current);
				}
				// read the gates and outputs
				for (int i=P; i<P+G+O; i++) {
					char id;
					ss >> id;
					element current (i,-1, id);
					nodes.push_back(current);
				}
				lineNo ++;
			}
			else {
				// read the connections between the nodes
				//cout << "reading line: " << lineNo << endl;
				int s;
				int d;
				ss >> s;
				while (ss>>d) {
					nodes [d].inDegs.push_back (nodes[s]);
					no_InDeg[d]++;
					nodes [s].outDegs.push_back (nodes[d]);
				}
				lineNo++;
			}
			//cout << "line no: " << lineNo << endl;
		}
	}

	/*cout << "line no: " << lineNo << endl;
	cout << "P: " << P <<endl;
	cout << "G: " << G <<endl;
	cout << "O: " << O <<endl;

	printNodes();*/

	//solve only if the graph is a DAG

	if (isDAG()) {
		solve();
		//cout << nodes[0].tvalue;
		outPut << nodes[0].tvalue;
		for (int i=1; i<nodes.size(); i++) {
			//cout << " " <<nodes[i].tvalue;
			outPut << " " <<nodes[i].tvalue;
		}
		outPut << endl;
	}
	else {
		//cout << "-1" << endl;
		outPut << "-1" << endl;
	}
	return 0;
}

void printNodes () {
	for (int i=0; i<nodes.size() ; i++){
		cout << "node " <<i<< ": "<<" /// ";
		cout << "vertex: " <<nodes[i].vertex <<"/// ";
		cout << "truth value: " <<nodes[i].tvalue <<" /// ";
		cout << "id: " <<nodes[i].identity <<" /// ";
		cout << "Indegs: ";
		for (int j=0; j<nodes[i].inDegs.size(); j++){
			cout << nodes [i].inDegs [j].vertex <<" ";
		}
		cout << endl;
	}


}

int calculateTV (element& e);
void solve() {
	for (int i=nodes.size()-1; i>=0; i--) {
		nodes[i].tvalue = calculateTV(nodes[i]);
	}
	//cout <<"system solved"<< endl;

}


int AND (vector <element>& list);
int OR (vector <element>& list);
int XOR (element& e1, element& e2);
int NOT (element& e1);
int calculateTV(element& e) { //calculates the truth value of a given element in this circuit. Uses recursion. 
	if (e.identity =='P') {
		return e.tvalue;
	}
	else if (e.identity == 'A') {
		for (int i=0; i<e.inDegs.size(); i++) {
			if (e.inDegs[i].tvalue==-1){
				e.inDegs[i].tvalue = calculateTV(e.inDegs[i]);
			}
		}
		return AND (e.inDegs);
	}
	else if (e.identity == 'R') {
		for (int i=0; i<e.inDegs.size(); i++) {
			if (e.inDegs[i].tvalue==-1){
				e.inDegs[i].tvalue = calculateTV(e.inDegs[i]);
			}
		}
		return OR (e.inDegs);
	}
	else if (e.identity == 'X') {
		if (e.inDegs[0].tvalue==-1){
			e.inDegs[0].tvalue = calculateTV(e.inDegs[0]);}
		if (e.inDegs[1].tvalue==-1){
			e.inDegs[1].tvalue = calculateTV(e.inDegs[1]);
		}
		return XOR (e.inDegs[1], e.inDegs[0]);
	}
	else if (e.identity == 'N') {
		if (e.inDegs[0].tvalue==-1){
			e.inDegs[0].tvalue = calculateTV(e.inDegs[0]);}


		return NOT (e.inDegs[0]);
	}
	else {
		return calculateTV(e.inDegs[0]);
	}
	return 0;
}


int AND (vector<element>& list) { //and operator
	for (int i=0; i<list.size(); i++) {
		if(list[i].tvalue==0) return 0;
	}
	return 1;
}

int OR (vector<element>& list) { // or operator
	for (int i=0; i<list.size(); i++) {
		if(list[i].tvalue==1) return 1;
	}
	return 0;
}

int XOR (element& p, element& q) { // xor operator
	if ((p.tvalue && !q.tvalue) or (!p.tvalue && q.tvalue)) return 1;
	return 0;

}

int NOT (element& p) { //not operator
	if (!p.tvalue) return 1;
	return 0;
}

bool isDAG () {
	vector <bool> isVisited;
	for (int i=0;i<no_InDeg.size(); i++) {
		isVisited.push_back(false);
	}
	for (int i=0; i<no_InDeg.size(); i++) {
		if (no_InDeg[i] ==0 and !isVisited[i]) {
			isVisited[i] =true;
			for (int j=0; j<nodes[i].outDegs.size(); j++)
			{
				no_InDeg[nodes[i].outDegs[j].vertex]--;
			}
		}
	}
	bool allZero = true;
	for (int i=0; i<no_InDeg.size(); i++) {
		if(no_InDeg[i]!=0) allZero=false;
	}
	if (allZero) return true;

	return false;
}

