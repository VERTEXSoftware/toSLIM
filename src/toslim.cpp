

const char* PROGRAM_VERSION = "1.2.0.0";
const char* PROGRAM_AUTHOR = "Sleptsov Vladimir";
const char* PROGRAM_DESCRIPTION = "miniSLIM";
const char* BUILD_DATE = __DATE__;
const char* BUILD_TIME = __TIME__;

#include <iostream>
#include <string>
#include <filesystem>

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif

#ifndef STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#endif

#define SLEP_SLIM_IMP

#include "SLIM/SLIM.h"
#include "support/support.h"
#include "../external/stb/stb_image.h"
#include "../external/stb/stb_image_write.h"


bool loadotherformat(const std::string& input, unsigned char* &data, int &w, int &h,  SLIM_CODE &channels){
    int chan=0;
    data = stbi_load(input.c_str(), &w, &h, &chan, 0);
    channels=ChannelToCode(chan);

    return data!=NULL;
}

bool load_image(const std::string& input, unsigned char* &data, int &w, int &h,  SLIM_CODE &channels) {
    ImageFormat fmt = detect_format(input);


    switch (fmt) {
        case ImageFormat::fPNG:
            return loadotherformat(input.c_str(), data, w, h, channels);
        case ImageFormat::fJPG:
            return loadotherformat(input.c_str(), data, w, h, channels);
        case ImageFormat::fBMP:
            return loadotherformat(input.c_str(), data, w, h, channels);
        case ImageFormat::fTGA:
            return loadotherformat(input.c_str(), data, w, h, channels);
        case ImageFormat::fSLIM:
            {
                SLIM_STREAM* infile = SLIM_STREAM_OPEN(input.c_str(), SLIM_STREAM_MODE::STREAM_MODE_READ);

                if(SLIM_STREAM_ISOPEN(infile)) {

                    SLIM_HEADER_DESC  header{};
                    SLIM_LAYER_DESC   layer{};

                    SLIM_Read_Header(infile, &header);
                    SLIM_Read_Layer(infile, &layer);
                    
                    w           = layer.width;
                    h           = layer.height;
                    channels    = (SLIM_CODE)layer.code;
                    data        = (unsigned char*)layer.img;


                    SLIM_Free(layer.name);
                    SLIM_Free(layer.ext);

                    SLIM_STREAM_CLOSE(infile);
                }else{
                    return false;
                }
                return true;
            }
        default:
            std::cerr << "Unknown type of format\n";
            return false;
    }
}


static void showHelp() {
    std::cout << "SLEP IMAGE CONVERTER\n";
    std::cout << "Copyright (C) 2026 VERTEX Software by Sleptsov Vladimir\n";
    std::cout << "Version: "<<PROGRAM_VERSION<<"\n";
    std::cout << "Build date: "<<BUILD_DATE<<" "<<BUILD_TIME<<"\n";
    std::cout << "Usage:\n";
    std::cout << "  toslim [options] <image_path_a> <image_path_b>\n";
    std::cout << "\nOptions:\n";
    std::cout << "  -i          Get information about an image\n";
    std::cout << "  -c          Convert from format file to other format\n";
    std::cout << "  -q          Image quality level for JPEG and SLIM (0..255)\n";
    std::cout << "  -v          Display image (default behavior)\n";
    std::cout << "  -m          Display map image (only SLIM is supported)\n";
    std::cout << "  -a          Comparison of images using PSNR/SSIM/PSQNR\n";
    std::cout << "  -w          Exporting a map from SLIM\n";
    std::cout << "  -h          Show this help message\n";
    std::cout << "  -y          Overwrite file\n";
    std::cout << "\nExamples:\n";
    std::cout << "  toslim image.png                               Display image\n";
    std::cout << "  toslim -v image.SLIM                           Display image\n";
    std::cout << "  toslim -m image.SLIM                           Display map image\n";
    std::cout << "  toslim -c image.png image.SLIM                 Convert image.png to image.SLIM\n";
    std::cout << "  toslim -c image.SLIM image.png                 Convert image.SLIM to image.png\n";
    std::cout << "  toslim -c -q 128 image.png image.SLIM          Convert image.png to image.SLIM quality 50%\n";
    std::cout << "  toslim -a image.png image.SLIM                 Comparing image.png with image.SLIM\n";
    std::cout << "  toslim -w image.SLIM map.png                   Exporting a map from image.SLIM to map.png\n";
    std::cout << "  toslim -i image.SLIM                           Information about the image.SLIM file\n";
    std::cout << "\nDefault:\n";
    std::cout << "  Quality: 255 (MAX)\n";
}


