#include "demuxer.hpp"
#include <cxxopts.hpp>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <vp6/decode.hpp>

bool load(const std::string &file)
{
    std::ifstream input(file);
    if(input.fail())
        return false;

    vp6::Demuxer demuxer(std::move(input));

    return true;
}

int main(int argc, char **argv)
{
    cxxopts::Options options("vp6_decoder", "A decoder that converts .flv vp6 streams to a set of images");

    options.add_options()("f,file", "File name", cxxopts::value<std::string>());

    auto result = options.parse(argc, argv);

    if (result.count("file") > 0)
    {
        auto file = result["file"].as<std::string>();

        if(!load(file))
        {
            std::cout << "Failed to load: " << file << std::endl;
            return EXIT_FAILURE;
        }
    }
    else
    {
        std::cout << options.help() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}