// SPDX-License-Identifier: MIT
// SLIM (Sleptsov Image format) for C/C++
// Version: 1.2.0.0
// Copyright (C) 2026 Sleptsov Vladimir 
// https://github.com/VERTEXSoftware

#ifndef miniSLIM_H
#define miniSLIM_H

#include <cmath>
#include <cstdio>
#include <cstdint>
#include <fstream>
#include <cstring>
#include <algorithm>
#include "./miniStream.h"

#define SLEP_SLDD_IMP
#define SLEP_MASKARED_IMP
#define RLE_IMP
#define RICE_IMP

//Custom Compression
#include "./compress/SLDD.h"
#include "./compress/MASKARED.h"
#include "./compress/RLE.h"
#include "./compress/RICE.h"

#define MINI_SLIM_HEADER 		"miniSLIM"

#define SLIM_VER_MAJOR 1
#define SLIM_VER_MINOR 2
#define SLIM_VER_BUGFIX 0
#define SLIM_VER_HOTFIX 0

#define SLIM_VER ((SLIM_VER_MAJOR << 24) | (SLIM_VER_MINOR << 16) | (SLIM_VER_BUGFIX << 8) | (SLIM_VER_HOTFIX))

#if defined(SLIM_MALLOC) && defined(SLIM_FREE)
// ok
#elif !defined(SLIM_MALLOC) && !defined(SLIM_FREE)
// ok
#else
#error "Must define all or none of SLIM_MALLOC, SLIM_FREE."
#endif

#ifndef SLIM_MALLOC
#define SLIM_MALLOC(sz)           malloc(sz)
#define SLIM_FREE(p)              free(p)
#endif


double perlin_noise_frame[256]{
0.542733, 0.526736, 0.445526, 0.566247, 0.426758, 0.60899, 0.721901, 0.469859, 0.5, 0.530141, 0.450104, 0.569366, 0.426758, 0.566247, 0.43148, 0.257248, 
0.526736, 0.636428, 0.519495, 0.494577, 0.423679, 0.633119, 0.709881, 0.443241, 0.5, 0.556759, 0.410785, 0.527608, 0.509748, 0.542102, 0.514311, 0.603491,
0.445526, 0.68481, 0.512071, 0.259638, 0.295893, 0.55983, 0.746392, 0.524091, 0.5, 0.475909, 0.226386, 0.339399, 0.516417, 0.381406, 0.51382, 0.756245,
0.566247, 0.808999, 0.634767, 0.420799, 0.566655, 0.748354, 0.605517, 0.280701, 0.5, 0.719299, 0.434042, 0.463292, 0.666275, 0.647604, 0.730281, 0.659798,
0.426758, 0.66951, 0.495277, 0.388043, 0.677721, 0.58526, 0.499869, 0.31965, 0.5, 0.68035, 0.367298, 0.328698, 0.5928, 0.774011, 0.604293, 0.514102,
0.60899, 0.823382, 0.637836, 0.471013, 0.482714, 0.32841, 0.556922, 0.541769, 0.480188, 0.418606, 0.231449, 0.453609, 0.701782, 0.613116, 0.339399, 0.527608,
0.721901, 0.664952, 0.512806, 0.352895, 0.369146, 0.188402, 0.255666, 0.463273, 0.364101, 0.26493, 0.300532, 0.361445, 0.583177, 0.499062, 0.226386, 0.410785,
0.46778, 0.464992, 0.583614, 0.463894, 0.603383, 0.421238, 0.308843, 0.560143, 0.529586, 0.499995, 0.580037, 0.460775, 0.68102, 0.720142, 0.4757, 0.558158, 
0.25, 0.319271, 0.550461, 0.433753, 0.573242, 0.410822, 0.413998, 0.49896, 0.375, 0.468819, 0.549896, 0.430634, 0.650879, 0.690001, 0.449539, 0.680729,
0.46778, 0.441842, 0.5243, 0.403612, 0.543101, 0.400406, 0.519152, 0.437777, 0.220414, 0.437643, 0.519755, 0.400494, 0.620738, 0.65986, 0.416386, 0.535008,
0.549896, 0.589215, 0.773614, 0.624692, 0.640944, 0.488919, 0.72446, 0.689871, 0.4547, 0.53522, 0.619666, 0.682327, 0.8111, 0.877844, 0.660303, 0.475267,
0.430634, 0.472392, 0.660601, 0.510638, 0.522339, 0.369967, 0.609802, 0.578352, 0.50762, 0.45813, 0.43469, 0.662892, 0.546678, 0.681273, 0.551017, 0.501456,
0.573242, 0.438076, 0.394461, 0.349743, 0.631321, 0.492947, 0.460697, 0.31965, 0.5, 0.68035, 0.517937, 0.433203, 0.283916, 0.47903, 0.373181, 0.515828,
0.433753, 0.18236, 0.265605, 0.47615, 0.557846, 0.358352, 0.393953, 0.280701, 0.5, 0.719299, 0.535525, 0.397897, 0.263282, 0.586368, 0.498129, 0.290392,
0.554474, 0.31519, 0.487929, 0.74289, 0.716698, 0.499154, 0.618416, 0.538106, 0.507023, 0.47594, 0.429479, 0.631887, 0.479845, 0.518282, 0.367135, 0.272283, 
0.473264, 0.363572, 0.480505, 0.553933, 0.817912, 0.673386, 0.792648, 0.712132, 0.634744, 0.557356, 0.536289, 0.801582, 0.57143, 0.253047, 0.276609, 0.446613
};

