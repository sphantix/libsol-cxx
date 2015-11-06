#include "teacrypt.h"

namespace sol 
{
    void CTea::SetKey(uint32_t key[4])
    {
        _key[0] = key[0];
        _key[1] = key[1];
        _key[2] = key[2];
        _key[3] = key[3];
    }

    void CTea::tea_encrypt(uint32_t* v, uint32_t* k)
    {
        uint32_t v0 = v[0], v1 = v[1], sum = 0, i;
        uint32_t delta = 0x9e3779b9;
        uint32_t k0 = k[0], k1 = k[1], k2 = k[2], k3 = k[3];

        for (i = 0; i < _round; i++)
        {
            sum += delta;
            v0 += ((v1 << 4) + k0) ^ (v1 + sum) ^ ((v1 >> 5) + k1);
            v1 += ((v0 << 4) + k2) ^ (v0 + sum) ^ ((v0 >> 5) + k3);
        }

        v[0] = v0;
        v[1] = v1;
    }

    void CTea::tea_decrypt(uint32_t* v, uint32_t* k)
    {
        uint32_t v0 = v[0], v1 = v[1], sum, i;
        sum = (_round == 16) ? 0xE3779B90 : 0xC6EF3720;

        uint32_t delta = 0x9e3779b9;
        uint32_t k0 = k[0], k1 = k[1], k2 = k[2], k3 = k[3];
        for (i = 0; i < _round; i++)
        {
            v1 -= ((v0 << 4) + k2) ^ (v0 + sum) ^ ((v0 >> 5) + k3);
            v0 -= ((v1 << 4) + k0) ^ (v1 + sum) ^ ((v1 >> 5) + k1);
            sum -= delta;
        }

        v[0] = v0;
        v[1] = v1;
    }

    bool CTea::Encrypt(const void *input, int input_len, CDataBuffer &out)
    {
        if(input == NULL || input_len <= 0)
            return false;

        int blocks =  (input_len + 7 ) / 8;
        out.Expand(blocks * 8);
        out.WriteBytes((const void*)input, input_len);
        int num = blocks * 8 - input_len;
        while(num--)
        {
            out.WriteInt8(0);
        }

        uint32_t *data = (uint32_t *)out.GetData();
        for(int i = 0; i < blocks; i ++)
        {
            tea_encrypt((uint32_t*)(data + 2 * i), _key);
        }

        return true;
    }

    bool CTea::Decrypt(const void *input, int input_len, CDataBuffer &out)
    {
        if(input == NULL || input_len < 8)
            return false;

        int blocks =  input_len / 8;
        out.Expand(blocks * 8);
        out.WriteBytes((const void*)input, blocks * 8);

        uint32_t *data = (uint32_t *)out.GetData();
        for(int i = 0; i < blocks; i ++)
        {
            tea_decrypt((uint32_t*)(data + 2 * i), _key);
        }

        return true;
    }
}
