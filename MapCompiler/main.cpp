#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

#include <rapidxml.hpp>

using namespace rapidxml;

std::string ROOT_DIR = "";

bool file_exists(const std::string& name) {
	std::ifstream f(name.c_str());
	return f.good();
}

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
			if (!file_exists(ROOT_DIR + "models/" + objName))
			{
				std::cout << "WARNING: File " << objName << " does not currently exist!\n";
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

bool parseLightDirectional(xml_node<> *node, std::ofstream& outfile) {

	double colR = 1;
	double colG = 1;
	double colB = 1;
	double intensity = 0.4;
	double shadowMapSize = 10;
	double shadowArea = 80;
	double shadowSoft = 1;

	for (xml_attribute<> *attr = node->first_attribute(); attr; attr = attr->next_attribute())
	{
		std::string name = attr->name();
		std::string value = attr->value();
		std::istringstream iss(value);

		// Ambiguity of spelling to keep American users happy.
		if (name == "colour" || name == "color")
		{
			if (!(iss >> colR >> colG >> colB))
			{
				std::cerr << "\n\nInvalid contents of 'colour'/'color' attribute!\n\tExpected three numeric values,\n\tgot '" << value << "'.\n";
				return false;
			}
		}
		else if (name == "intensity")
		{
			if (!(iss >> intensity))
			{
				std::cerr << "\n\nInvalid contents of 'intensity' attribute!\n\tExpected one numeric value,\n\tgot '" << value << "'.\n";
				return false;
			}
		}
		else if (name == "shadowMapSize")
		{
			if (!(iss >> shadowMapSize))
			{
				std::cerr << "\n\nInvalid contents of 'shadowMapSize' attribute!\n\tExpected one numeric value,\n\tgot '" << value << "'.\n";
				return false;
			}
		}
		else if (name == "shadowArea")
		{
			if (!(iss >> shadowArea))
			{
				std::cerr << "\n\nInvalid contents of 'shadowArea' attribute!\n\tExpected one numeric value,\n\tgot '" << value << "'.\n";
				return false;
			}
		}
		else if (name == "shadowSoft")
		{
			if (!(iss >> shadowSoft))
			{
				std::cerr << "\n\nInvalid contents of 'shadowSoft' attribute!\n\tExpected one numeric value,\n\tgot '" << value << "'.\n";
				return false;
			}
		}
		else
		{
			std::cerr << "\n\nInvalid attribute of 'DirectionalLight' tag!\n\tExpected 'colour'/'color', 'intensity', 'shadowMapSize', 'shadowArea', 'shadowSoft',\n\tgot '" << name << "'.\n";
			return false;
		}
	}

	outfile << "LD " << colR << " " << colG << " " << colB << " "
		<< intensity << " "
		<< shadowMapSize << " " << shadowArea << " " << shadowSoft << "\n";

	return true;
}

bool parseFreeCamera(xml_node<> *node, std::ofstream& outfile) {

	double fov = 70;
	double near = 0.1;
	double far = 1000;

	for (xml_attribute<> *attr = node->first_attribute(); attr; attr = attr->next_attribute())
	{
		std::string name = attr->name();
		std::string value = attr->value();
		std::istringstream iss(value);

		if (name == "fov")
		{
			if (!(iss >> fov))
			{
				std::cerr << "\n\nInvalid contents of 'fov' attribute!\n\tExpected one numeric value,\n\tgot '" << value << "'.\n";
				return false;
			}
		}
		else if (name == "near")
		{
			if (!(iss >> near))
			{
				std::cerr << "\n\nInvalid contents of 'near' attribute!\n\tExpected one numeric value,\n\tgot '" << value << "'.\n";
				return false;
			}
		}
		else if (name == "far")
		{
			if (!(iss >> far))
			{
				std::cerr << "\n\nInvalid contents of 'far' attribute!\n\tExpected one numeric value,\n\tgot '" << value << "'.\n";
				return false;
			}
		}
		else
		{
			std::cerr << "\n\nInvalid attribute of 'DirectionalLight' tag!\n\tExpected 'fov', 'near', 'far',\n\tgot '" << name << "'.\n";
			return false;
		}
	}

	outfile << "C " << fov << " " << near << " " << far << "\nFL\nFM\n";

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
	double scl = 1;

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
			if (!(iss >> rotX >> rotY >> rotZ >> rotAng))
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
		else if (name == "DirectionalLight")
		{
			if (!parseLightDirectional(child, outfile)) return false;
		}
		else if (name == "FreeCamera")
		{
			if (!parseFreeCamera(child, outfile)) return false;
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

// Returns whether it was a successful parse.
bool parseMaterial(xml_node<> *node, std::ofstream& outfile) {

	std::string matName;
	std::string diffuse = "defaultTexture.png";
	std::string normal = "default_normal.jpg";
	std::string displacement = "default_disp.png";
	double specular = 0;
	double specularPower = 1;
	double displacementScale = 0;
	double displacementOffset = 0;

	// Attributes
	for (xml_attribute<> *attr = node->first_attribute(); attr; attr = attr->next_attribute())
	{
		std::string name = attr->name();
		std::string value = attr->value();
		std::istringstream iss(value);

		if (name == "name")
		{
			if (!(iss >> matName))
			{
				std::cerr << "\n\nInvalid contents of 'name' attribute!\n\tExpected one string value,\n\tgot '" << value << "'.\n";
				return false;
			}
		}
		else if (name == "diffuse")
		{
			if (!(iss >> diffuse))
			{
				std::cerr << "\n\nInvalid contents of 'diffuse' attribute!\n\tExpected one string value,\n\tgot '" << value << "'.\n";
				return false;
			}
		}
		else if (name == "normal")
		{
			if (!(iss >> normal))
			{
				std::cerr << "\n\nInvalid contents of 'normal' attribute!\n\tExpected one string value,\n\tgot '" << value << "'.\n";
				return false;
			}
		}
		else if (name == "displacement")
		{
			if (!(iss >> displacement))
			{
				std::cerr << "\n\nInvalid contents of 'displacement' attribute!\n\tExpected one string value,\n\tgot '" << value << "'.\n";
				return false;
			}
		}
		else if (name == "specular")
		{
			if (!(iss >> specular))
			{
				std::cerr << "\n\nInvalid contents of 'specular' attribute!\n\tExpected one numeric value,\n\tgot '" << value << "'.\n";
				return false;
			}
		}
		else if (name == "specularPower")
		{
			if (!(iss >> specularPower))
			{
				std::cerr << "\n\nInvalid contents of 'specularPower' attribute!\n\tExpected one numeric value,\n\tgot '" << value << "'.\n";
				return false;
			}
		}
		else if (name == "displacementScale")
		{
			if (!(iss >> displacementScale))
			{
				std::cerr << "\n\nInvalid contents of 'displacementScale' attribute!\n\tExpected one numeric value,\n\tgot '" << value << "'.\n";
				return false;
			}
		}
		else if (name == "displacementOffset")
		{
			if (!(iss >> displacementOffset))
			{
				std::cerr << "\n\nInvalid contents of 'displacementOffset' attribute!\n\tExpected one numeric value,\n\tgot '" << value << "'.\n";
				return false;
			}
		}
		else
		{
			std::cerr << "\n\nInvalid attribute of 'Material' tag!\n\tExpected 'name', 'diffuse', 'normal', 'displacement', 'specular', 'specularPower', 'displacementScale', 'displacementOffset',\n\tgot '" << name << "'.\n";
			return false;
		}
	}

	if (matName == "")
	{
		std::cerr << "\n\nMaterial must have a 'name' attribute!\n";
		return false;
	}

	outfile << "M " << matName << " "
		<< diffuse << " " << normal << " " << displacement << " "
		<< specular << " " << specularPower << " "
		<< displacementScale << " " << displacementOffset << "\n";

	return true;
}

void compile(std::string fnamei, std::string fnameo) {
	
	std::ifstream infile(fnamei);
	if (infile.bad())
	{
		return;
	}
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
		else if (rootName == "Material")
		{
			if (!parseMaterial(node, outfile)) return;
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
		std::cerr << "Usage: " << argv[0] << " <directory containing res/ folder> <filename (no extension)>\n";
	}
	else
	{
		ROOT_DIR = std::string(argv[1]) + "/res/";
		compile(ROOT_DIR + "maps/" +  std::string(argv[2]) + ".xml", ROOT_DIR + "maps/" + std::string(argv[2]) + ".temap");
	}

	std::cout << "\n\nPress enter to exit...";
	std::cin.ignore();
	return 0;
}