double atten_frame[256]{
0, 0.00392157, 0.00784314, 0.0117647, 0.0156863, 0.0196078, 0.0235294, 0.027451, 0.0313725, 0.0352941, 0.0392157, 0.0431373, 0.0470588, 0.0509804, 0.054902, 0.0588235, 
0.0627451, 0.0666667, 0.0705882, 0.0745098, 0.0784314, 0.0823529, 0.0862745, 0.0901961, 0.0941176, 0.0980392, 0.101961, 0.105882, 0.109804, 0.113725, 0.117647, 0.121569, 
0.12549, 0.129412, 0.133333, 0.137255, 0.141176, 0.145098, 0.14902, 0.152941, 0.156863, 0.160784, 0.164706, 0.168627, 0.172549, 0.176471, 0.180392, 0.184314, 
0.188235, 0.192157, 0.196078, 0.2, 0.203922, 0.207843, 0.211765, 0.215686, 0.219608, 0.223529, 0.227451, 0.231373, 0.235294, 0.239216, 0.243137, 0.247059, 
0.25098, 0.254902, 0.258824, 0.262745, 0.266667, 0.270588, 0.27451, 0.278431, 0.282353, 0.286275, 0.290196, 0.294118, 0.298039, 0.301961, 0.305882, 0.309804, 
0.313725, 0.317647, 0.321569, 0.32549, 0.329412, 0.333333, 0.337255, 0.341176, 0.345098, 0.34902, 0.352941, 0.356863, 0.360784, 0.364706, 0.368627, 0.372549, 
0.376471, 0.380392, 0.384314, 0.388235, 0.392157, 0.396078, 0.4, 0.403922, 0.407843, 0.411765, 0.415686, 0.419608, 0.423529, 0.427451, 0.431373, 0.435294, 
0.439216, 0.443137, 0.447059, 0.45098, 0.454902, 0.458824, 0.462745, 0.466667, 0.470588, 0.47451, 0.478431, 0.482353, 0.486275, 0.490196, 0.494118, 0.498039, 
0.501961, 0.505882, 0.509804, 0.513725, 0.517647, 0.521569, 0.52549, 0.529412, 0.533333, 0.537255, 0.541176, 0.545098, 0.54902, 0.552941, 0.556863, 0.560784, 
0.564706, 0.568627, 0.572549, 0.576471, 0.580392, 0.584314, 0.588235, 0.592157, 0.596078, 0.6, 0.603922, 0.607843, 0.611765, 0.615686, 0.619608, 0.623529, 
0.627451, 0.631373, 0.635294, 0.639216, 0.643137, 0.647059, 0.65098, 0.654902, 0.658824, 0.662745, 0.666667, 0.670588, 0.67451, 0.678431, 0.682353, 0.686275, 
0.690196, 0.694118, 0.698039, 0.701961, 0.705882, 0.709804, 0.713725, 0.717647, 0.721569, 0.72549, 0.729412, 0.733333, 0.737255, 0.741176, 0.745098, 0.74902, 
0.752941, 0.756863, 0.760784, 0.764706, 0.768627, 0.772549, 0.776471, 0.780392, 0.784314, 0.788235, 0.792157, 0.796078, 0.8, 0.803922, 0.807843, 0.811765, 
0.815686, 0.819608, 0.823529, 0.827451, 0.831373, 0.835294, 0.839216, 0.843137, 0.847059, 0.85098, 0.854902, 0.858824, 0.862745, 0.866667, 0.870588, 0.87451, 
0.878431, 0.882353, 0.886275, 0.890196, 0.894118, 0.898039, 0.901961, 0.905882, 0.909804, 0.913725, 0.917647, 0.921569, 0.92549, 0.929412, 0.933333, 0.937255, 
0.941176, 0.945098, 0.94902, 0.952941, 0.956863, 0.960784, 0.964706, 0.968627, 0.972549, 0.976471, 0.980392, 0.984314, 0.988235, 0.992157, 0.996078, 1
};



enum	SLIMERROR {

		ERROR_OK			= 0x0,
		ERROR_FILE			= 0x1,
		ERROR_BLOCK			= 0x2,
		ERROR_DATA			= 0x3,
		ERROR_END			= 0x4,
		ERROR_NOTSUP		= 0x5,
		ERROR_NONE			= 0x6,
		ERROR_MEM			= 0x7,
		ERROR_ARG			= 0x8,
		ERROR_COLORSPACE	= 0x9
};

enum	SLIMCOMP {

		COMP_NONE			= 0x0,
		COMP_RLE			= 0x1,
		COMP_MASKARED		= 0x2,
		COMP_SLDD			= 0x3,
		COMP_ADAPTIVE		= 0x4

};

enum	SLIMFILTER {

		FILTER_NONE			= 0x0,
		FILTER_YCBCRDIV		= 0x1,
		FILTER_YCBCRSTEP	= 0x2,
		FILTER_COLORDIV		= 0x3,
		FILTER_STEP			= 0x4

};

enum	SLIMCODE {
		CODE_NONE			= 0x0,
		CODE_RGB			= 0x3,
		CODE_RGBA			= 0x4,
		CODE_MAP			= 0x5,
};

struct		SLIM_INFO {

	uint32_t				_VERS;
	uint16_t				_WIDTH;
	uint16_t				_HEIGHT;
	uint8_t					_CODE;	
	uint8_t					_FILTER;
	uint8_t					_LEVEL;
};

struct		SLIM_INFO_FULL {

	uint32_t				_VERS;
	uint16_t				_WIDTH;
	uint16_t				_HEIGHT;
	uint8_t					_CODE;	
	uint8_t					_FILTER;
	uint8_t					_LEVEL;

	uint32_t 				_BLOCK_256_ALL;
	uint32_t 				_BLOCK_256_EXIST;
	uint32_t 				_BLOCK_256_EMPTY;

	uint32_t				_BLOCK_COLOR_TABLE_MAX;
	uint32_t				_BLOCK_COLOR_TABLE_MIN;
	uint32_t				_BLOCK_COLOR_TABLE_AVG;

	uint32_t				_BLOCK_Q_MAX;
	uint32_t				_BLOCK_Q_MIN;
	uint32_t				_BLOCK_Q_AVG;

	uint32_t				_ALL_C;
	uint32_t				_REUSE_C;
	uint32_t				_ORIGINAL_C;	
	uint32_t				_RLE_C;
	uint32_t				_RICE_C;
	uint32_t				_SLDD_C;
	uint32_t				_MASKARED_C;

};


SLIM_INFO Create_Info(uint16_t w, uint16_t h, uint8_t code = CODE_RGBA, uint8_t filter = FILTER_COLORDIV, uint8_t level = 2);

SLIMERROR Load_SLIM(MiniStream &infile, SLIM_INFO &header,uint8_t* &img);

SLIMERROR Save_SLIM(MiniStream &outfile, SLIM_INFO &header, uint8_t* &img);

SLIMERROR Info_SLIM(MiniStream &infile, SLIM_INFO_FULL &info);

SLIMERROR Free_Buf(void* buf);

SLIMERROR Load_SLIM_Map(MiniStream &infile, SLIM_INFO &header,uint8_t* &img);

SLIMERROR Load_SLIM_Mini(MiniStream &infile, SLIM_INFO &header,uint8_t* &img);

SLIMERROR Free_Buf(void* buf){

	if(buf!=NULL){return SLIMERROR::ERROR_ARG;}

	SLIM_FREE(buf);

	return SLIMERROR::ERROR_OK;
}



bool IsOrgLine(uint8_t* a, uint8_t* b, uint32_t count) {

	uint8_t* _a = a;
	uint8_t* _b = b;
	uint32_t num = count;

	while (num--) {
		if (*_a++ != *_b++) {
			return true;
		}
	}

	return false;
}


void GEN_CLR_MAP_RGB(uint8_t* R, uint8_t* G, uint8_t* B, uint32_t& size, uint8_t* idx, uint32_t pidx, uint8_t cR, uint8_t cG, uint8_t cB) {

	uint32_t pos = 0;
	uint32_t fnd = ((uint32_t)cR << 16) | ((uint32_t)cG << 8) | (uint32_t)cB;

    while (pos < size) {
        uint32_t cur = ((uint32_t)R[pos] << 16) | ((uint32_t)G[pos] << 8) | (uint32_t)B[pos];
        
        if (cur == fnd) {
            idx[pidx] = pos;
            return;
        }
        if (cur > fnd) {
            break;
        }
        ++pos;
    }

	for (uint32_t i = 0; i < pidx; ++i) {
		if (idx[i] >= pos) { ++idx[i]; }
	}

	for (uint32_t i = size; i > pos; --i) {
		R[i] = R[i - 1];
		G[i] = G[i - 1];
		B[i] = B[i - 1];
	}

	R[pos] 		= cR;
	G[pos] 		= cG;
	B[pos] 		= cB;
	idx[pidx] 	= pos;
	++size;
}

