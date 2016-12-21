#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <rapidxml.hpp>

using namespace rapidxml;

void compile(char* fname) {

	std::ifstream infile(fname);
	std::vector<char> buffer((std::istreambuf_iterator<char>(infile)), std::istreambuf_iterator<char>());
	buffer.push_back('\0');

	xml_document<> doc;
	doc.parse<0>(&buffer[0]);

	xml_node<> *root_node = doc.first_node(0);
	std::cout << "Compiling map " << root_node->first_attribute("name")->value() << "...\n";

	std::cout << "Compiled successfully." << std::endl;
}

int main(int argc, char** argv)
{
	if (argc != 2) // argc should be 2 for correct execution
	{
		std::cerr << "Usage: " << argv[0] << " <filename>\n";
	}
	else
	{
		compile(argv[1]);
	}

	std::cout << "Press enter to exit...";
	std::cin.ignore();
	return 0;
}