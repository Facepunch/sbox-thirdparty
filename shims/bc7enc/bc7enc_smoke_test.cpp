#include "bc7e_ispc.h"
#include "bc7decomp.h"

#include <array>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

extern "C" bool InitalizeModule();
extern "C" void GetProfile_BC7( int, ispc::bc7e_compress_block_params *, bool );
extern "C" void CompressBlocks( uint32_t, uint64_t *, const uint32_t *, const ispc::bc7e_compress_block_params * );

int main()
{
	if ( !InitalizeModule() ) return 1;

	// An odd block count exercises partially occupied SIMD batches. Include solid RGB,
	// solid RGBA, transparent white, gradients and noise, for every exposed profile.
	constexpr int blockCount = 257;
	std::array<uint32_t, blockCount * 16> pixels;
	uint32_t random = 0x12345678;
	for ( int i = 0; i < blockCount * 16; ++i )
	{
		random = random * 1664525u + 1013904223u;
		pixels[i] = i < 16 ? 0xFF000000u : i < 32 ? 0xFFFFFFFFu : i < 48 ? 0x80808080u
			: i < 64 ? 0x00FFFFFFu : i < 80 ? 0xD3955325u : i < 96 ? 0xFF03807Fu
			: i < 112 ? 0xFF0000FFu : i < 128 ? (uint32_t)( i * 2 ) * 0x01010101u : random;
		if ( i >= 128 && ( i / 16 ) % 2 ) pixels[i] |= 0xFF000000u;
	}

	constexpr uint64_t guard = 0xDEADBEEFCAFEBABEull;
	for ( int profile = 0; profile <= 6; ++profile )
	{
		for ( bool perceptual : { false, true } )
		{
			ispc::bc7e_compress_block_params params{};
			GetProfile_BC7( profile, &params, perceptual );
			std::array<uint64_t, blockCount * 2 + 4> batch, repeat;
			batch.fill( guard );
			repeat.fill( guard );
			CompressBlocks( blockCount, batch.data() + 2, pixels.data(), &params );
			CompressBlocks( blockCount, repeat.data() + 2, pixels.data(), &params );
			if ( batch != repeat || batch.front() != guard || batch[1] != guard
				|| batch[blockCount * 2 + 2] != guard || batch.back() != guard )
			{
				fprintf( stderr, "BC7E profile %d: nondeterministic output or output overrun\n", profile );
				return 1;
			}

			for ( int block = 0; block < 7; ++block )
			{
				bc7decomp::color_rgba decoded[16];
				if ( !bc7decomp::unpack_bc7( batch.data() + 2 + block * 2, decoded )
					|| memcmp( decoded, pixels.data() + block * 16, sizeof( decoded ) ) )
				{
					fprintf( stderr, "BC7E profile %d, block %d: solid-color decoding is not exact\n", profile, block );
					return 1;
				}
			}

			for ( int block = 0; block < blockCount; ++block )
			{
				uint64_t single[2]{};
				CompressBlocks( 1, single, pixels.data() + block * 16, &params );
				if ( !( single[0] & 0xFF ) || memcmp( single, batch.data() + 2 + block * 2, sizeof( single ) ) )
				{
					fprintf( stderr, "BC7E profile %d, block %d: invalid mode or batch-dependent output\n", profile, block );
					return 1;
				}
			}
		}
	}

	puts( "BC7E: all profiles passed repeatability, batching, exact solid colors and output bounds checks" );
	return 0;
}