void GEN_CLR_MAP_RGBA(uint8_t* R, uint8_t* G, uint8_t* B, uint8_t* A, uint32_t& size, uint8_t* idx, uint32_t pidx, uint8_t cR, uint8_t cG, uint8_t cB, uint8_t cA) {

	uint32_t pos = 0;
	uint32_t fnd = ((uint32_t)cR << 24) | ((uint32_t)cG << 16) | ((uint32_t)cB << 8) | (uint32_t)cA;

    while (pos < size) {
        uint32_t cur = ((uint32_t)R[pos] << 24) |((uint32_t)G[pos] << 16) | ((uint32_t)B[pos] << 8) | (uint32_t)A[pos];
        
        if (cur == fnd) {
            idx[pidx] = pos;
            return;
        }
        if (cur > fnd) {
            break;
        }
        ++pos;
    }

	for (uint32_t i = 0; i < pidx; ++i) {
		if (idx[i] >= pos) { ++idx[i]; }
	}

	for (uint32_t i = size; i > pos; --i) {
		R[i] = R[i - 1];
		G[i] = G[i - 1];
		B[i] = B[i - 1];
		A[i] = A[i - 1];
	}

	R[pos] 		= cR;
	G[pos] 		= cG;
	B[pos] 		= cB;
	A[pos] 		= cA;
	idx[pidx] 	= pos;
	++size;

}




uint16_t ENCODE_REVOLVER(bool orig, uint8_t* src, uint8_t* dest, uint32_t size, uint32_t &r_size) {

	//--------------------------------------------------------------//
	//Encode by the revolver method
	//--------------------------------------------------------------//

	if (size <= 0) { return 0; }
	if (orig==false)  {return 0; }

	uint8_t t_rle      [1024]{ 0 };
	uint8_t t_rice     [1024]{ 0 };
    uint8_t t_sldd     [1024]{ 0 };
    uint8_t t_maskared [1024]{ 0 };

    uint32_t r_size_pack    [5]{size,size,size,size,size};
    uint8_t* pack           [5]{src, t_rle,t_rice, t_sldd, t_maskared };

	uint16_t pos_mode = 0;

    RLE_ENCODE(src, size, pack[1], r_size_pack[1]);
	RICE_ENCODE(src, size, pack[2], r_size_pack[2]);
    SLDD_ENCODE(src, size, pack[3], r_size_pack[3]);
    MASKARED_ENCODE(src, size, pack[4], r_size_pack[4]);

    for(uint16_t i = 1; i < 5; ++i){
        if(r_size_pack[pos_mode]>r_size_pack[i]){
            pos_mode = i;
        }
    }

	r_size = r_size_pack[pos_mode];

	uint8_t* d = dest;
	uint8_t* s = pack[pos_mode];
	uint8_t* e = s + r_size;
	while (s < e) {*d++ = *s++;}

	return pos_mode+1;
}




void  DECODE_REVOLVER(uint16_t mode, uint8_t* src, uint8_t* dest, uint32_t size) {

	//--------------------------------------------------------------//
	//Decode by the revolver method
	//--------------------------------------------------------------//

	if (size<=0) { return; }
	if (mode==0)  { return; }

	uint32_t r_size = 0;

	switch (mode)
	{
		case 1:
		{
			uint8_t* d = dest;
			uint8_t* s = src;
			uint8_t* e = s + size;
			while (s < e) {*d++ = *s++;}
			break;
		}	
		case 2:
		{
			RLE_DECODE(src, size, dest, r_size);
			break;
		}
		case 3:
		{
			r_size = 256;
			RICE_DECODE(src, size, dest, r_size);
			break;
		}
		case 4:
		{
			r_size = 256;
			SLDD_DECODE(src, size, dest, r_size);
			break;
		}		
		case 5:
		{
			r_size = 256;
			MASKARED_DECODE(src, size, dest, r_size);
			break;
		}
		default:
		{
			return;
		}		
	}
}


uint32_t BLOCK_ANALYZER(uint8_t level,uint8_t* img, uint32_t m_WIDTH, uint32_t m_HEIGHT, uint32_t blocksX, uint32_t blocksY, uint32_t channels = 3) {

	//--------------------------------------------------------------//
	//Counting unique colors
	//--------------------------------------------------------------//

	uint32_t colors[256]{0};
	uint32_t colorCount = 0;

    for (uint32_t y = 0; y < 16; ++y)
    {
        for (uint32_t x = 0; x < 16; ++x)
        {
			uint32_t row    = blocksY + y;
            uint32_t column = blocksX + x;

            if (column >= m_WIDTH || row >= m_HEIGHT){continue;}

            uint32_t idx = channels * (row * m_WIDTH + column);

            uint8_t r = channels>0	? img[idx]		:0;
            uint8_t g = channels>1	? img[idx+1]	:0;
            uint8_t b = channels>2	? img[idx+2]	:0;
			uint8_t a = channels>3	? img[idx+3]	:0;

            uint32_t color = (r << 24) | (g << 16) | (b << 8) | a;
			bool found = false;

			for (uint32_t i = 0; i < colorCount; ++i)
			{
				if (colors[i] == color){found = true;break;}
			}

			if (!found){colors[colorCount++] = color;}
        }
    }

   	uint32_t levelq = colorCount * 0.0274509803;  // (7 / 255)
	
	uint32_t count = 0;
	double sumDiff = 0;
    const double invLevelq = levelq == 0 ? 1.0 : 1.0 / levelq * 2.0;

	//--------------------------------------------------------------//
	//PSNR Analysis
	//--------------------------------------------------------------//

    for (uint32_t y = 0; y < 16; ++y)
    {
        for (uint32_t x = 0; x < 16; ++x)
        {
            uint32_t column = blocksX + x;
			uint32_t row    = blocksY + y;

            if (column >= m_WIDTH || row >= m_HEIGHT){continue;}

            uint32_t idx = channels * (row * m_WIDTH + column);

			if(channels>0){ 
				double c = (double)img[idx]; 
                double d = c - (c * invLevelq);
                sumDiff += d * d;
			} 
			if(channels>1){ 
				double c = (double)img[idx+1]; 
                double d = c - (c * invLevelq);
                sumDiff += d * d;
			} 
			if(channels>2){ 
				double c = (double)img[idx+2]; 
                double d = c - (c * invLevelq);
                sumDiff += d * d;
			} 
			if(channels>3){ 
				double c = (double)img[idx+3]; 
                double d = c - (c * invLevelq);
                sumDiff += d * d;
			}
            count += channels;
        }
    }

	double psnr = 1.0 - (sumDiff / count / 65025.0);

	//--------------------------------------------------------------//
	//Adjusting the evaluation level and quantization
	//--------------------------------------------------------------//

	if (psnr < 0.0){psnr = 0.0;}
    if (psnr > 1.0){psnr = 1.0;}
    
  	const double factor = (255.0 - (double)level) * 0.0156862745; //(4.0 / 255.0)

	uint32_t idxt = levelq * psnr * factor;

	if (idxt > 7){ idxt = 7;}

	return idxt;
}



