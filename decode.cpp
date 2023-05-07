#include <iostream>
#include <fstream>
#include <string>
#include <vector>

int main()
{
    std::string filename = "s.jpg";
    std::cout << "Enter filename in .jpg format: "<< std::endl;
    std::getline(std::cin, filename);
    // Open the input and output files
    std::ifstream infile(filename, std::ios::binary);

    // Read the first two bytes of the file, which should be the marker for a SOI (start of image) segment
    unsigned char soi[2];
    infile.read((char *)soi, 2);

    // Read the next segment, which should be the APP0 (application-specific) segment
    unsigned char app0header[7];
    infile.read((char *)app0header, 7);

    // Read the next segment, which should be the DQT (quantization table) segment
    unsigned char dqtheader[4];
    infile.read((char *)dqtheader, 4);

    // Read the next segment, which should be the SOF0 (baseline DCT) segment
    unsigned char sof0header[19];
    infile.read((char *)sof0header, 19);

    // Read the next segment, which should be the DHT (Huffman table) segment for luminance
    unsigned char dhtlumheader[4];
    infile.read((char *)dhtlumheader, 4);

    // Read the next segment, which should be the DHT (Huffman table) segment for chrominance
    unsigned char dhtchromheader[4];
    infile.read((char *)dhtchromheader, 4);

    // Read the next segment, which should be the SOS (start of scan) segment
    unsigned char sosheader[14];
    infile.read((char *)sosheader, 14);

    // Skip over any extra bytes in the SOS segment
    //infile.ignore(sosheader[12]);

    std::string message = "";

    char c;
    int index = 0;
    unsigned char last_char = 0x00;
        
    while (infile.get(c) )
    {
        //modify c
        if (index == 4)
        {
            if (+last_char == 3)
                break;
            message.push_back(last_char);
            index = 0;
            last_char = 0x00;
        }

        c = (c & 0x03) << (3-index)*2;
        last_char |= c;


        index++;
    } 

    std::cout << message << std::endl;

    infile.close();

    std::cout << "Finished!";
    std::cin;
    return 0;
}