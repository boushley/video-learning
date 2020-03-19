//
// Created on 3/17/20.
//

#include <iostream>
#include <fstream>
#include <filesystem>
#include "MP2TSFile.h"

using std::cout;
using std::endl;

int main (int argc, char** argv) {
    if (argc < 2) {
        cout << "Missing arguments! Only received " << argc << " arguments." << endl;
        exit(9);
    }

    std::string assetPath(std::filesystem::canonical(argv[1]));
    MP2TSFile file(std::move(assetPath));

    if (file.isOpen()) {
        auto size = file.size();
        cout << "Opened: " << file.getPath() << " the current position is: " << size << " so we should expect: " << (size/188) << " packets with " << (size%188) << " remainder" << endl;

        file.parsePacket();
    } else {
        cout << "Failed to open the file: " << endl;
        exit(1);
    }
}