#ifdef ONLY_TERMINAL
#include "support/image_console.h"
#else
#include "support/image_viewer.h"
#endif

enum class Mode {
    NONE,
    VIEW,
    CONVERT,
    ANALIZE,
    INFO,
    VIEWMAP,
    SAVEMAP
};


void DemoIMG(std::string file){

    unsigned char* data = NULL;
    int w = 0;
    int h = 0;
    SLIM_CODE channels;


    if(load_image(file, data, w, h, channels)){

        #ifdef ONLY_TERMINAL
        ImageConsoleViewer(data, w, h, channels);
        #else
        ImageViewer(file, data, w, h, channels);
        #endif
    }

    SLIM_Free(data);
}


void DemoMapSLIMIMG(std::string file){

    unsigned char* data = NULL;
    int w = 0;
    int h = 0;

    SLIM_STREAM* infile = SLIM_STREAM_OPEN(file.c_str(), SLIM_STREAM_MODE::STREAM_MODE_READ);

    if(SLIM_STREAM_ISOPEN(infile)) {
        SLIM_HEADER_DESC  header{};
        SLIM_LAYER_DESC   layer{};

        layer.forced_code = SLIM_CODE::CODE_GRAY;
        SLIM_Read_Header(infile, &header);
        SLIM_Read_Layer_Map(infile, &layer);

        w           = layer.width;
        h           = layer.height;
        data        = (unsigned char*)layer.img;

        SLIM_Free(layer.name);
        SLIM_Free(layer.ext);

        SLIM_STREAM_CLOSE(infile);

        unsigned char* dataimg = (unsigned char*)SLIM_MALLOC(w * h * 3);
        grayToInferno(data, dataimg, w, h);

        #ifdef ONLY_TERMINAL
        ImageConsoleViewer(dataimg, w, h, SLIMCODE::CODE_RGB);
        #else
        ImageViewer(file, dataimg, w, h, SLIM_CODE::CODE_RGB);
        #endif

        SLIM_Free(dataimg);
        SLIM_Free(data);
    }    
}


void InfoOtherFormat(std::string imagePath){
    int width = 0;
    int height = 0;
    int channels = 0;

    unsigned char* data = stbi_load(imagePath.c_str(), &width, &height, &channels, 0);

    if (data==NULL) {
        std::cerr << "Failed to open image: " << imagePath << "\n";
    }
    std::cout << "----[ INFORMATION ]----\n";
    std::cout << "FORMAT: Image ("<<getFileExtension(imagePath)<<")\n";
    std::cout << "WIDTH: " << width << "\n";
    std::cout << "HEIGHT: " << height << "\n";

    std::cout << "CODE: ";
    switch (channels) {
        case 1: std::cout << "GRAY (1 channel)\n"; break;
        case 2: std::cout << "GRAY+ALPHA (2 channels)\n"; break;
        case 3: std::cout << "RGB (3 channels)\n"; break;
        case 4: std::cout << "RGBA (4 channels)\n"; break;
        default:  std::cout<<"NONE (not defined)\n"; break;
    }

    size_t sizefile=0;    

    SLIM_STREAM* infile = SLIM_STREAM_OPEN(imagePath.c_str(), SLIM_STREAM_MODE::STREAM_MODE_READ);

    if(SLIM_STREAM_ISOPEN(infile)) {
        SLIM_STREAM_SIZE(infile);
        SLIM_STREAM_CLOSE(infile);
    }

    size_t sizefileraw = width * height * channels;

    std::cout << compressionRatio(sizefileraw, sizefile) << "\n";
    std::cout << "SIZE COMP: " << sizefile << " (" << formatSize(sizefile) << ")\n";
    std::cout << "SIZE RAW: " << sizefileraw << " (" << formatSize(sizefileraw) << ")\n";

    stbi_image_free(data);
}




double Percent(uint32_t value, uint32_t total)
{
    if (total == 0){
        return 0.0;
    }

    return double(value) * 100.0 /double(total);
}

