#include <iostream>
#include <fstream>
#include <string>
#include <vector>

int main(){

    std::string filename = "s.jpg";
    std::cout << "Enter filename in .jpg format: "<< std::endl;
    std::getline(std::cin, filename);

    //open image file in binary mode input only
    std::ifstream file(filename, std::ios::binary);

    //check if file exists
    if (!file) {
        std::cout << "Error opening file\n";
        return 1;
    }

    // Read the contents of the file into a vector of bytes
    std::vector<unsigned char> data((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    //close file read
    file.close();

    // Print the number of bytes read
    std::cout << "Read " << data.size() << " bytes\n";

    //find width height
    int height = 0;
    int width = 0;
    int stop_width_height = 0xFFC0;
    bool found = false;
    int current_bit = 0;
    for (int i = 0; i < data.size(); i++)
    {
        if( ((data[i] << 8) | data[i + 1]) == stop_width_height){
            if (found)
            {
                height = (data[i + 5] << 8) | data[i + 6];
                width = (data[i + 7] << 8) | data[i + 8];
                std::cout << "Read " << width << " bytes\n";
                std::cout << "Read " << height << " bytes\n";
                current_bit = i;
                break;
            }
            found = true;            
        }
    }

    int max_length = (width*height*3) / 8;
    std::cout << max_length << " max chars to be encoded\n";

    // Open the input and output files
    std::ifstream infile(filename, std::ios::binary);
    size_t dotIndex = filename.find(".");
    if (dotIndex != std::string::npos) {
        filename.replace(dotIndex, std::string::npos, "_encoded.jpg");
    }
    std::ofstream outfile(filename, std::ios::binary);

    // Read the first two bytes of the file, which should be the marker for a SOI (start of image) segment
    unsigned char soi[2];
    infile.read((char*)soi, 2);
    outfile.write((char*)soi, 2);

    // Read the next segment, which should be the APP0 (application-specific) segment
    unsigned char app0header[7];
    infile.read((char*)app0header, 7);
    outfile.write((char*)app0header, 7);

    // Read the next segment, which should be the DQT (quantization table) segment
    unsigned char dqtheader[4];
    infile.read((char*)dqtheader, 4);
    outfile.write((char*)dqtheader, 4);

    // Read the next segment, which should be the SOF0 (baseline DCT) segment
    unsigned char sof0header[19];
    infile.read((char*)sof0header, 19);
    outfile.write((char*)sof0header, 19);

    // Read the next segment, which should be the DHT (Huffman table) segment for luminance
    unsigned char dhtlumheader[4];
    infile.read((char*)dhtlumheader, 4);
    outfile.write((char*)dhtlumheader, 4);

    // Read the next segment, which should be the DHT (Huffman table) segment for chrominance
    unsigned char dhtchromheader[4];
    infile.read((char*)dhtchromheader, 4);
    outfile.write((char*)dhtchromheader, 4);

    // Read the next segment, which should be the SOS (start of scan) segment
    unsigned char sosheader[14];
    infile.read((char*)sosheader, 14);
    outfile.write((char*)sosheader, 14);

    // Skip over any extra bytes in the SOS segment
    infile.ignore(sosheader[12]);

    std::string message = "";
    std::cout << "Enter message to encode: " << std::endl;
    std::getline(std::cin, message);
    message.push_back(3);

    char c;
    int index = 3;
    int curr_char = 0;
    int temp = 0;
    while (infile.get(c))
    {
        //modify c
        if(curr_char < message.size()){
            c = c & 0b11111100;
            c |= (message[curr_char] >> index * 2) & 0x03;
            index--;
            if (index < 0)
            {
                index = 3;
                curr_char++;
            }
        }
        
        outfile.put(c);
    }

    infile.close();
    outfile.close();    

    std::cout << "Finished!";
    return 0;
}