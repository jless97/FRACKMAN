#include "GameController.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ctime>
using namespace std;

  // If your program is having trouble finding the Assets directory,
  // replace the string literal with a full path name to the directory,
  // e.g., "Z:/CS32/BoulderBlast/Assets" or "/Users/fred/cs32/BoulderBlast/Assets"

// Note: The path of the assets directory needs to be specified to access images and sound effects
const string asset_directory = "/Users/JasonLess/Desktop/FRACKMAN/frack_project/DerivedData/FrackMan/Build/Products/Debug/Assets";

class GameWorld;

GameWorld* createStudentWorld(string asset_dir = "");

int main(int argc, char* argv[])
{
	{
		string path = asset_directory;
		if (!path.empty())
			path += '/';
		const string someAsset = "frack1.tga";
		ifstream ifs(path + someAsset);
		if (!ifs)
		{
			cout << "Cannot find " << someAsset << " in ";
			cout << (asset_directory.empty() ? "current directory"
											: asset_directory) << endl;
			return 1;
		}
	}

	srand(static_cast<unsigned int>(time(nullptr)));

	GameWorld* gw = createStudentWorld(asset_directory);
	Game().run(argc, argv, gw, "FrackMan");
}
