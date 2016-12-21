#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

#include <rapidxml.hpp>

using namespace rapidxml;

void compile(char* fnamei, char* fnameo) {

	std::ifstream infile(fnamei);
	std::vector<char> buffer((std::istreambuf_iterator<char>(infile)), std::istreambuf_iterator<char>());
	buffer.push_back('\0');

	xml_document<> doc;
	doc.parse<0>(&buffer[0]);

	xml_node<> *root_node = doc.first_node(0);
	std::cout << "Compiling map " << root_node->first_attribute("name")->value() << "...\n";

	std::string out;

	for (xml_node<> *node = root_node->first_node(0); node; node = node->next_sibling())
	{
		std::string rootName = node->name();
		if (rootName == "Entity")
		{
			// Convert to doubles and back again to avoid things that are not numbers creeping in!
			double posX = 0;
			double posY = 0;
			double posZ = 0;
			double rotX = 1;
			double rotY = 0;
			double rotZ = 0;
			double rotAng = 0;
			double scl = 0;

			for (xml_node<> *child = node->first_node(); child; child = child->next_sibling())
			{
				std::string name = child->name();
				std::string value = child->value();
				std::istringstream iss(value);
				if (name == "pos")
				{
					if (!(iss >> posX >> posY >> posZ))
					{
						std::cerr << "\n\nInvalid contents of 'pos' tag!\n\tExpected three numeric values,\n\tgot '" << value << "'.\n";
						break;
					}
				}
				else
				{
					std::cerr << "\n\nInvalid node name!\n\tExpected 'pos',\n\tgot '" << name << "'.\n";
					break;
				}
			}

			out += "E "
				+ std::to_string(posX) + " "
				+ std::to_string(posY) + " "
				+ std::to_string(posZ) + " "
				+ std::to_string(rotX) + " "
				+ std::to_string(rotY) + " "
				+ std::to_string(rotZ) + " "
				+ std::to_string(rotAng) + " "
				+ std::to_string(scl)
				+ "\ne\n";
		}
		else
		{
			std::cerr << "\n\nInvalid node name!\n\tExpected 'Entity',\n\tgot '" << rootName << "'.\n";
			break;
		}
	}

	std::ofstream outfile(fnameo);
	outfile << out;
	outfile.close();

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