SLIMERROR SLIM_WRITE_BLOCKS_3CHANNEL(MiniStream &outfile, SLIM_INFO &header, uint8_t* &img){


	uint32_t m_WIDTH = (uint32_t)header._WIDTH;
	uint32_t m_HEIGHT = (uint32_t)header._HEIGHT;

	uint8_t m_data		[1024]{0}; 	//Old		block memory
	uint8_t l_data		[1024]{0}; 	//Curret	block memory
	uint8_t m_write		[1024]{0}; 	//Curret	block packed
	uint8_t m_size		[4]{0};	 	//Size 		blocks packed

	//Pointers old block memory
	uint8_t* m_ch0 = m_data;
	uint8_t* m_ch1 = m_data + 256u;
	uint8_t* m_ch2 = m_data + 512u;
	uint8_t* m_idx = m_data + 768u;

	//Pointers curret block memory
	uint8_t* l_ch0 = l_data;
	uint8_t* l_ch1 = l_data + 256u;
	uint8_t* l_ch2 = l_data + 512u;
	uint8_t* l_idx = l_data + 768u;


	for (uint32_t blcY = 0; blcY < m_HEIGHT; blcY += 16)
	{
		for (uint32_t blcX = 0; blcX < m_WIDTH; blcX += 16)
		{

			uint32_t Cout 	= 0;
			uint32_t CColor = 0;	
			uint32_t qnt_idx = BLOCK_ANALYZER(header._LEVEL, img, m_WIDTH, m_HEIGHT, blcX, blcY, 3);
			uint32_t qnt 	= qnt_idx << 1;

			for (uint32_t y = 0; y < 16; ++y)
			{
				for (uint32_t x = 0; x < 16; ++x)
				{
					uint32_t column		= blcX + x;
					uint32_t row		= blcY + y;

					if (column >= m_WIDTH || row >= m_HEIGHT) {continue; }

					uint32_t index = 3 * (row * m_WIDTH + column);

					uint8_t Rc = img[index];
					uint8_t Gc = img[index+1];
					uint8_t Bc = img[index+2];

					if(qnt>0){
						Rc /= qnt;
						Gc /= qnt;
						Bc /= qnt;
					}

					GEN_CLR_MAP_RGB(l_ch0, l_ch1, l_ch2, CColor, l_idx, Cout, Rc, Gc, Bc);
					++Cout;
				}
			}

			const bool ch0_org	= IsOrgLine(m_ch0, l_ch0, CColor);
			const bool ch1_org	= IsOrgLine(m_ch1, l_ch1, CColor);
			const bool ch2_org	= IsOrgLine(m_ch2, l_ch2, CColor);
			const bool idx_org	= IsOrgLine(m_idx, l_idx, Cout);

			if(ch0_org||ch1_org||ch2_org){
				for (uint32_t i = 0; i < CColor; ++i) {
					if(ch0_org){m_ch0[i] = l_ch0[i];}
					if(ch1_org){m_ch1[i] = l_ch1[i];}
					if(ch2_org){m_ch2[i] = l_ch2[i];}
				}
				for (uint32_t i = CColor; i < 256; ++i) {
					m_ch0[i]=0;
					m_ch1[i]=0;
					m_ch2[i]=0;
				}
			}

			if(idx_org){
				for (uint32_t i = 0; i < Cout; ++i) {
					m_idx[i] = l_idx[i];
				}
				for (uint32_t i = Cout; i < 256; ++i) {
					m_idx[i] = 0;
				}
			}

			uint32_t ch0_c = 0;
			uint32_t ch1_c = 0;
			uint32_t ch2_c = 0;
			uint32_t idx_c = 0;

			const uint16_t v0 = ENCODE_REVOLVER(ch0_org, l_ch0, m_write, CColor, ch0_c);
			const uint16_t v1 = ENCODE_REVOLVER(ch1_org, l_ch1, m_write + ch0_c, CColor, ch1_c);
			const uint16_t v2 = ENCODE_REVOLVER(ch2_org, l_ch2, m_write + ch0_c + ch1_c, CColor, ch2_c);
			const uint16_t v4 = ENCODE_REVOLVER(idx_org, l_idx, m_write + ch0_c + ch1_c + ch2_c, Cout, idx_c);
			
			uint16_t meta_code = v0 * 1296u + v1 * 216u + v2 * 36u + v4;

    		meta_code = uint16_t((meta_code << 0x03u) | (qnt_idx & 0x07u));

			outfile.write(&meta_code, 1, 2);

			uint8_t cm_size = 0;

			if(ch0_org){ m_size[cm_size++]=uint8_t(ch0_c - 0x1u);}
			if(ch1_org){ m_size[cm_size++]=uint8_t(ch1_c - 0x1u);}
			if(ch2_org){ m_size[cm_size++]=uint8_t(ch2_c - 0x1u);}
			if(idx_org){ m_size[cm_size++]=uint8_t(idx_c - 0x1u);}

			outfile.write(m_size, 1, cm_size);
			outfile.write(m_write, 1, ch0_c + ch1_c + ch2_c + idx_c);
		}
	}

	return SLIMERROR::ERROR_OK;
}