void InfoIMG(std::string imagePath){

    ImageFormat fmt = detect_format(imagePath);

    switch (fmt) {
        case ImageFormat::fPNG:
            InfoOtherFormat(imagePath);
            break;
        case ImageFormat::fJPG:
            InfoOtherFormat(imagePath);
            break;
        case ImageFormat::fBMP:
            InfoOtherFormat(imagePath);
            break;
        case ImageFormat::fTGA:
            InfoOtherFormat(imagePath);
            break;
        case ImageFormat::fSLIM:
            {
                SLIM_STREAM* infile = SLIM_STREAM_OPEN(imagePath.c_str(), SLIM_STREAM_MODE::STREAM_MODE_READ);

                if(SLIM_STREAM_ISOPEN(infile)) {
  
                    SLIM_HEADER_DESC        header{};
                    SLIM_LAYER_INFO_DESC    layer{};

                    SLIM_Read_Header(infile, &header);
                    SLIM_Read_Layer_Info(infile, &layer);

                    std::cout << "----[ INFORMATION ]----\n";

                    std::cout<<"FORMAT: Sleptsov Vladimir Image (SLIM)\n";

                    uint8_t major = (header.version>> 24) & 0xFF;
                    uint8_t minor = (header.version>> 16) & 0xFF;
                    uint8_t patch = (header.version >> 8) & 0xFF;
                    uint8_t build = header.version & 0xFF;

                    std::cout<<"VERSION: "<<(int)major <<"."<<(int)minor<<"."<<(int)patch<<"."<<(int)build<< "\n";
                    
                    std::cout<<"NAME: "<<(layer.name==NULL?"[NULL]":layer.name)<< "\n";
                    std::cout<<"ID: "<<layer.id<< "\n";
                    std::cout<<"WIDTH: "<<layer.width<< "\n";
                    std::cout<<"HEIGHT: "<<layer.height<< "\n";
                    std::cout<<"CODE: ";

                    uint32_t chanells =1;

                    switch (layer.code)
                    {
                        case SLIM_CODE::CODE_GRAY:
                            std::cout<<"Gray (1 channels)\n";
                            chanells =1;
                            break;
                        case SLIM_CODE::CODE_RGB:
                            std::cout<<"RGB (3 channels)\n";
                            chanells =3;
                            break;
                        case SLIM_CODE::CODE_BGR:
                            std::cout<<"BGR (3 channels)\n";
                            chanells =3;
                            break;
                        case SLIM_CODE::CODE_RGBA:
                            std::cout<<"RGBA (4 channels)\n";
                            chanells =4;
                            break;
                        case SLIM_CODE::CODE_BGRA:
                            std::cout<<"BGRA (4 channels)\n";
                            chanells =4;
                            break;
                        default:
                            std::cout<<"NONE (not defined)\n";
                    }
                    uint32_t sizefile=SLIM_STREAM_SIZE(infile);
                    uint32_t sizefileraw=layer.width * layer.height * chanells;


                    std::cout<<compressionRatio(sizefileraw,sizefile)<<"\n";
                    std::cout<<"SIZE COMP: "<<sizefile<<" ("<<formatSize(sizefile)<<")\n";
                    std::cout<<"SIZE RAW: "<<sizefileraw<<" ("<<formatSize(sizefileraw)<<")\n";

                    const auto totalpix = layer.block_256_all;

                    std::cout << "\n----[ BLOCKS " << totalpix << " ]----\n";
                    std::cout << "COLOR MIN: "<< layer.block_color_table_min<< "\n";
                    std::cout << "COLOR MAX: "<< layer.block_color_table_max<< "\n";
                    std::cout << "COLOR AVG: "<< layer.block_color_table_avg<< "\n";
                    std::cout << "\n----[ BLOCKS " << totalpix << " ]----\n";
                    std::cout << "DELTA MIN: "<< layer.block_q_min<< "\n";
                    std::cout << "DELTA MAX: "<< layer.block_q_max<< "\n";
                    std::cout << "DELTA AVG: "<< layer.block_q_avg<< "\n";

                    const uint32_t total = layer.all_c;

                    std::cout << "\n----[ LINES " << total << " ]----\n";
                    std::cout << "REUSE: "<< layer.reuse_c<< " (" << Percent(layer.reuse_c, total) << "%)\n";
                    std::cout << "ORIGINAL: "<< layer.origin_c<< " (" << Percent(layer.origin_c, total) << "%)\n";
                    std::cout << "RLE: "<< layer.rle_c<< " (" << Percent(layer.rle_c, total) << "%)\n";
                    std::cout << "RICE: "<< layer.rice_c<< " (" << Percent(layer.rice_c, total) << "%)\n";
                    std::cout << "SLDD: "<< layer.sldd_c<< " (" << Percent(layer.sldd_c, total) << "%)\n";
                    std::cout << "MASKARED: "<< layer.maskared_c<< " (" << Percent(layer.maskared_c, total) << "%)\n";

                    SLIM_Free(layer.name);
                    SLIM_Free(layer.ext);

                    SLIM_STREAM_CLOSE(infile);
                }
            }
            break;
        default:
            std::cerr << "Unknown type of format\n";
            break;
    }
}




