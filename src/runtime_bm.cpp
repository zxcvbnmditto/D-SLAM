#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <sstream>
#include <numeric>

std::vector<float> enc_time, dec_time, track_time, iter_time;

float find_average(std::vector<float> v)
{
    return accumulate( v.begin(), v.end(), 0.0)/v.size(); 
}

std::string split(const std::string &str, char ch)
{
    size_t pos = str.find(ch);
    size_t initialPos = 0;

    while (pos != std::string::npos)
    {
        initialPos = pos + 1;
        pos = str.find(ch, initialPos);
    }

    return str.substr(initialPos, std::min(pos, str.size()) - initialPos + 1 );
}

void read_file(const std::string& filename)
{
    std::string line;
    std::ifstream in_file(filename);

    if (!in_file.is_open()) 
    {
        std::cerr << "Cannot open input file: " << filename << std::endl;
        exit(EXIT_FAILURE);
    }

    while (std::getline(in_file, line))
    {
        if (!line.find("Encoder Time"))
            enc_time.push_back(std::stof(split(line, ' ')));
        else if (!line.find("Decoder Time"))
            dec_time.push_back(std::stof(split(line, ' ')));
        else if (!line.find("Track Time"))
            track_time.push_back(std::stof(split(line, ' ')));
        else if (!line.find("Iter Time"))
            iter_time.push_back(std::stof(split(line, ' ')));
        else 
            continue;
    }

}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cerr << "usage: ./runtime_bm <path-to-time-data>\n";
        return EXIT_FAILURE;
    }

    read_file(argv[1]);
    std::string sequence = split(argv[1], '/');
    std::cout << sequence.substr(0,2) << "\t\t" << find_average(enc_time) << "\t\t" << find_average(dec_time) << "\t\t" << find_average(track_time) << "\t\t" << find_average(iter_time) << std::endl;
}