#pragma once
#include <cassert>
#include <cmath>
#include <memory>

#include "FastNoise/FastNoise_Config.h"
#include "FastNoise/FastNoiseMetadata.h"

namespace FastNoise
{
    struct OutputMinMax
    {
        float min = INFINITY;
        float max = -INFINITY;

        OutputMinMax& operator <<( float v )
        {
            min = fminf( min, v );
            max = fmaxf( max, v );
            return *this;
        }

        OutputMinMax& operator <<( const OutputMinMax& v )
        {
            min = fminf( min, v.min );
            max = fmaxf( max, v.max );
            return *this;
        }
    };

    template<typename T>
    struct BaseSource
    {
        std::shared_ptr<T> base;
        void* simdGeneratorPtr = nullptr;

    protected:
        BaseSource() = default;
    };

    template<typename T>
    struct GeneratorSourceT : BaseSource<T>
    { };

    template<typename T>
    struct HybridSourceT : BaseSource<T>
    {
        float constant;

        HybridSourceT( float f = 0.0f )
        {
            constant = f;
        }
    };

    class Generator
    {
    public:
        using Metadata = FastNoise::Metadata;

        virtual ~Generator() = default;

        virtual FastSIMD::eLevel GetSIMDLevel() const = 0;

        virtual OutputMinMax GenUniformGrid2D( float* noiseOut,
            int32_t xStart, int32_t yStart,
            int32_t xSize, int32_t ySize,
            float frequency, int32_t seed ) const = 0;

        virtual OutputMinMax GenUniformGrid3D( float* noiseOut,
            int32_t xStart, int32_t yStart, int32_t zStart,
            int32_t xSize,  int32_t ySize,  int32_t zSize,
            float frequency, int32_t seed ) const = 0;

        virtual OutputMinMax GenPositionArray2D( float* noiseOut, int32_t count,
            const float* xPosArray, const float* yPosArray,
            float xOffset, float yOffset, int32_t seed ) const = 0;     

        virtual OutputMinMax GenPositionArray3D( float* noiseOut, int32_t count,
            const float* xPosArray, const float* yPosArray, const float* zPosArray, 
            float xOffset, float yOffset, float zOffset, int32_t seed ) const = 0;     

        virtual const Metadata* GetMetadata() = 0;

    protected:
        template<typename T>
        void SetSourceMemberVariable( BaseSource<T>& memberVariable, const std::shared_ptr<T>& gen )
        {
            static_assert( std::is_base_of_v<Generator, T> );
            assert( gen.get() );
            assert( GetSIMDLevel() == gen->GetSIMDLevel() );

            memberVariable.base = gen;
            SetSourceSIMDPtr( dynamic_cast<Generator*>( gen.get() ), &memberVariable.simdGeneratorPtr );
        }

    private:
        virtual void SetSourceSIMDPtr( Generator* base, void** simdPtr ) = 0;
    };

    using GeneratorSource = GeneratorSourceT<Generator>;
    using HybridSource = HybridSourceT<Generator>;

}
