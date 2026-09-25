// ┌──────────────────────────────────┐
// │            __          _   _  _  │
// │ |\/|  /\  (_  |/  /\  |_) |_ | \ │
// │ |  | /--\ __) |\ /--\ | \ |_ |_/ │
// │ Sleptsov compression algorithm   │
// └──────────────────────────────────┘
// SPDX-License-Identifier: MIT
// MASKARED compression algorithm for C/C++
// Version: 1.4.0.0
// Copyright (C) 2026 Sleptsov Vladimir 
// https://github.com/VERTEXSoftware


#ifndef SLEP_MASKARED_H
#define SLEP_MASKARED_H

#define MASKARED_VER_MAJOR  1
#define MASKARED_VER_MINOR  4
#define MASKARED_VER_BUGFIX 0
#define MASKARED_VER_HOTFIX 0

#define MASKARED_VER ((MASKARED_VER_MAJOR << 24) | (MASKARED_VER_MINOR << 16) | (MASKARED_VER_BUGFIX << 8) | (MASKARED_VER_HOTFIX))

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <cstdint>

	typedef enum {
		SL_OK 					= 0,
		SL_ERROR_INVALID_PARAM 	= 1,
		SL_ERROR_DATA 			= 2

	} MASKARED_RESULT;

	extern uint32_t        MASKARED_VERSION		    ();

	extern MASKARED_RESULT MASKARED_SIZE_CALC		(uint8_t* buf, uint32_t size, uint32_t* sizec, uint8_t mask = 0x00u);

	extern MASKARED_RESULT MASKARED_ENCODE		    (uint8_t* buf, uint32_t size, uint8_t* bufc, uint32_t* sizec);
	extern MASKARED_RESULT MASKARED_DECODE		    (uint8_t* buf, uint32_t size, uint8_t* bufd, uint32_t  sized);


#ifdef __cplusplus
}
#endif

#ifdef SLEP_MASKARED_IMP

uint32_t MASKARED_VERSION(){ return MASKARED_VER; }

#ifdef SLEP_MASKARED_EXP

MASKARED_RESULT MASKARED_ENCODE(uint8_t* buf, uint32_t size, uint8_t* bufc, uint32_t* sizec) {

    if (buf == NULL || bufc == NULL  || size == 0) { return MASKARED_RESULT::SL_ERROR_INVALID_PARAM; }

	uint8_t* pstr		= bufc + 0x02u;
	const uint8_t* end	= buf + size;
	uint8_t mask		= 255u;
	uint8_t accum		= 0x00u;
	uint32_t step		= 0x00u;
	uint8_t mover		= 0x00u;
	uint8_t first 		= 0x00u;

	for (uint16_t mv = 0u; mv < 256u && mask != 0x0u; ++mv) {
		uint8_t firstt 		= uint8_t(*buf + mv);
		uint8_t maskt		= 0x00u;

		for (uint8_t* p = buf+1u; p < end && maskt != 0xFFu; ++p) {
			maskt |= (firstt ^ uint8_t(*p+mv));
		}

		if(mask > maskt){
			mover 	= uint8_t(mv);
			mask 	= maskt;
			first 	= firstt;
		}
	}		

	for (uint8_t bit = 0x80u; bit > 0x00u; bit >>= 0x01u) {
		if (!(mask & bit)) {
			if (first & bit) {
				accum |= (0x80u >> step);
			}
			++step;
		}
	}

	uint32_t total = size * (0x08u - step);
	*sizec = (step + total + 23u) >> 0x03u;

	*bufc = mover;
	*(bufc+1) = mask;
	*pstr = accum;

	for (uint8_t *c, *p = buf; p < end; ++p) {
		const uint8_t ps = uint8_t(*p+mover);
		for (uint8_t bit = 0x80u; bit > 0x00u; bit >>= 0x01u) {
			if (mask & bit) {
				if (ps & bit) {
					c = pstr + (step >> 0x03u);
					*c |= (0x80u >> (step & 0x07u));
				}
				++step;
			}
		}
	}

	//Tails remover
    uint32_t pos_tail = *sizec;
    while (pos_tail > 1 && bufc[pos_tail - 1] == 0) {
        --pos_tail;
    }
    *sizec = pos_tail;

    return MASKARED_RESULT::SL_OK;
}


