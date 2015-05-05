// round up to nearest factor of 2 for better alignment
#define BS_NUM_ELEM (size_t)(numBits / (sizeof(storageType) * 8) + numBits / (sizeof(storageType) * 8) % 2)
     
/*
 * Arbitrary-size aligned bit set. No easier/cleaner way to do this if I can't use std::bitset or Boost?
 * Has manual control over which type you want to use for storage if you want to reduce the number of redundant bits.
 *
 * Originally posted on http://pastebin.com/nuFDD91w
 *
 */
template<typename storageType, size_t numBits> class BitSet
{
public:
    BitSet()
    {
        ClearAll();
    }
     
    void ClearAll()
    {
        for (size_t i = 0; i < BS_NUM_ELEM; ++i)
        {
            m_bits[i] = 0;
        }
    }
     
    // set bit value
    void Set(size_t bitNo)
    {
        for (size_t i = 0; i < BS_NUM_ELEM; ++i)
        {
            if (bitNo < (i + 1) * sizeof(storageType) * 8)
            {
                m_bits[i] |= ((storageType)0x01 << (bitNo - i * sizeof(storageType) * 8));
                break;
            }
        }
    }
     
    // clear bit value
    void Clear(size_t bitNo)
    {
        for (size_t i = 0; i < BS_NUM_ELEM; ++i)
        {
            if (bitNo < (i + 1) * sizeof(storageType) * 8)
            {
                m_bits[i] &= ~((storageType)0x01 << (bitNo - i * sizeof(storageType) * 8));
                break;
            }
        }
    }
     
    // access bit value
    bool operator[](size_t bitNo)
    {
        for (size_t i = 0; i < BS_NUM_ELEM; ++i)
        {
            if (bitNo < (i + 1) * sizeof(storageType) * 8)
            {
                return ((m_bits[i] >> (bitNo - i * sizeof(storageType) * 8)) & 0x01) && 0x01;
            }
        }
               
        return false;
    }
     
private:
    storageType m_bits[BS_NUM_ELEM];
};
     

 /*
    // Usage
     
    BitSet<unsigned long long, 104> bv;  // create an "aligned" 128 bit set stored in two ULL variables.
     
    printf("%d", sizeof(bv));  // 16 bytes/128 bits
     
    bv.Set(24);  // set bit 24
    bv.Set(103);  // set bit 103
    ...
      
*/
     