SLIMERROR SLIM_WRITE_BLOCKS_4CHANNEL(MiniStream &outfile, SLIM_INFO &header, uint8_t* &img){

	uint32_t m_WIDTH = (uint32_t)header._WIDTH;
	uint32_t m_HEIGHT = (uint32_t)header._HEIGHT;

	uint8_t m_data		[1280]{0}; 	//Old		block memory
	uint8_t l_data		[1280]{0}; 	//Curret	block memory
	uint8_t m_write		[1280]{0}; 	//Curret	block packed
	uint8_t m_size		[5]{0};		//Size 		blocks packed

	//Pointers old block memory
	uint8_t* m_ch0 = m_data;
	uint8_t* m_ch1 = m_data + 256u;
	uint8_t* m_ch2 = m_data + 512u;
	uint8_t* m_ch3 = m_data + 768u;
	uint8_t* m_idx = m_data + 1024u;

	//Pointers curret block memory
	uint8_t* l_ch0 = l_data;
	uint8_t* l_ch1 = l_data + 256u;
	uint8_t* l_ch2 = l_data + 512u;
	uint8_t* l_ch3 = l_data + 768u;
	uint8_t* l_idx = l_data + 1024u;

	for (uint32_t blcY = 0; blcY < m_HEIGHT; blcY += 16)
	{
		for (uint32_t blcX = 0; blcX < m_WIDTH; blcX += 16)
		{
			uint32_t Cout = 0;
			uint32_t CColor = 0;
			uint32_t qnt_idx = BLOCK_ANALYZER(header._LEVEL, img, m_WIDTH, m_HEIGHT, blcX, blcY, 4);
			uint32_t qnt	= qnt_idx << 1;

			for (uint32_t y = 0; y < 16; ++y)
			{
				for (uint32_t x = 0; x < 16; ++x)
				{
					uint32_t column = blcX + x;
					uint32_t row = blcY + y;

					if (column>=m_WIDTH || row>= m_HEIGHT) { continue; }

					uint32_t index = 4 * (row * m_WIDTH + column);

					uint8_t Rc = img[index];
					uint8_t Gc = img[index + 1];
					uint8_t Bc = img[index + 2];
					uint8_t Ac = img[index + 3];

					if(Ac<1){Rc=0;Gc=0;Bc=0;}

					if(qnt>0){
						Rc /= qnt;
						Gc /= qnt;
						Bc /= qnt;
						Ac /= qnt;
					}

					GEN_CLR_MAP_RGBA(l_ch0, l_ch1, l_ch2, l_ch3, CColor, l_idx, Cout, Rc, Gc, Bc, Ac);
					++Cout;
				}
			}

			const bool ch0_org	= IsOrgLine(m_ch0, l_ch0, CColor);
			const bool ch1_org	= IsOrgLine(m_ch1, l_ch1, CColor);
			const bool ch2_org	= IsOrgLine(m_ch2, l_ch2, CColor);
			const bool ch3_org	= IsOrgLine(m_ch3, l_ch3, CColor);
			const bool idx_org	= IsOrgLine(m_idx, l_idx, Cout);

			if (ch0_org || ch1_org || ch2_org|| ch3_org) {
				for (uint32_t i = 0; i < CColor; ++i) {
					if (ch0_org) { m_ch0[i] = l_ch0[i]; }
					if (ch1_org) { m_ch1[i] = l_ch1[i]; }
					if (ch2_org) { m_ch2[i] = l_ch2[i]; }
					if (ch3_org) { m_ch3[i] = l_ch3[i]; }
				}
				for (uint32_t i = CColor; i < 256; ++i) {
					m_ch0[i] = 0;
					m_ch1[i] = 0;
					m_ch2[i] = 0;
					m_ch3[i] = 0;
				}
			}

			if (idx_org) {
				for (uint32_t i = 0; i < Cout; ++i) {
					m_idx[i] = l_idx[i];
				}
				for (uint32_t i = Cout; i < 256; ++i) {
					m_idx[i] = 0;
				}
			}

			uint32_t ch0_c = 0;
			uint32_t ch1_c = 0;
			uint32_t ch2_c = 0;
			uint32_t ch3_c = 0;
			uint32_t idx_c = 0;

			const uint16_t v0 = ENCODE_REVOLVER(ch0_org, l_ch0, m_write, CColor, ch0_c);
			const uint16_t v1 = ENCODE_REVOLVER(ch1_org, l_ch1, m_write + ch0_c, CColor, ch1_c);
			const uint16_t v2 = ENCODE_REVOLVER(ch2_org, l_ch2, m_write + ch0_c + ch1_c, CColor, ch2_c);
			const uint16_t v3 = ENCODE_REVOLVER(ch3_org, l_ch3, m_write + ch0_c + ch1_c + ch2_c, CColor, ch3_c);
			const uint16_t v4 = ENCODE_REVOLVER(idx_org, l_idx, m_write + ch0_c + ch1_c + ch2_c + ch3_c, Cout, idx_c);

			uint16_t meta_code = v0 * 1296u + v1 * 216u + v2 * 36u + v3 * 6u + v4;

			meta_code = uint16_t((meta_code << 0x03u) | (qnt_idx & 0x07u));

			outfile.write(&meta_code, 1, 2);

			uint8_t cm_size = 0;

			if(ch0_org){ m_size[cm_size++]=uint8_t(ch0_c - 0x1u);}
			if(ch1_org){ m_size[cm_size++]=uint8_t(ch1_c - 0x1u);}
			if(ch2_org){ m_size[cm_size++]=uint8_t(ch2_c - 0x1u);}
			if(ch3_org){ m_size[cm_size++]=uint8_t(ch3_c - 0x1u);}
			if(idx_org){ m_size[cm_size++]=uint8_t(idx_c - 0x1u);}

			outfile.write(m_size, 1, cm_size);
			outfile.write(m_write, 1, ch0_c + ch1_c + ch2_c + ch3_c + idx_c);
		}
	}
	return SLIMERROR::ERROR_OK;
}


SLIMERROR 	SLIM_READ_BLOCKS_3CHANNEL(MiniStream &infile, SLIM_INFO &header, uint8_t* &img) {

	const uint32_t m_WIDTH	= (uint32_t)header._WIDTH;
	const uint32_t m_HEIGHT = (uint32_t)header._HEIGHT;

	img = (uint8_t*)SLIM_MALLOC(m_WIDTH * m_HEIGHT * 3);

	uint8_t m_data		[1024]{0};	//Curret	block memory
	uint8_t m_read		[1024]{0};	//Read		block memory
	uint8_t m_size		[4]{0};		//Size 		blocks packed

	uint32_t qnt 		= 0;
	uint16_t meta_code	= 0;

	for (uint32_t blcY = 0; blcY < m_HEIGHT; blcY += 16)
	{
		for (uint32_t blcX = 0; blcX < m_WIDTH; blcX += 16)
		{
				
			if (!infile.read(&meta_code, 1, 2)){ return SLIMERROR::ERROR_END; }

			qnt = (meta_code & 0x07u) << 1;
			meta_code >>= 0x03u;

			uint32_t t;

			t = meta_code / 1296u;  uint16_t v0 = t;  meta_code -= t * 1296u;
			t = meta_code /  216u;  uint16_t v1 = t;  meta_code -= t *  216u;
			t = meta_code /   36u;  uint16_t v2 = t;  meta_code -= t *   36u;
			t = meta_code /    6u;  				  meta_code -= t *    6u;
			uint16_t v4 = meta_code;

			bool ch0_org	= (v0>0);
			bool ch1_org	= (v1>0);
			bool ch2_org	= (v2>0);
			bool idx_org	= (v4>0);

			uint8_t cm_size = ch0_org + ch1_org + ch2_org + idx_org;

			if (!infile.read(m_size, 1, cm_size)){ return SLIMERROR::ERROR_END; }

			uint8_t  pos 				= 0x0u;
			const uint32_t cmps_ch0 	= ch0_org ? 0x1u + m_size[pos++] : 0x0u;
			const uint32_t cmps_ch1 	= ch1_org ? 0x1u + m_size[pos++] : 0x0u;
			const uint32_t cmps_ch2 	= ch2_org ? 0x1u + m_size[pos++] : 0x0u;
			const uint32_t cmps_idx 	= idx_org ? 0x1u + m_size[pos++] : 0x0u;

			const uint32_t st_ch1  		= cmps_ch0;
			const uint32_t st_ch2  		= st_ch1 + cmps_ch1;
			const uint32_t st_idx  		= st_ch2 + cmps_ch2;
			const uint32_t st_size 		= st_idx + cmps_idx;

			if (!infile.read(m_read, 1, st_size)){ return SLIMERROR::ERROR_END; }
			
			DECODE_REVOLVER(v0, m_read, m_data, cmps_ch0);
			DECODE_REVOLVER(v1, m_read + st_ch1, m_data + 256, cmps_ch1);
			DECODE_REVOLVER(v2, m_read + st_ch2, m_data + 512, cmps_ch2);
			DECODE_REVOLVER(v4, m_read + st_idx, m_data + 768, cmps_idx);

			uint32_t idxclr		= 0;
			uint32_t Cout		= 0;

			for (uint32_t y = 0; y < 16; ++y)
			{
				for (uint32_t x = 0; x < 16; ++x)
				{

					uint32_t column		= blcX + x;
					uint32_t row		= blcY + y;

					if (column >= m_WIDTH || row >= m_HEIGHT) { continue; }

					uint32_t index = 3 * (row * m_WIDTH + column);

					idxclr = m_data[768 + Cout];

					uint8_t chn0	= m_data[idxclr];
					uint8_t chn1 	= m_data[idxclr + 256];
					uint8_t chn2 	= m_data[idxclr + 512];

					if (qnt > 0) {
						double pnl = perlin_noise_frame[Cout];
						double level = pnl * 1.7 + pnl * qnt * 0.7;

						uint32_t tchn0 = (uint32_t)(chn0*qnt + level);
						uint32_t tchn1 = (uint32_t)(chn1*qnt + level);
						uint32_t tchn2 = (uint32_t)(chn2*qnt + level);

    					chn0 = (uint8_t)(tchn0  > 255 ? 255 : tchn0);
						chn1 = (uint8_t)(tchn1  > 255 ? 255 : tchn1);
						chn2 = (uint8_t)(tchn2  > 255 ? 255 : tchn2);
					}

					img[index]		= chn0;
					img[index + 1] 	= chn1;
					img[index + 2] 	= chn2;

					++Cout;

				}
			}
		}
	}

	return SLIMERROR::ERROR_OK;
}





