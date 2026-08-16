// Reconstruction of the shim behind bc7enc.dll, whose original source is not in the sbox
// repo. sbox loads the module by name and resolves exactly three entry points from
// src/bitmap/ispc_compressor_loader.cpp:
//
//   InitalizeModule   (the misspelling is load bearing - it is what sbox looks up)
//   GetProfile_BC7
//   CompressBlocks
//
// They forward to BinomialLLC/bc7e's ISPC kernel. The profile values match
// BC7_CompressionProfile_t in src/bitmap/ispc_compressor_loader.h, and
// bc7e_compress_block_params is declared there field for field, so the struct crosses the
// boundary by layout - do not reorder it on either side.

#include "bc7e_ispc.h"

#include <stdint.h>

#if defined( _WIN32 )
	#define BC7ENC_EXPORT extern "C" __declspec( dllexport )
#else
	#define BC7ENC_EXPORT extern "C" __attribute__( ( visibility( "default" ) ) )
#endif

// Mirrors BC7_CompressionProfile_t.
enum BC7ProfileLevel
{
	BC7_UltraFast = 0,
	BC7_VeryFast = 1,
	BC7_Fast = 2,
	BC7_Basic = 3,
	BC7_Slow = 4,
	BC7_VerySlow = 5,
	BC7_Slowest = 6,
};

BC7ENC_EXPORT bool InitalizeModule()
{
	ispc::bc7e_compress_block_init();
	return true;
}

BC7ENC_EXPORT void GetProfile_BC7( int nProfile, ispc::bc7e_compress_block_params *pParams, bool bPerceptual )
{
	switch ( nProfile )
	{
	case BC7_UltraFast:	ispc::bc7e_compress_block_params_init_ultrafast( pParams, bPerceptual ); break;
	case BC7_VeryFast:	ispc::bc7e_compress_block_params_init_veryfast( pParams, bPerceptual ); break;
	case BC7_Fast:		ispc::bc7e_compress_block_params_init_fast( pParams, bPerceptual ); break;
	case BC7_Slow:		ispc::bc7e_compress_block_params_init_slow( pParams, bPerceptual ); break;
	case BC7_VerySlow:	ispc::bc7e_compress_block_params_init_veryslow( pParams, bPerceptual ); break;
	case BC7_Slowest:	ispc::bc7e_compress_block_params_init_slowest( pParams, bPerceptual ); break;
	case BC7_Basic:
	default:			ispc::bc7e_compress_block_params_init_basic( pParams, bPerceptual ); break;
	}
}

BC7ENC_EXPORT void CompressBlocks( uint32_t nNumBlocks, uint64_t *pBlocks, const uint32_t *pPixelsRGBA,
	const ispc::bc7e_compress_block_params *pCompParams )
{
	ispc::bc7e_compress_blocks( nNumBlocks, pBlocks, pPixelsRGBA, pCompParams );
}
