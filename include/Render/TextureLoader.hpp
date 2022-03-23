//
// Created by Sam on 3/22/2022.
//

#ifndef ELYS_TEXTURE_LOADER_HPP
#define ELYS_TEXTURE_LOADER_HPP

#include <string>

using std::string;

namespace Elys {
    struct Texture {
        unsigned int id;
        string path;
    };

    unsigned int TextureFromFile(const char *path, const std::string &directory);
    Texture GenerateTexture(const char *path, const std::string &directory);
}

#endif // ELYS_TEXTURE_LOADER_HPP
