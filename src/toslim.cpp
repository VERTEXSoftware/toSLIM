

const char* PROGRAM_VERSION = "1.2.0.0";
const char* PROGRAM_AUTHOR = "Sleptsov Vladimir";
const char* PROGRAM_DESCRIPTION = "miniSLIM";
const char* BUILD_DATE = __DATE__;
const char* BUILD_TIME = __TIME__;

#include <iostream>
#include <string>
#include <string>
#include <filesystem>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "SLIM/miniSLIM.h"
#include "support/support.h"
#include "../external/stb/stb_image.h"
#include "../external/stb/stb_image_write.h"


bool loadotherformat(const std::string& input, unsigned char* &data, int &w, int &h,  SLIMCODE &channels){
    int chan=0;
    data = stbi_load(input.c_str(), &w, &h, &chan, 0);

    channels=ChannelToCode(chan);

    return data!=NULL;
}

bool load_image(const std::string& input, unsigned char* &data, int &w, int &h,  SLIMCODE &channels) {
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
                IStream infile(input.c_str(), MiniStream::Mode::Read);

                if(infile.isOpen()) {
                    SLIM_INFO header;

                    Load_SLIM(infile, header, data);

                    w           = header._WIDTH;
                    h           = header._HEIGHT;
                    channels    = (SLIMCODE)header._CODE;

                    infile.close();
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



#ifdef ONLY_TERMINAL
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
    std::cout << "  -a          Comparison of images using PSNR/SSIM/PSQNR\n";
    std::cout << "  -h          Show this help message\n";
    std::cout << "  -y          Overwrite file\n";
    std::cout << "\nExamples:\n";
    std::cout << "  toslim -c image.png image.SLIM                 Convert image.png to image.SLIM\n";
    std::cout << "  toslim -c image.SLIM image.png                 Convert image.SLIM to image.png\n";
    std::cout << "  toslim -c -q 128 image.SLIM image.png          Convert image.png to image.SLIM quality 50%\n";
    std::cout << "  toslim -a image.SLIM image.png                 Comparing image.png with image.SLIM\n";
    std::cout << "\nDefault:\n";
    std::cout << "  Quality: 255 (MAX)\n";
    }
#else
#include "support/image_viewer.h"

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
    std::cout << "  -a          Comparison of images using PSNR/SSIM/PSQNR\n";
    std::cout << "  -h          Show this help message\n";
    std::cout << "  -y          Overwrite file\n";
    std::cout << "\nExamples:\n";
    std::cout << "  toslim image.png                               Display image\n";
    std::cout << "  toslim -v image.SLIM                           Display image\n";
    std::cout << "  toslim -c image.png image.SLIM                 Convert image.png to image.SLIM\n";
    std::cout << "  toslim -c image.SLIM image.png                 Convert image.SLIM to image.png\n";
    std::cout << "  toslim -c -q 128 image.SLIM image.png          Convert image.png to image.SLIM quality 50%\n";
    std::cout << "  toslim -a image.SLIM image.png                 Comparing image.png with image.SLIM\n";
    std::cout << "\nDefault:\n";
    std::cout << "  Quality: 255 (MAX)\n";
    }

    void DemoIMG(std::string file){

    unsigned char* data = NULL;
    int w = 0;
    int h = 0;
    SLIMCODE channels;


    load_image(file, data, w, h, channels);

    ImageViewer viewer(data, w, h, channels);
    viewer.show(file);

    if(data!=NULL){free(data);}
    }


    void DemoMapSLIMIMG(std::string file){

        unsigned char* data = NULL;
        int w = 0;
        int h = 0;
        SLIMCODE channels=SLIMCODE::CODE_NONE;


        IStream infile(file.c_str(), MiniStream::Mode::Read);

        if(infile.isOpen()) {
            SLIM_INFO header;

            Load_SLIM_Map(infile, header, data);

            w   = header._WIDTH;
            h   = header._HEIGHT;
            channels    = (SLIMCODE)header._CODE;

            infile.close();
        }    

        ImageViewer viewer(data, w, h, channels);
        viewer.show(file);

        if(data!=NULL){free(data);}

    }
#endif