SLIMERROR SLIM_READ_BLOCKS_4CHANNEL(MiniStream &infile, SLIM_INFO &header, uint8_t* &img){

	const uint32_t m_WIDTH	= (uint32_t)header._WIDTH;
	const uint32_t m_HEIGHT = (uint32_t)header._HEIGHT;

	img = (uint8_t*)SLIM_MALLOC(m_WIDTH * m_HEIGHT * 4);

	uint8_t m_data		[1280]{0};	//Curret	block memory
	uint8_t m_read		[1280]{0};	//Read		block memory
	uint8_t m_size		[5]{0};		//Size 		blocks packed

	uint32_t qnt		= 0;
	uint16_t meta_code	= 0;

	for (uint32_t blcY = 0; blcY < m_HEIGHT; blcY += 16)
	{
		for (uint32_t blcX = 0; blcX < m_WIDTH; blcX += 16)
		{

			if (!infile.read(&meta_code, 1, 2)){ return SLIMERROR::ERROR_END; }

			qnt = (meta_code & 0x07u) << 1;
			meta_code >>= 0x03u;

			uint32_t t;

			t = meta_code / 1296u;  uint16_t v0 = t;  meta_code -= t * 1296u;
			t = meta_code /  216u;  uint16_t v1 = t;  meta_code -= t *  216u;
			t = meta_code /   36u;  uint16_t v2 = t;  meta_code -= t *   36u;
			t = meta_code /    6u;  uint16_t v3 = t;  meta_code -= t *    6u;
			uint16_t v4 = meta_code;

			bool ch0_org	= (v0>0);
			bool ch1_org	= (v1>0);
			bool ch2_org	= (v2>0);
			bool ch3_org	= (v3>0);
			bool idx_org	= (v4>0);

			uint8_t cm_size = ch0_org + ch1_org + ch2_org + ch3_org + idx_org;

			if (!infile.read(m_size, 1, cm_size)){ return SLIMERROR::ERROR_END; }

			uint8_t  cm_pos 			= 0x0u;
			const uint32_t cmps_ch0 	= ch0_org ? 0x1u + ((uint32_t)m_size[cm_pos++]) : 0x0u;
			const uint32_t cmps_ch1 	= ch1_org ? 0x1u + ((uint32_t)m_size[cm_pos++]) : 0x0u;
			const uint32_t cmps_ch2 	= ch2_org ? 0x1u + ((uint32_t)m_size[cm_pos++]) : 0x0u;
			const uint32_t cmps_ch3 	= ch3_org ? 0x1u + ((uint32_t)m_size[cm_pos++]) : 0x0u;
			const uint32_t cmps_idx 	= idx_org ? 0x1u + ((uint32_t)m_size[cm_pos++]) : 0x0u;

			const uint32_t st_ch1		= cmps_ch0;
			const uint32_t st_ch2		= st_ch1 + cmps_ch1;
			const uint32_t st_ch3		= st_ch2 + cmps_ch2;
			const uint32_t st_idx		= st_ch3 + cmps_ch3;
			const uint32_t st_size		= st_idx + cmps_idx;

			if (!infile.read(m_read, 1, st_size)){ return SLIMERROR::ERROR_END; }

			DECODE_REVOLVER(v0, m_read, m_data, cmps_ch0);
			DECODE_REVOLVER(v1, m_read + st_ch1, m_data + 256, cmps_ch1);
			DECODE_REVOLVER(v2, m_read + st_ch2, m_data + 512, cmps_ch2);
			DECODE_REVOLVER(v3, m_read + st_ch3, m_data + 768, cmps_ch3);
			DECODE_REVOLVER(v4, m_read + st_idx, m_data + 1024, cmps_idx);

			uint32_t idxclr		= 0;
			uint32_t Cout		= 0;

			for (uint32_t y = 0; y < 16; ++y)
			{
				for (uint32_t x = 0; x < 16; ++x)
				{

					uint32_t column		= blcX + x;
					uint32_t row		= blcY + y;

					if (column >= m_WIDTH || row >= m_HEIGHT) { continue; }

					uint32_t index	= 4 * (row * m_WIDTH + column);

					idxclr	= m_data[1024 + Cout];

					uint8_t chn0	= m_data[idxclr];
					uint8_t chn1 	= m_data[idxclr + 256];
					uint8_t chn2 	= m_data[idxclr + 512];
					uint8_t chn3 	= m_data[idxclr + 768];

					if (qnt > 0 && chn3 > 0) {
						double pnl = perlin_noise_frame[Cout];
						double level = pnl * 1.7 + pnl * qnt * 0.7;

						uint32_t tchn0 = (uint32_t)(chn0*qnt + level);
						uint32_t tchn1 = (uint32_t)(chn1*qnt + level);
						uint32_t tchn2 = (uint32_t)(chn2*qnt + level);
						uint32_t tchn3 = (uint32_t)(chn3*qnt + level);

    					chn0 = (uint8_t)(tchn0  > 255 ? 255 : tchn0);
						chn1 = (uint8_t)(tchn1  > 255 ? 255 : tchn1);
						chn2 = (uint8_t)(tchn2  > 255 ? 255 : tchn2);
						chn3 = (uint8_t)(tchn3  > 255 ? 255 : tchn3);
					}

					img[index]		= chn0;
					img[index + 1] 	= chn1;
					img[index + 2] 	= chn2;
					img[index + 3] 	= chn3;

					++Cout;
				}
			}
		}
	}


	return SLIMERROR::ERROR_OK;
}


SLIM_INFO Create_Info(uint16_t w, uint16_t h, uint8_t code, uint8_t filter, uint8_t level){

	SLIM_INFO tmp;
	tmp._VERS = SLIM_VER;
	tmp._WIDTH = w;
	tmp._HEIGHT = h;
	tmp._CODE = code;
	tmp._FILTER = filter;
	tmp._LEVEL = level;

	return tmp;
}


