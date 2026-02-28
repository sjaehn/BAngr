#ifndef RTRAND_HPP_
#define RTRAND_HPP_


#include <cstddef>
#include <cstdint>
#include <ctime>

/**
Minimal implementation of a realtime-safe random class
derived from PCG random by 2014 M.E. O'Neill (Apache License 2.0)
with some adaptions by 2021 Robin Gareus for Airwindows (MIT License).
 */
class RTRand 
{
public:
	inline RTRand ();

    /**
    Gets a random value in the range [0.0, 1.0]. Thus, it only makes sense for
    floating point types.
    @tparam T   Floating point type.
    @return     Random value in the range [0.0, 1.0].
     */
	template<class T = float>
	T rand ();

    /**
    Gets a random value in the range [from, to].
    @tparam T   Numeric type.
    @param from Lower limit.
    @param to   Upper limit.
    @return     Random value in the range [from, to].
    */
    template<class T = float>
    T rand_range(const T from, const T to);

private:
	uint64_t __state;
	uint64_t __inc;

    uint32_t __rand_u32 ();
};

inline RTRand:: RTRand() :
    __state(0),
    __inc(0)
{
    int foo = 0;
		uint64_t initseq = (intptr_t)&foo;
		__inc = (initseq << 1) | 1;
		__rand_u32 ();
		__state += time (NULL) ^ (intptr_t)this;
		__rand_u32 ();
}

template<class T>
inline T RTRand::rand () {return __rand_u32 () / static_cast<T>(4294967295.0);}

template<class T>
inline T RTRand::rand_range(const T from, const T to)
{
    return from + (from != to) * __rand_u32() / static_cast<T>(4294967295.0 / (to - from));
}

inline uint32_t RTRand::__rand_u32 ()
{
    uint64_t ostate = __state;
    __state = ostate * 6364136223846793005ULL + __inc;
    uint32_t xsh = ((ostate >> 18u) ^ ostate) >> 27u;
    uint32_t rot = ostate >> 59u;
    return (xsh >> rot) | (xsh << ((-rot) & 31));
}

#endif /* RTRAND_HPP_ */