#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

#include <rapidxml.hpp>

using namespace rapidxml;

bool parseMesh(xml_node<> *node, std::ofstream& outfile) {

	std::string objName;
	std::string matName;

	for (xml_attribute<> *attr = node->first_attribute(); attr; attr = attr->next_attribute())
	{
		std::string name = attr->name();
		std::string value = attr->value();
		std::istringstream iss(value);

		if (name == "name")
		{
			if (!(iss >> objName))
			{
				std::cerr << "\n\nInvalid contents of 'name' attribute!\n\tExpected one string,\n\tgot '" << value << "'.\n";
				return false;
			}
		}
		else if (name == "mat")
		{
			if (!(iss >> matName))
			{
				std::cerr << "\n\nInvalid contents of 'mat' attribute!\n\tExpected one string,\n\tgot '" << value << "'.\n";
				return false;
			}
		}
		else
		{
			std::cerr << "\n\nInvalid attribute of 'Mesh' tag!\n\tExpected 'pos', 'rot', 'scl',\n\tgot '" << name << "'.\n";
			return false;
		}
	}

	if (objName == "" || matName == "")
	{
		std::cerr << "\n\nMalformed attributes of 'Mesh' tag!\n\tExpected 'name' AND 'mat',\n\tgot ";
		for (xml_attribute<> *attr = node->first_attribute(); attr; attr = attr->next_attribute())
		{
			std::cerr << attr->name() << "=\"" << attr->value() << "\" ";
		}
		std::cerr << "\n";
		return false;
	}

	outfile << "MR " << objName << " " << matName << "\n";

	return true;
}

// Returns whether it was a successful parse.
bool parseEntity(xml_node<> *node, std::ofstream& outfile) {

	// Convert to doubles and back again to avoid things that are not numbers creeping in, and remove trailing zeros!
	double posX = 0;
	double posY = 0;
	double posZ = 0;
	double rotX = 1;
	double rotY = 0;
	double rotZ = 0;
	double rotAng = 0;
	double scl = 0;

	// Attributes
	for (xml_attribute<> *attr = node->first_attribute(); attr; attr = attr->next_attribute())
	{
		std::string name = attr->name();
		std::string value = attr->value();
		std::istringstream iss(value);

		if (name == "pos")
		{
			if (!(iss >> posX >> posY >> posZ))
			{
				std::cerr << "\n\nInvalid contents of 'pos' attribute!\n\tExpected three numeric values,\n\tgot '" << value << "'.\n";
				return false;
			}
		}
		else if (name == "rot")
		{
			if (!(iss >> rotX >> rotY >> rotZ))
			{
				std::cerr << "\n\nInvalid contents of 'rot' attribute!\n\tExpected four numeric values,\n\tgot '" << value << "'.\n";
				return false;
			}
		}
		else if (name == "scl")
		{
			if (!(iss >> scl))
			{
				std::cerr << "\n\nInvalid contents of 'scl' attribute!\n\tExpected one numeric value,\n\tgot '" << value << "'.\n";
				return false;
			}
		}
		else
		{
			std::cerr << "\n\nInvalid attribute of 'Entity' tag!\n\tExpected 'pos', 'rot', 'scl',\n\tgot '" << name << "'.\n";
			return false;
		}
	}

	outfile << "E "
		<< posX << " " << posY << " " << posZ << " "
		<< rotX << " " << rotY << " " << rotZ << " " << rotAng << " "
		<< scl << "\n";

	// Subnodes
	for (xml_node<> *child = node->first_node(); child; child = child->next_sibling())
	{
		std::string name = child->name();
		std::string value = child->value();
		std::istringstream iss(value);

		if (name == "Mesh")
		{
			if (!parseMesh(child, outfile)) return false;
		}
		else
		{
			std::cerr << "\n\nInvalid node name!\n\tExpected 'Mesh',\n\tgot '" << name << "'.\n";
			return false;
		}
	}

	outfile << "e\n";
	return true;
}

void compile(char* fnamei, char* fnameo) {

	std::ifstream infile(fnamei);
	std::vector<char> buffer((std::istreambuf_iterator<char>(infile)), std::istreambuf_iterator<char>());
	buffer.push_back('\0');

	xml_document<> doc;
	doc.parse<0>(&buffer[0]);

	xml_node<> *root_node = doc.first_node(0);
	std::cout << "Compiling map " << root_node->first_attribute("name")->value() << "...\n";

	std::ofstream outfile(fnameo);

	for (xml_node<> *node = root_node->first_node(0); node; node = node->next_sibling())
	{
		std::string rootName = node->name();
		if (rootName == "Entity")
		{
			if (!parseEntity(node, outfile)) return;
		}
		else
		{
			std::cerr << "\n\nInvalid node name!\n\tExpected 'Entity',\n\tgot '" << rootName << "'.\n";
			return;
		}
	}

	outfile.close();	// Closed automatically if function is ended prematurely, since this is on the stack

	std::cout << "Compiled successfully." << std::endl;
}

int main(int argc, char** argv)
{
	if (argc != 3) // argc should be 2 for correct execution
	{
		std::cerr << "Usage: " << argv[0] << " <filename>.xml <filename>.temap\n";
	}
	else
	{
		compile(argv[1], argv[2]);
	}

	std::cout << "\n\nPress enter to exit...";
	std::cin.ignore();
	return 0;
}