enum class Mode {
    NONE,
    VIEW,
    CONVERT,
    ANALIZE,
    INFO,
    VIEWMAP
};

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

    IStream infile(imagePath.c_str(),MiniStream::Mode::Read);
       
    if(infile.isOpen()){
        sizefile=infile.size(); 
        infile.close();
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
                IStream infile(imagePath.c_str(),MiniStream::Mode::Read);
                
                if(infile.isOpen()){
                    
                    SLIM_INFO_FULL header;
                    Info_SLIM(infile,header);    

                    std::cout << "----[ INFORMATION ]----\n";

                    std::cout<<"FORMAT: Sleptsov Vladimir Image (SLIM)\n";

                    uint8_t major = (header._VERS>> 24) & 0xFF;
                    uint8_t minor = (header._VERS>> 16) & 0xFF;
                    uint8_t patch = (header._VERS >> 8) & 0xFF;
                    uint8_t build = header._VERS & 0xFF;

                    std::cout<<"VERSION: "<<(int)major <<"."<<(int)minor<<"."<<(int)patch<<"."<<(int)build<< "\n";
                    std::cout<<"WIDTH: "<<header._WIDTH<< "\n";
                    std::cout<<"HEIGHT: "<<header._HEIGHT<< "\n";
                    std::cout<<"CODE: ";

                    uint32_t chanells =1;

                    switch (header._CODE)
                    {
                        case SLIMCODE::CODE_RGB:
                            std::cout<<"RGB (3 channels)\n";
                            chanells =3;
                            break;
                        case SLIMCODE::CODE_RGBA:
                            std::cout<<"RGBA (4 channels)\n";
                            chanells =4;
                            break;
                        default:
                            std::cout<<"NONE (not defined)\n";
                    }
                    uint32_t sizefile=infile.size();
                    uint32_t sizefileraw=header._WIDTH*header._HEIGHT*chanells;


                    std::cout<<compressionRatio(sizefileraw,sizefile)<<"\n";
                    std::cout<<"SIZE COMP: "<<sizefile<<" ("<<formatSize(sizefile)<<")\n";
                    std::cout<<"SIZE RAW: "<<sizefileraw<<" ("<<formatSize(sizefileraw)<<")\n";

                    const auto totalpix = header._BLOCK_256_C;

                    std::cout << "\n----[ BLOCKS " << totalpix << " ]----\n";
                    std::cout << "COLOR MIN: "<< header._BLOCK_COLOR_TABLE_MIN<< "\n";
                    std::cout << "COLOR MAX: "<< header._BLOCK_COLOR_TABLE_MAX<< "\n";
                    std::cout << "COLOR AVG: "<< header._BLOCK_COLOR_TABLE_AVG<< "\n";
                     std::cout << "\n----[ BLOCKS " << totalpix << " ]----\n";
                    std::cout << "DELTA MIN: "<< header._BLOCK_Q_MIN<< "\n";
                    std::cout << "DELTA MAX: "<< header._BLOCK_Q_MAX<< "\n";
                    std::cout << "DELTA AVG: "<< header._BLOCK_Q_AVG<< "\n";

                    const uint32_t total = header._ALL_C;

                    std::cout << "\n----[ LINES " << total << " ]----\n";
                    std::cout << "REUSE: "<< header._REUSE_C<< " (" << Percent(header._REUSE_C, total) << "%)\n";
                    std::cout << "ORIGINAL: "<< header._ORIGINAL_C<< " (" << Percent(header._ORIGINAL_C, total) << "%)\n";
                    std::cout << "RLE: "<< header._RLE_C<< " (" << Percent(header._RLE_C, total) << "%)\n";
                    std::cout << "RICE: "<< header._RICE_C<< " (" << Percent(header._RICE_C, total) << "%)\n";
                    std::cout << "SLDD: "<< header._SLDD_C<< " (" << Percent(header._SLDD_C, total) << "%)\n";
                    std::cout << "MASKARED: "<< header._MASKARED_C<< " (" << Percent(header._MASKARED_C, total) << "%)\n";

                    infile.close();
                }
            }
            break;
        default:
            std::cerr << "Unknown type of format\n";
            break;
    }
}




bool save_image(const std::string& output, unsigned char* data, int w, int h,  SLIMCODE chan, uint8_t quality) {


    
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
                IStream infile(output.c_str(),MiniStream::Mode::Write);
                
                if(infile.isOpen()){
                    uint8_t     code    = chan;
                    uint8_t*    img     = (uint8_t*)data;
                    uint16_t    width   = (uint16_t)w;
                    uint16_t    height  = (uint16_t)h;

                    SLIM_INFO header = Create_Info(width, height, code, FILTER_COLORDIV, quality);
                    Save_SLIM(infile,header,img);             

                    infile.close();
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
    SLIMCODE c1;

    int w2=0;
    int h2=0;
    SLIMCODE c2;

    unsigned char* img1=NULL;
    unsigned char* img2=NULL;

    load_image(orig, img1, w1, h1, c1);

    std::cout << "Original: " << orig << "\n";

    for(size_t i=1;i<files.size();++i){

        std::string file = files[i];

        load_image(file, img2, w2, h2, c2);

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

        if(img2!=NULL){free(img2);}
    }

    if(img1!=NULL){free(img1);}
}




void ConvertIMG(std::string fileA,std::string fileB, uint8_t quality){

    unsigned char* data = NULL;
    int w = 0;
    int h = 0;
    SLIMCODE channels;


    if(load_image(fileA, data, w, h, channels)){
        save_image(fileB, data, w, h,channels,quality);
    }

    if(data!=NULL){free(data);}

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
        } 
        #ifdef ONLY_TERMINAL
        #else
        else if (args[i] == "-v") {
            mode = Mode::VIEW;
        }else if (args[i] == "-m") {
            mode = Mode::VIEWMAP;
        }
        #endif
        else if (args[i] == "-c") {
            mode = Mode::CONVERT;
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
    #ifdef ONLY_TERMINAL
    #else
    if (mode == Mode::VIEW) {
        if(files.size()<1){return -1;}
        DemoIMG(files[0]);
    }else if (mode == Mode::VIEWMAP) {
        if(files.size()<1){return -1;}
        DemoMapSLIMIMG(files[0]);
    } 
    #endif
    else if (mode == Mode::CONVERT) {
        if(files.size()<2){return -1;}
        if(files[0]==files[1]){return -1;}

        if(!overwrite){if(!FileNotExistSave(files[1])){return 0;}}

        ConvertIMG(files[0],files[1],imageQuality);
    } else if (mode == Mode::ANALIZE) {
        if(files.size()<2){return -1;}
        AnalizeIMG(files);
    } else if (mode == Mode::INFO) {
        if(files.size()<1){return -1;}
        InfoIMG(files[0]);
    }

    return 0;
}