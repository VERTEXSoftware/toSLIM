#ifndef SUPPORT_H
#define SUPPORT_H


#include "SLIM/miniSLIM.h"
#include <cmath>
#include <vector>

SLIMCODE ChannelToCode(int channels);

double calcPSNR(const unsigned char* img1, const unsigned char* img2, int width, int height, int channels);
double calcPSQNR(const unsigned char* img1, const unsigned char* img2, int width, int height, int channels);
double calcSSIM(const unsigned char* img1, const unsigned char* img2, int width, int height, int channels);
void grayToViridis(const unsigned char* grayscale, unsigned char* rgb, int width, int height);

std::string formatSize(uint32_t size);
std::string compressionRatio(size_t originalSize, size_t compressedSize);

static std::string getFileExtension(const std::string& filename) {
    auto pos = filename.find_last_of('.');
    if (pos == std::string::npos) return "";
    std::string ext = filename.substr(pos + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return ext;
}


SLIMCODE ChannelToCode(int channels){

    SLIMCODE code = CODE_NONE;

    switch (channels) {
        case 1:
            code = CODE_NONE;
            break;
        case 3:
            code = CODE_RGB;
            break;
        case 4:
            code = CODE_RGBA;
            break;
        default:
            code = CODE_NONE;
            break;
    }
    return code;
}

int CodeToChannel(SLIMCODE code){
    int channels = 0;

    switch (code) {
        case SLIMCODE::CODE_NONE:
            channels = 0;
            break;
        case SLIMCODE::CODE_RGB:
            channels = 3;
            break;
        case SLIMCODE::CODE_RGBA:
            channels = 4;
            break;
        default:
            channels = 0;
            break;
    }
    return channels;
}

enum class ImageFormat {
    fPNG, fJPG, fBMP, fTGA,fSLIM,fUNKNOWN
};

std::string getExtension(const ImageFormat& format) {
    switch (format) {
        case ImageFormat::fPNG: return "png";
        case ImageFormat::fJPG: return "jpg";
        case ImageFormat::fBMP: return "bmp";
        case ImageFormat::fTGA: return "tga";
        case ImageFormat::fSLIM: return "SLIM";
        default: return "";
    }
}

ImageFormat detect_format(const std::string& path) {
    std::string ext = getFileExtension(path);

    if (ext == "png"){return ImageFormat::fPNG;}
    if (ext == "jpg" || ext == "jpeg"){return ImageFormat::fJPG;}
    if (ext == "bmp"){return ImageFormat::fBMP;}
    if (ext == "tga"){return ImageFormat::fTGA;}
    if (ext == "SLIM" || ext == "slim"){ return ImageFormat::fSLIM;}
    return ImageFormat::fUNKNOWN;
}



std::string formatSize(uint32_t size) {

    double bytes = size;

    const char* suffixes[] = {"B", "KB", "MB", "GB", "TB"};
    int i = 0;
    while (bytes >= 1024 && i < 4) {
        bytes /= 1024;
        i++;
    }

    char buf[32];
    std::snprintf(buf, sizeof(buf), "%.2f %s", bytes, suffixes[i]);
    return buf;
}

std::string compressionRatio(size_t originalSize, size_t compressedSize) {
    if (originalSize == 0 || compressedSize == 0){
        return "incorrect data";
    }

    double ratio = (double)originalSize / compressedSize;
    double percent = (1.0 - (double)compressedSize / originalSize) * 100.0;

    char buf[64];
    if (compressedSize <= originalSize)
        std::snprintf(buf, sizeof(buf), "COEF: %.2f (%.2f%% economy)", ratio, percent);
    else
        std::snprintf(buf, sizeof(buf), "COEF: %.2f (%.2f%% inflation)", ratio, -percent);

    return buf;
}





void grayToViridis(const unsigned char* grayscale, unsigned char* rgb, int width, int height) {
    const int totalPixels = width * height;
    
    unsigned char minGray = 255;
    unsigned char maxGray = 0;
    for (int i = 0; i < totalPixels; ++i) {
        if (grayscale[i] > maxGray){maxGray = grayscale[i];}
        if (grayscale[i] < minGray){minGray = grayscale[i];}
    }
    
    if (maxGray == minGray){maxGray = minGray + 1;}
    
    for (int i = 0; i < totalPixels; ++i) {
        double t = (double)(grayscale[i] - minGray) / (maxGray - minGray);

        unsigned char r = 0;
        unsigned char g = 0; 
        unsigned char b = 0;
        
        if (t < 0.25) {
            double local_t = t / 0.25;
            r = (unsigned char)(36 * local_t);
            g = 0;
            b = (unsigned char)(68 * local_t);
        }
        else if (t < 0.5) {
            double local_t = (t - 0.25) / 0.25;
            r = (unsigned char)(36 + (155 * local_t));
            g = (unsigned char)(33 * local_t);
            b = (unsigned char)(68 + (59 * local_t));
        }
        else if (t < 0.75) {
            double local_t = (t - 0.5) / 0.25;
            r = (unsigned char)(191 + (55 * local_t));
            g = (unsigned char)(33 + (92 * local_t));
            b = (unsigned char)(127 - (68 * local_t));
        }
        else {
            double local_t = (t - 0.75) / 0.25;
            r = (unsigned char)(246 + (9 * local_t));
            g = (unsigned char)(125 + (130 * local_t));
            b = (unsigned char)(59 + (196 * local_t));
        }
        
        rgb[i * 3]     = r;
        rgb[i * 3 + 1] = g;
        rgb[i * 3 + 2] = b;
    }
}

double calcPSNR(const unsigned char* img1, const unsigned char* img2, int width, int height, int channels) {

    if (!img1 || !img2 || width <= 0 || height <= 0 || channels <= 0){return -1.0;}

    double mse = 0.0;
    int size = width * height * channels;
    for (int i = 0; i < size; ++i) {
        double diff = (double)(img1[i]) - double(img2[i]);
        mse += diff * diff;
    }
    mse /= size;
    if (mse == 0){return INFINITY;}
    return 10.0 * log10((255.0 * 255.0) / mse);
}


double calcPSQNR(const unsigned char* img1, const unsigned char* img2, int width, int height, int channels){
    
    if (!img1 || !img2 || width <= 0 || height <= 0 || channels <= 0){return -1.0;}

    const int total = width * height * channels;
    double weightedError = 0.0;
    double weightSum = 0.0;

    for (int  i = 0; i < total; ++i)
    {
        const double p1 = double(img1[i]);
        const double p2 = double(img2[i]);
        const double diff = p1 - p2;

        const double luminanceWeight = 1.0 / (1.0 + 0.003 * p1 * p1);
        const double error = diff * diff * luminanceWeight;

        weightedError += error;
        weightSum += luminanceWeight;
    }

    const double mse = weightedError / weightSum;
    if (mse <= 0.0)
        return 0.0;

    const double MAX_I = 255.0;
    return 10.0 * std::log10((MAX_I * MAX_I) / mse);
}


double calcSSIM(const unsigned char* img1, const unsigned char* img2, int width, int height, int channels) {

    if (!img1 || !img2 || width <= 0 || height <= 0 || channels <= 0){return -1.0;}

    double* Y1 = new double[width * height]{0};
    double* Y2 = new double[width * height]{0};

    for (int i = 0; i < width * height; ++i) {
        if (channels >= 3) {
            Y1[i] = 0.299 * img1[i * channels + 0] + 0.587 * img1[i * channels + 1] + 0.114 * img1[i * channels + 2];
            Y2[i] = 0.299 * img2[i * channels + 0] + 0.587 * img2[i * channels + 1] + 0.114 * img2[i * channels + 2];
        } else {
            Y1[i] = img1[i * channels];
            Y2[i] = img2[i * channels];
        }
    }

    double mu1 = 0.0;
    double mu2 = 0.0;
    for (int i = 0; i < width * height; ++i) {
        mu1 += Y1[i];
        mu2 += Y2[i];
    }
    mu1 /= (width * height);
    mu2 /= (width * height);

    double sigma1 = 0.0, sigma2 = 0.0, sigma12 = 0.0;
    for (int i = 0; i < width * height; ++i) {
        sigma1 += (Y1[i] - mu1) * (Y1[i] - mu1);
        sigma2 += (Y2[i] - mu2) * (Y2[i] - mu2);
        sigma12 += (Y1[i] - mu1) * (Y2[i] - mu2);
    }
    sigma1 /= (width * height - 1);
    sigma2 /= (width * height - 1);
    sigma12 /= (width * height - 1);

    const double C1 = (0.01 * 255) * (0.01 * 255);
    const double C2 = (0.03 * 255) * (0.03 * 255);

    double ssim = ((2 * mu1 * mu2 + C1) * (2 * sigma12 + C2)) / ((mu1 * mu1 + mu2 * mu2 + C1) * (sigma1 + sigma2 + C2));

    delete[] Y1;
    delete[] Y2;

    return ssim;
}


#endif // SUPPORT_H