MASKARED_RESULT MASKARED_DECODE(uint8_t* buf, uint32_t size, uint8_t* bufd, uint32_t sized) {

    if (buf == NULL || bufd == NULL  || size == 0 || sized == 0 ) { return MASKARED_RESULT::SL_ERROR_INVALID_PARAM; }

	uint8_t* pstr		= buf + 0x02u;
	const uint8_t mask	= *(buf+1);
	const uint8_t mover	= *buf;
	const uint8_t accum = *pstr;
	const uint8_t* endd = bufd + sized;
	const uint8_t* endp = buf + size;
	uint8_t  chr		= 0x00u;
	uint32_t step		= 0x00u;

	for (uint8_t bit = 0x80u; bit > 0x00u; bit >>= 0x01u) {
		if (!(mask & bit)) {
			if ((accum << step) & 0x80u) {
				chr |= bit;
			}
			++step;
		}
	}

	for (uint8_t* d = bufd, *p = pstr; d < endd; ++d) {
		*d = chr;
		for (uint8_t bit = 0x80u; bit > 0x00u && p < endp; bit >>= 0x01u) {
			if (mask & bit) {
				if (*p & (0x80u >> (step & 0x07u))) { *d |= bit; }
				p = pstr + (++step >> 0x03u);
			}
		}
		*d-=mover;
	}

    return MASKARED_RESULT::SL_OK;
}

#else

MASKARED_RESULT MASKARED_SIZE_CALC(uint8_t* buf, uint32_t size, uint32_t* sizec, uint8_t mask) {

	if (buf == NULL || size == 0) { return MASKARED_RESULT::SL_ERROR_INVALID_PARAM; }

	const uint8_t first = *buf;
	const uint8_t* end	= buf + size;
	uint8_t accum		= 0x00u;
	uint32_t step		= 0x00u;

	for (uint8_t* p = buf; p < end && mask != 0xFFu; ++p) {
		mask |= (first ^ *p);
	}

	for (uint8_t bit = 0x80u; bit > 0x00u; bit >>= 0x01u) {
		if (!(mask & bit)) {
			if (first & bit) {
				accum |= (0x80u >> step);
			}
			++step;
		}
	}

	uint32_t total = size * (0x08u - step);
	*sizec = (step + total + 0x0Fu) >> 0x03u;

	return MASKARED_RESULT::SL_OK;
}



MASKARED_RESULT MASKARED_ENCODE(uint8_t* buf, uint32_t size, uint8_t* bufc, uint32_t* sizec) {

    if (buf == NULL || bufc == NULL  || size == 0) { return MASKARED_RESULT::SL_ERROR_INVALID_PARAM; }

	uint8_t* pstr		= bufc + 0x01u;
	const uint8_t first = *buf;
	const uint8_t* end	= buf + size;
	uint8_t mask		= 0x00u;
	uint8_t accum		= 0x00u;
	uint32_t step		= 0x00u;

	for (uint8_t* p = buf; p < end && mask != 0xFFu; ++p) {
		mask |= (first ^ *p);
	}

	for (uint8_t bit = 0x80u; bit > 0x00u; bit >>= 0x01u) {
		if (!(mask & bit)) {
			if (first & bit) {
				accum |= (0x80u >> step);
			}
			++step;
		}
	}

	uint32_t total = size * (0x08u - step);
	*sizec = (step + total + 0x0Fu) >> 0x03u;

	*bufc = mask;
	*pstr = accum;

	for (uint8_t *c, *p = buf; p < end; ++p) {
		for (uint8_t bit = 0x80u; bit > 0x00u; bit >>= 0x01u) {
			if (mask & bit) {
				if (*p & bit) {
					c = pstr + (step >> 0x03u);
					*c |= (0x80u >> (step & 0x07u));
				}
				++step;
			}
		}
	}

	//Tails remover
    uint32_t pos_tail = *sizec;
    while (pos_tail > 1 && bufc[pos_tail - 1] == 0) {
        --pos_tail;
    }
    *sizec = pos_tail;

    return MASKARED_RESULT::SL_OK;
}


MASKARED_RESULT MASKARED_DECODE(uint8_t* buf, uint32_t size, uint8_t* bufd, uint32_t sized) {

    if (buf == NULL || bufd == NULL  || size == 0 || sized == 0 ) { return MASKARED_RESULT::SL_ERROR_INVALID_PARAM; }

	uint8_t* pstr		= buf + 0x01u;
	const uint8_t mask	= *buf;
	const uint8_t accum = *pstr;
	const uint8_t* endd = bufd + sized;
	const uint8_t* endp = buf + size;
	uint8_t  chr		= 0x00u;
	uint32_t step		= 0x00u;

	for (uint8_t bit = 0x80u; bit > 0x00u; bit >>= 0x01u) {
		if (!(mask & bit)) {
			if ((accum << step) & 0x80u) {
				chr |= bit;
			}
			++step;
		}
	}

	for (uint8_t* d = bufd, *p = pstr; d < endd; ++d) {
		*d = chr;
		for (uint8_t bit = 0x80u; bit > 0x00u && p < endp; bit >>= 0x01u) {
			if (mask & bit) {
				if (*p & (0x80u >> (step & 0x07u))) { *d |= bit; }
				p = pstr + (++step >> 0x03u);
			}
		}
	}

    return MASKARED_RESULT::SL_OK;
}

#endif // SLEP_MASKARED_EXP
#endif // SLEP_MASKARED_IMP 
#endif // SLEP_MASKARED_H