bool save_image(const std::string& output, unsigned char* data, int w, int h,  SLIM_CODE chan, uint8_t quality) {

    ImageFormat fmt = detect_format(output);

    int channels = CodeToChannel(chan);
    switch (fmt) {
        case ImageFormat::fPNG:
            return stbi_write_png(output.c_str(), w, h, channels, data, w * channels);
        case ImageFormat::fJPG:
            return stbi_write_jpg(output.c_str(), w, h, channels, data, int((quality* 100) / 255));
        case ImageFormat::fBMP:
            return stbi_write_bmp(output.c_str(), w, h, channels, data);
        case ImageFormat::fTGA:
            return stbi_write_tga(output.c_str(), w, h, channels, data);
        case ImageFormat::fSLIM:
            {
                SLIM_STREAM* infile = SLIM_STREAM_OPEN(output.c_str(), SLIM_STREAM_MODE::STREAM_MODE_WRITE);

                if(SLIM_STREAM_ISOPEN(infile)) {

                    SLIM_HEADER_DESC        header{};
                    header.layers           = 0x1u;
                    header.canvas_width     =(uint16_t)w;
                    header.canvas_height    =(uint16_t)h;
                    header.canvas_channel   =(uint8_t )chan;

                    SLIM_LAYER_DESC         layer{};
                    layer.width             = (uint16_t)w;
                    layer.height            = (uint16_t)h;
                    layer.code              = (uint8_t )chan;
                    layer.gen_mipmap        = (uint8_t )SLIM_MIPMAP_MODE::MM_GENERATE;
                    layer.quality           = quality;
                    layer.img               = data;
                                         
                    SLIM_Write_Header(infile, &header);
                    SLIM_Write_Layer(infile, &layer);

                    SLIM_STREAM_CLOSE(infile);
                }
                return true;
            }
        default:
            std::cerr << "Unknown type of format\n";
            return false;
    }
}



void AnalizeIMG(std::vector<std::string> files){

    std::string orig = files[0];

    int w1=0;
    int h1=0;
    SLIM_CODE c1;

    int w2=0;
    int h2=0;
    SLIM_CODE c2;

    unsigned char* img1=NULL;
    unsigned char* img2=NULL;

    if(load_image(orig, img1, w1, h1, c1)==false){
        return;
    }

    std::cout << "Original: " << orig << "\n";

    for(size_t i=1;i<files.size();++i){

        std::string file = files[i];

        if(load_image(file, img2, w2, h2, c2)==false){continue;}

        std::cout << "\nFile: " << file << "\n";

        if(w1==w2 && h1==h2 && c1 == c2 && img1!=NULL && img2!=NULL){

        double psnr = calcPSNR(img1, img2, w1, h1, c1);
        double psqnr = calcPSQNR(img1, img2, w1, h1, c1);
        double ssim = calcSSIM(img1, img2, w1, h1, c1);
        
        
        std::cout << "PSNR: " << psnr << " dB\n";
        std::cout << "PSQNR: " << psqnr << " dB\n";


        char buf[128];
        std::snprintf(buf, sizeof(buf), "SSIM: %f (%.2f%%)", ssim, ssim* 100.0 );
        std::cout << buf << "\n";

        }else{
            std::cout << "The image sizes do not match\n";
        }

        SLIM_Free(img2);
    }

    SLIM_Free(img1);
}




void ConvertIMG(std::string fileA,std::string fileB, uint8_t quality){

    unsigned char* data = NULL;
    int w = 0;
    int h = 0;
    SLIM_CODE channels;


    if(load_image(fileA, data, w, h, channels)){
        
        save_image(fileB, data, w, h,channels,quality);
    }

    SLIM_Free(data);

}

