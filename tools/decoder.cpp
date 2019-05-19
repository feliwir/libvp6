#include <cxxopts.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <vp6/decode.hpp>
#include "demuxer.hpp"

void load(const std::string& file)
{
	std::ifstream input(file);

	vp6::Demuxer demuxer(std::move(input));
	
}

int main(int argc, char** argv)
{
	cxxopts::Options options("vp6_decoder", "A decoder that converts .flv vp6 streams to a set of images");

	options.add_options()
		("f,file", "File name", cxxopts::value<std::string>());

	auto result = options.parse(argc, argv);

	if (result.count("file") > 0)
	{
		auto file = result["file"].as<std::string>();

		load(file);
	}
	else
	{
		options.help();
	}
}