#ifndef __TEACRYPT_H__
#define __TEACRYPT_H__

#include "databuffer.h"

namespace sol 
{
    namespace crypt
    {
        class CTea
        {
            public:
                CTea()
                {
                    _key[0] = 0x11;
                    _key[1] = 0x22;
                    _key[2] = 0x33;
                    _key[3] = 0x44;
                    _round = 16;
                }

                virtual ~CTea()
                {
                }

                void SetKey(uint32_t key[4]);
                bool Encrypt(const void *input, int input_len, CDataBuffer &out);
                bool Decrypt(const void *input, int input_len, CDataBuffer &out);

            private:
                void tea_encrypt(uint32_t *v, uint32_t *k);
                void tea_decrypt(uint32_t *v, uint32_t *k);
                uint32_t _key[4];
                size_t _round;
        };
    }
}

#endif /* __TEACRYPT_H__ */