SLIMERROR Save_SLIM(MiniStream &outfile, SLIM_INFO &header, uint8_t* &img){

	if (!outfile.isOpen()){return SLIMERROR::ERROR_FILE;}
	if (img == NULL){return SLIMERROR::ERROR_ARG;}

	if (header._VERS != uint32_t(SLIM_VER)) 									{ return SLIMERROR::ERROR_NOTSUP; }
	if (header._WIDTH == 0 || header._HEIGHT == 0) 								{ return SLIMERROR::ERROR_BLOCK; }

	if (!outfile.write(MINI_SLIM_HEADER, 1,sizeof(MINI_SLIM_HEADER))){ return SLIMERROR::ERROR_BLOCK; }
	if (!outfile.write(reinterpret_cast<char*>(&header), 1, sizeof(SLIM_INFO))) { return SLIMERROR::ERROR_BLOCK; }

	SLIMERROR res =SLIMERROR::ERROR_OK;

	switch (header._CODE)
	{
	case SLIMCODE::CODE_RGB:
		res = SLIM_WRITE_BLOCKS_3CHANNEL(outfile, header, img);
		break;
	case SLIMCODE::CODE_RGBA:
		res = SLIM_WRITE_BLOCKS_4CHANNEL(outfile, header, img);
		break;
	default:
		return SLIMERROR::ERROR_BLOCK;
	}

	return res;
}


SLIMERROR Info_SLIM(MiniStream &infile, SLIM_INFO_FULL &info){

	if (!infile.isOpen()){return SLIMERROR::ERROR_FILE;}

	char m_buf[sizeof(MINI_SLIM_HEADER)] = {0};
	SLIM_INFO header;
	
	if (!infile.read(m_buf, 1, sizeof(MINI_SLIM_HEADER))) 						{ return SLIMERROR::ERROR_BLOCK; }

	if (strncmp(m_buf, MINI_SLIM_HEADER, sizeof(MINI_SLIM_HEADER))) 			{ return SLIMERROR::ERROR_NOTSUP; }

	if (!infile.read(reinterpret_cast<char*>(&header), 1, sizeof(SLIM_INFO))) 	{ return SLIMERROR::ERROR_BLOCK; }

	if (header._VERS != uint32_t(SLIM_VER)) 									{ return SLIMERROR::ERROR_NOTSUP; }
	if (header._WIDTH == 0 || header._HEIGHT == 0) 								{ return SLIMERROR::ERROR_BLOCK; }

	info._VERS 					= header._VERS;
	info._WIDTH 				= header._WIDTH;
	info._HEIGHT 				= header._HEIGHT;
	info._CODE 					= header._CODE;	
	info._FILTER 				= header._FILTER;
	info._LEVEL					= header._LEVEL;

	info._BLOCK_256_ALL 		= 0;
	info._BLOCK_256_EXIST		= 0;
	info._BLOCK_256_EMPTY		= 0;
	info._BLOCK_COLOR_TABLE_MAX = 0;
	info._BLOCK_COLOR_TABLE_MIN = 0xFFFFFFFFu;
	info._BLOCK_COLOR_TABLE_AVG = 0;

	info._BLOCK_Q_MAX		= 0;
	info._BLOCK_Q_MIN		= 0xFFFFFFFFu;
	info._BLOCK_Q_AVG		= 0;
	
	info._ALL_C					= 0;
	info._REUSE_C				= 0;
	info._ORIGINAL_C			= 0;
	info._RICE_C				= 0;
	info._RLE_C					= 0;
	info._SLDD_C				= 0;
	info._MASKARED_C			= 0;
	
	const uint32_t m_WIDTH	= (uint32_t)header._WIDTH;
	const uint32_t m_HEIGHT = (uint32_t)header._HEIGHT;

	uint8_t m_data		[1280]{0};	//Curret	block memory
	uint8_t m_read		[1280]{0};	//Read		block memory
	uint8_t m_size		[5]{0};		//Size 		blocks packed

	uint16_t comp_pack[5]{0,0,0,0,0};
	uint32_t qnt 		= 0;

	uint16_t meta_code	= 0;

	for (uint32_t blcY = 0; blcY < m_HEIGHT; blcY += 16)
	{
		for (uint32_t blcX = 0; blcX < m_WIDTH; blcX += 16)
		{

			if (!infile.read(&meta_code, 1, 2)){ return SLIMERROR::ERROR_END; }

			qnt = (meta_code & 0x07u)<< 1;
			meta_code >>= 0x03u;

			uint32_t t;

			t = meta_code / 1296;  comp_pack[0] = t;  meta_code -= t * 1296;
			t = meta_code /  216;  comp_pack[1] = t;  meta_code -= t *  216;
			t = meta_code /   36;  comp_pack[2] = t;  meta_code -= t *   36;
			t = meta_code /    6;  comp_pack[3] = t;  meta_code -= t *    6;
			comp_pack[4] = meta_code;

			bool ch0_org	= (comp_pack[0]>0);
			bool ch1_org	= (comp_pack[1]>0);
			bool ch2_org	= (comp_pack[2]>0);
			bool ch3_org	= (comp_pack[3]>0);
			bool idx_org	= (comp_pack[4]>0);

			for(uint8_t i=0; i<5; ++i){
				info._REUSE_C		+= (comp_pack[i]==0);
				info._ORIGINAL_C	+= (comp_pack[i]==1);
				info._RLE_C			+= (comp_pack[i]==2);
				info._RICE_C		+= (comp_pack[i]==3);
				info._SLDD_C		+= (comp_pack[i]==4);
				info._MASKARED_C	+= (comp_pack[i]==5);
			}

			if(ch0_org || ch1_org || ch2_org || ch3_org){
				if(info._BLOCK_Q_MAX<qnt){info._BLOCK_Q_MAX=qnt;}
				if(info._BLOCK_Q_MIN>qnt){info._BLOCK_Q_MIN=qnt;}
			}

			uint8_t cm_size = ch0_org + ch1_org + ch2_org + ch3_org + idx_org;

			info._BLOCK_256_ALL++;
			info._BLOCK_256_EXIST += (cm_size>0);
			info._BLOCK_256_EMPTY += (cm_size==0);
			info._BLOCK_Q_AVG += qnt;

			if (!infile.read(m_size, 1, cm_size)){ return SLIMERROR::ERROR_END; }

			uint8_t  cm_pos 			= 0x0u;
			const uint32_t cmps_ch0 	= ch0_org ? 0x1u + ((uint32_t)m_size[cm_pos++]) : 0x0u;
			const uint32_t cmps_ch1 	= ch1_org ? 0x1u + ((uint32_t)m_size[cm_pos++]) : 0x0u;
			const uint32_t cmps_ch2 	= ch2_org ? 0x1u + ((uint32_t)m_size[cm_pos++]) : 0x0u;
			const uint32_t cmps_ch3 	= ch3_org ? 0x1u + ((uint32_t)m_size[cm_pos++]) : 0x0u;
			const uint32_t cmps_idx 	= idx_org ? 0x1u + ((uint32_t)m_size[cm_pos++]) : 0x0u;

			const uint32_t st_ch1		= cmps_ch0;
			const uint32_t st_ch2		= st_ch1 + cmps_ch1;
			const uint32_t st_ch3		= st_ch2 + cmps_ch2;
			const uint32_t st_idx		= st_ch3 + cmps_ch3;
			const uint32_t st_size		= st_idx + cmps_idx;

			if (!infile.read(m_read, 1, st_size)){ return SLIMERROR::ERROR_END; }

			DECODE_REVOLVER(comp_pack[0], m_read, m_data, cmps_ch0);
			DECODE_REVOLVER(comp_pack[1], m_read + st_ch1, m_data + 256, cmps_ch1);
			DECODE_REVOLVER(comp_pack[2], m_read + st_ch2, m_data + 512, cmps_ch2);
			DECODE_REVOLVER(comp_pack[3], m_read + st_ch3, m_data + 768, cmps_ch3);
			DECODE_REVOLVER(comp_pack[4], m_read + st_idx, m_data + 1024, cmps_idx);

			uint32_t lc_blk_max = 0;
			uint32_t lc_blk_min = 0xFFFFFFFFu;
			if(idx_org){
				for(uint32_t idx = 0; idx<256; ++idx){
					uint32_t idxclr = m_data[1024 + idx]+1;
					if(info._BLOCK_COLOR_TABLE_MIN>idxclr){info._BLOCK_COLOR_TABLE_MIN=idxclr;}
					if(info._BLOCK_COLOR_TABLE_MAX<idxclr){info._BLOCK_COLOR_TABLE_MAX=idxclr;}			
					if(lc_blk_min>idxclr){lc_blk_min=idxclr;}
					if(lc_blk_max<idxclr){lc_blk_max=idxclr;}
				}
				info._BLOCK_COLOR_TABLE_AVG+=lc_blk_max;
			}
			
		}
	}
	info._ALL_C = info._REUSE_C + info._ORIGINAL_C + info._RLE_C + info._RICE_C + info._SLDD_C + info._MASKARED_C;
	info._BLOCK_Q_AVG /= info._BLOCK_256_ALL;
	info._BLOCK_COLOR_TABLE_AVG /= info._BLOCK_256_EXIST;

	return  SLIMERROR::ERROR_OK;
}