void SaveMapToIMG(std::string fileA,std::string fileB){

    unsigned char* data = NULL;
    int w = 0;
    int h = 0;
    SLIM_CODE channels;
    
    SLIM_STREAM* infile = SLIM_STREAM_OPEN(fileA.c_str(), SLIM_STREAM_MODE::STREAM_MODE_READ);

    if(SLIM_STREAM_ISOPEN(infile)) {
        SLIM_HEADER_DESC  header{};
        SLIM_LAYER_DESC   layer{};

        layer.forced_code = SLIM_CODE::CODE_GRAY;
        SLIM_Read_Header(infile, &header);
        SLIM_Read_Layer_Map(infile, &layer);

        w           = layer.width;
        h           = layer.height;
        channels    = SLIM_CODE::CODE_GRAY;
        data        = (unsigned char*)layer.img;
        SLIM_STREAM_CLOSE(infile);

        if(channels!=SLIM_CODE::CODE_GRAY){return;}

        unsigned char* dataimg = (unsigned char*)SLIM_Malloc(w * h * 3);
        grayToInferno(data, dataimg, w, h);
        save_image(fileB, dataimg, w, h,SLIM_CODE::CODE_RGB,255);
        SLIM_Free(dataimg);
        SLIM_Free(data);

        SLIM_Free(layer.name);
        SLIM_Free(layer.ext);
        SLIM_Free(layer.img);
    }




}



bool FileNotExistSave(std::string path){

    if (std::filesystem::exists(path)) {
        std::cout << "File \"" << path << "\" already exists. Overwrite? [y/N]: ";
        std::string answer;
        std::getline(std::cin, answer);

        if (answer != "y" && answer != "Y") {
            std::cout << "Aborted. File will not be overwritten.\n";
            return false;
        }
    }

    return true;
}

int main(int argc, char* argv[]) {

    if (argc < 2) {
        showHelp();
        return 0;
    }

    std::vector<std::string> args(argv + 1, argv + argc);
    Mode mode = Mode::NONE;
    uint8_t imageQuality = 255;
    bool overwrite = false;
    std::vector<std::string> files;

    for (size_t i = 0; i < args.size(); ++i) {
        if (args[i] == "-h") {
            showHelp();
            return 0;
        } else if (args[i] == "-v") {
            mode = Mode::VIEW;
        } else if (args[i] == "-m") {
            mode = Mode::VIEWMAP;
        }else if (args[i] == "-c") {
            mode = Mode::CONVERT;
        } else if (args[i] == "-w") {
            mode = Mode::SAVEMAP;
        } else if (args[i] == "-i") {
            mode = Mode::INFO;
        } else if (args[i] == "-a") {
            mode = Mode::ANALIZE;
        } else if (args[i] == "-y") {
            overwrite = true;
        } else if (args[i] == "-q") {
            if (i + 1 < args.size()) {
                try {
                    imageQuality = std::stoi(args[i + 1]);
                    ++i;
                } catch (const std::exception& e) {
                    std::cerr << "Error: Invalid quality value. Using default quality 255.\n";
                    imageQuality = 255;
                }
            } else {
                std::cerr << "Error: -q requires a quality value (0-255). Using default quality 255.\n";
            }
        } else {
            if (!args[i].empty() && args[i][0] != '-') {
                files.push_back(args[i]);
            }
        }
    }

    if (mode == Mode::NONE) {
        mode = Mode::VIEW;
    }

    if (mode == Mode::VIEW) {
        if(files.size()<1){return -1;}
        DemoIMG(files[0]);
    } else if (mode == Mode::VIEWMAP) {
        if(files.size()<1){return -1;}
        DemoMapSLIMIMG(files[0]);
    } else if (mode == Mode::CONVERT) {
        if(files.size()<2){return -1;}
        if(files[0]==files[1]){return -1;}

        if(!overwrite){if(!FileNotExistSave(files[1])){return 0;}}

        ConvertIMG(files[0],files[1],imageQuality);
    } else if (mode == Mode::SAVEMAP) {
        if(files.size()<2){return -1;}
        if(files[0]==files[1]){return -1;}

        if(detect_format(files[0]) != ImageFormat::fSLIM){std::cout << "You can only export a map from the SLIM format!\n";return 0;}
        if(!overwrite){if(!FileNotExistSave(files[1])){return 0;}}
        
        SaveMapToIMG(files[0],files[1]);
    
    } else if (mode == Mode::ANALIZE) {
        if(files.size()<2){return -1;}
        AnalizeIMG(files);
    } else if (mode == Mode::INFO) {
        if(files.size()<1){return -1;}
        InfoIMG(files[0]);
    }

    return 0;
}