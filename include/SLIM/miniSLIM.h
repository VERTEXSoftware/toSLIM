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
0.434186, 0.421389, 0.356421, 0.452998, 0.341406, 0.487192, 0.577521, 0.375887, 0.4, 0.424113, 0.360083, 0.455493, 0.341406, 0.452998, 0.345184, 0.205798, 
0.421389, 0.509142, 0.415596, 0.395662, 0.338943, 0.506495, 0.567905, 0.354593, 0.4, 0.445407, 0.328628, 0.422086, 0.407798, 0.433682, 0.411449, 0.482793, 
0.356421, 0.547848, 0.409657, 0.20771, 0.236714, 0.447864, 0.597114, 0.419273, 0.4, 0.380727, 0.181109, 0.271519, 0.413134, 0.305125, 0.411056, 0.604996, 
0.452998, 0.647199, 0.507814, 0.336639, 0.453324, 0.598683, 0.484414, 0.224561, 0.4, 0.575439, 0.347234, 0.370634, 0.53302, 0.518083, 0.584225, 0.527838, 
0.341406, 0.535608, 0.396222, 0.310434, 0.542177, 0.468208, 0.399895, 0.25572, 0.4, 0.54428, 0.293838, 0.262958, 0.47424, 0.619209, 0.483434, 0.411282, 
0.487192, 0.658706, 0.510269, 0.37681, 0.386171, 0.262728, 0.445538, 0.433415, 0.38415, 0.334885, 0.185159, 0.362887, 0.561426, 0.490493, 0.271519, 0.422086, 
0.577521, 0.531962, 0.410245, 0.282316, 0.295317, 0.150722, 0.204533, 0.370618, 0.291281, 0.211944, 0.240426, 0.289156, 0.466542, 0.39925, 0.181109, 0.328628, 
0.374224, 0.371994, 0.466891, 0.371115, 0.482706, 0.33699, 0.247074, 0.448114, 0.423669, 0.399996, 0.46403, 0.36862, 0.544816, 0.576114, 0.38056, 0.446526, 
0.2, 0.255417, 0.440369, 0.347002, 0.458594, 0.328658, 0.331198, 0.399168, 0.3, 0.375055, 0.439917, 0.344507, 0.520703, 0.552001, 0.359631, 0.544583, 
0.374224, 0.353474, 0.41944, 0.32289, 0.434481, 0.320325, 0.415322, 0.350222, 0.176331, 0.350114, 0.415804, 0.320395, 0.49659, 0.527888, 0.333109, 0.428006, 
0.439917, 0.471372, 0.618891, 0.499754, 0.512755, 0.391135, 0.579568, 0.551897, 0.36376, 0.428176, 0.495733, 0.545862, 0.64888, 0.702275, 0.528242, 0.380214, 
0.344507, 0.377914, 0.528481, 0.40851, 0.417871, 0.295974, 0.487842, 0.462682, 0.406096, 0.366504, 0.347752, 0.530314, 0.437342, 0.545018, 0.440814, 0.401165, 
0.458594, 0.350461, 0.315569, 0.279794, 0.505057, 0.394358, 0.368558, 0.25572, 0.4, 0.54428, 0.41435, 0.346562, 0.227133, 0.383224, 0.298545, 0.412662, 
0.347002, 0.145888, 0.212484, 0.38092, 0.446277, 0.286682, 0.315162, 0.224561, 0.4, 0.575439, 0.42842, 0.318318, 0.210626, 0.469094, 0.398503, 0.232314, 
0.443579, 0.252152, 0.390343, 0.594312, 0.573358, 0.399323, 0.494733, 0.430485, 0.405618, 0.380752, 0.343583, 0.50551, 0.383876, 0.414626, 0.293708, 0.217826, 
0.378611, 0.290858, 0.384404, 0.443146, 0.65433, 0.538709, 0.634118, 0.569706, 0.507795, 0.445885, 0.429031, 0.641266, 0.457144, 0.202438, 0.221287, 0.35729
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
						uint32_t tchn0 = (uint32_t)(chn0 * qnt + ((double)qnt * pnl));
						uint32_t tchn1 = (uint32_t)(chn1 * qnt + ((double)qnt * pnl));
						uint32_t tchn2 = (uint32_t)(chn2 * qnt + ((double)qnt * pnl));

    					chn0 =  (uint8_t)(tchn0 > 255 ? 255 : tchn0);
						chn1 =  (uint8_t)(tchn1 > 255 ? 255 : tchn1);
						chn2 =  (uint8_t)(tchn2 > 255 ? 255 : tchn2);
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
						uint32_t tchn0 = (uint32_t)(chn0 * qnt + ((double)qnt * pnl));
						uint32_t tchn1 = (uint32_t)(chn1 * qnt + ((double)qnt * pnl));
						uint32_t tchn2 = (uint32_t)(chn2 * qnt + ((double)qnt * pnl));
						uint32_t tchn3 = (uint32_t)(chn3 * qnt + ((double)qnt * pnl));

    					chn0 =  (uint8_t)(tchn0 > 255 ? 255 : tchn0);
						chn1 =  (uint8_t)(tchn1 > 255 ? 255 : tchn1);
						chn2 =  (uint8_t)(tchn2 > 255 ? 255 : tchn2);
						chn3 =  (uint8_t)(tchn3 > 255 ? 255 : tchn3);
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