SLIMERROR Load_SLIM(MiniStream &infile, SLIM_INFO &header, uint8_t* &img){

	if (!infile.isOpen()){return SLIMERROR::ERROR_FILE;}

	char m_buf[sizeof(MINI_SLIM_HEADER)] = {0};
	
	if (!infile.read(m_buf, 1, sizeof(MINI_SLIM_HEADER))) { return SLIMERROR::ERROR_BLOCK; }

	if (strncmp(m_buf, MINI_SLIM_HEADER, sizeof(MINI_SLIM_HEADER))) { return SLIMERROR::ERROR_NOTSUP; }

	if (!infile.read(reinterpret_cast<char*>(&header), 1, sizeof(SLIM_INFO))) 	{ return SLIMERROR::ERROR_BLOCK; }


	if (header._VERS != uint32_t(SLIM_VER)) 									{ return SLIMERROR::ERROR_NOTSUP; }
	if (header._WIDTH == 0 || header._HEIGHT == 0) 								{ return SLIMERROR::ERROR_BLOCK; }

	SLIMERROR res = SLIMERROR::ERROR_OK;

	switch (header._CODE)
	{
	case SLIMCODE::CODE_RGB:
		res = SLIM_READ_BLOCKS_3CHANNEL(infile, header, img);
		break;
	case SLIMCODE::CODE_RGBA:
		res = SLIM_READ_BLOCKS_4CHANNEL(infile, header, img);
		break;
	default:
		return SLIMERROR::ERROR_BLOCK;
	}

	return res;
}



SLIMERROR Load_SLIM_Map(MiniStream &infile, SLIM_INFO &header, uint8_t* &img){

	if (!infile.isOpen()){return SLIMERROR::ERROR_FILE;}

	char m_buf[sizeof(MINI_SLIM_HEADER)] = {0};
	
	if (!infile.read(m_buf, 1, sizeof(MINI_SLIM_HEADER))) { return SLIMERROR::ERROR_BLOCK; }

	if (strncmp(m_buf, MINI_SLIM_HEADER, sizeof(MINI_SLIM_HEADER))) { return SLIMERROR::ERROR_NOTSUP; }

	if (!infile.read(reinterpret_cast<char*>(&header), 1, sizeof(SLIM_INFO))) 	{ return SLIMERROR::ERROR_BLOCK; }

	if (header._VERS != uint32_t(SLIM_VER)) 									{ return SLIMERROR::ERROR_NOTSUP; }
	if (header._WIDTH == 0 || header._HEIGHT == 0) 								{ return SLIMERROR::ERROR_BLOCK; }

	header._CODE = SLIMCODE::CODE_MAP;

	const uint32_t m_WIDTH = header._WIDTH;
	const uint32_t m_HEIGHT = header._HEIGHT;

	img = (uint8_t*)SLIM_MALLOC(m_WIDTH * m_HEIGHT);

	uint8_t m_size		[5]{0};


	uint32_t qnt_idx 	= 0;
	uint16_t meta_code	= 0;

	for (uint32_t blcY = 0; blcY < m_HEIGHT; blcY += 16)
	{
		for (uint32_t blcX = 0; blcX < m_WIDTH; blcX += 16)
		{
			if (!infile.read(&meta_code, 1, 2)){ return SLIMERROR::ERROR_END; }

			qnt_idx  = (meta_code & 0x07u);
			meta_code >>= 0x03u;

			uint32_t t;

			t = meta_code / 1296u;  uint16_t v0 = t;  meta_code -= t * 1296u;
			t = meta_code /  216u;  uint16_t v1 = t;  meta_code -= t *  216u;
			t = meta_code /   36u;  uint16_t v2 = t;  meta_code -= t *   36u;
			t = meta_code /    6u;  uint16_t v3 = t;  meta_code -= t *    6u;
			uint16_t v4 = meta_code;

			bool ch0_org	= (v0>0);
			bool ch1_org	= (v1>0);
			bool ch2_org	= (v2>0);
			bool ch3_org	= (v3>0);
			bool idx_org	= (v4>0);

			uint8_t cm_size = ch0_org + ch1_org + ch2_org + ch3_org + idx_org;

			if (!infile.read(m_size, 1, cm_size)){ return SLIMERROR::ERROR_END; }

			uint8_t  cm_pos 			= 0u;
			const uint32_t cmps_ch0 	= ch0_org ? 1 + ((uint32_t)m_size[cm_pos++]) : 0u;
			const uint32_t cmps_ch1 	= ch1_org ? 1 + ((uint32_t)m_size[cm_pos++]) : 0u;
			const uint32_t cmps_ch2 	= ch2_org ? 1 + ((uint32_t)m_size[cm_pos++]) : 0u;
			const uint32_t cmps_ch3 	= ch3_org ? 1 + ((uint32_t)m_size[cm_pos++]) : 0u;
			const uint32_t cmps_idx 	= idx_org ? 1 + ((uint32_t)m_size[cm_pos++]) : 0u;

			const uint32_t st_size		= cmps_ch0 + cmps_ch1 + cmps_ch2 + cmps_ch3 + cmps_idx;

			if (!infile.seek(st_size,MiniStream::Cur)){ return SLIMERROR::ERROR_END; }

			for (uint32_t y = 0; y < 16; ++y)
			{
				for (uint32_t x = 0; x < 16; ++x)
				{

					uint32_t column		= blcX + x;
					uint32_t row		= blcY + y;

					if (column >= m_WIDTH || row >= m_HEIGHT) { continue; }

					uint32_t index	= (row * m_WIDTH + column);
					img[index]		= qnt_idx;

				}
			}
		}
	}


	return SLIMERROR::ERROR_OK;
}



#endif // miniSLIM_H