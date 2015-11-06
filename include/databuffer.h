#ifndef __DATABUFFER_H__
#define __DATABUFFER_H__

#include <vector>
#include <stdio.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <string>
#include <assert.h>
#include <arpa/inet.h>

#define MAX_BUFFER_SIZE 2048

namespace sol
{
    class CDataBuffer
    {
        private:
            void Destroy()
            {
                if (_pstart)
                {
                    free(_pstart);
                    _pend = _pfree = _pdata = _pstart = NULL;
                }
            }

        public:
            CDataBuffer()
            {
                _pend = _pfree = _pdata = _pstart = NULL;
            }

            ~CDataBuffer()
            {
                Destroy();
            }

            char *GetData()
            {
                return (char*) _pdata;
            }

            int GetDataLen()
            {
                return static_cast<int32_t>(_pfree - _pdata);
            }

            char *GetFree()
            {
                return (char*) _pfree;
            }

            int GetFreeLen()
            {
                return static_cast<int32_t>(_pend - _pfree);
            }

            void SeekPos(int offset)
            {
                _pdata = _pstart + offset;
            }

            void DrainData(int len)
            {
                _pdata += len;

                if (_pdata >= _pfree)
                {
                    Clear();
                }
            }

            void PourData(int len)
            {
                assert(_pend - _pfree >= len);
                _pfree += len;
            }

            void StripData(int len)
            {
                assert(_pfree - _pdata >= len);
                _pfree -= len;
            }

            void Clear()
            {
                _pdata = _pfree = _pstart;
            }

            void Shrink()
            {
                if (_pstart == NULL)
                {
                    return;
                }
                if ((_pend - _pstart) <= MAX_BUFFER_SIZE || (_pfree - _pdata) > MAX_BUFFER_SIZE)
                {
                    return;
                }

                int dlen = static_cast<int32_t>(_pfree - _pdata);
                if (dlen < 0) dlen = 0;

                unsigned char *newbuf = (unsigned char*) malloc(MAX_BUFFER_SIZE);
                assert(newbuf != NULL);

                if (dlen > 0)
                {
                    memcpy(newbuf, _pdata, dlen);
                }
                free(_pstart);

                _pdata = _pstart = newbuf;
                _pfree = _pstart + dlen;
                _pend = _pstart + MAX_BUFFER_SIZE;

                return;
            }

            void WriteInt8(uint8_t n)
            {
                Expand(1);
                *_pfree++ = (unsigned char) n;
            }

            void WriteInt16(uint16_t n)
            {
                Expand(2);
                _pfree[1] = (unsigned char) n;
                n = static_cast<uint16_t>(n >> 8);
                _pfree[0] = (unsigned char) n;
                _pfree += 2;
            }

            void WriteInt32(uint32_t n)
            {
                Expand(4);
                _pfree[3] = (unsigned char) n;
                n >>= 8;
                _pfree[2] = (unsigned char) n;
                n >>= 8;
                _pfree[1] = (unsigned char) n;
                n >>= 8;
                _pfree[0] = (unsigned char) n;
                _pfree += 4;
            }

            void WriteTeaBlock(uint32_t n)
            {
                unsigned char *s = (unsigned char*)&n;
                WriteInt8(s[0]);
                WriteInt8(s[1]);
                WriteInt8(s[2]);
                WriteInt8(s[3]);
            }

            void WriteInt64(uint64_t n)
            {
                Expand(8);
                _pfree[7] = (unsigned char) n;
                n >>= 8;
                _pfree[6] = (unsigned char) n;
                n >>= 8;
                _pfree[5] = (unsigned char) n;
                n >>= 8;
                _pfree[4] = (unsigned char) n;
                n >>= 8;
                _pfree[3] = (unsigned char) n;
                n >>= 8;
                _pfree[2] = (unsigned char) n;
                n >>= 8;
                _pfree[1] = (unsigned char) n;
                n >>= 8;
                _pfree[0] = (unsigned char) n;
                _pfree += 8;
            }

            void WriteBytes(const void *src, int len)
            {
                Expand(len);
                memcpy(_pfree, src, len);
                _pfree += len;
            }

            void FillInt8(unsigned char *dst, uint8_t n)
            {
                *dst = n;
            }

            void FillInt16(unsigned char *dst, uint16_t n)
            {
                dst[1] = (unsigned char) n;
                n = static_cast<uint16_t>(n >> 8);
                dst[0] = (unsigned char) n;
            }

            void FillInt32(unsigned char *dst, uint32_t n)
            {
                dst[3] = (unsigned char) n;
                n >>= 8;
                dst[2] = (unsigned char) n;
                n >>= 8;
                dst[1] = (unsigned char) n;
                n >>= 8;
                dst[0] = (unsigned char) n;
            }

            void FillInt64(unsigned char *dst, uint64_t n)
            {
                dst[7] = (unsigned char) n;
                n >>= 8;
                dst[6] = (unsigned char) n;
                n >>= 8;
                dst[5] = (unsigned char) n;
                n >>= 8;
                dst[4] = (unsigned char) n;
                n >>= 8;
                dst[3] = (unsigned char) n;
                n >>= 8;
                dst[2] = (unsigned char) n;
                n >>= 8;
                dst[1] = (unsigned char) n;
                n >>= 8;
                dst[0] = (unsigned char) n;
            }

            //todo: 下面几个函数是从原先的拷贝过来的，后续需要整改
            // 填充指定的字符数据
            void write_fill(int c, int len)
            {
                Expand(len);
                memset(_pfree, c, len);
                _pfree += len;
            }

            void fill_int8(uint8_t n, int offset)
            {
                *(_pstart + offset) = n;
            }

            void fill_int16(uint16_t n, int offset)
            {
                n = htons(n);
                memcpy(_pstart + offset, &n, 2);
            }

            void fill_int32(uint32_t n, int offset)
            {
                n = htonl(n);
                memcpy(_pstart + offset, &n, 4);
            }

            void fill_block(const void *src, int len, int offset)
            {
                memcpy(_pstart + offset, src, len);
            }

            void WriteString(const char *str)
            {
                int len = (str ? static_cast<int32_t>(strlen(str)) : 0);
                if (len > 0) len++;
                Expand(static_cast<int32_t>(len + sizeof(uint32_t)));
                WriteInt32(len);
                if (len > 0)
                {
                    memcpy(_pfree, str, len);
                    _pfree += (len);
                }
            }

            void WriteString(const std::string& str)
            {
                WriteString(str.c_str());
            }

            void WriteVector(const std::vector<int32_t>& v)
            {
                const uint32_t iLen = static_cast<uint32_t>(v.size());
                WriteInt32(iLen);
                for (uint32_t i = 0; i < iLen; ++i)
                {
                    WriteInt32(v[i]);
                }
            }

            void WriteVector(const std::vector<uint32_t>& v)
            {
                const uint32_t iLen = static_cast<uint32_t>(v.size());
                WriteInt32(iLen);
                for (uint32_t i = 0; i < iLen; ++i)
                {
                    WriteInt32(v[i]);
                }
            }

            void WriteVector(const std::vector<int64_t>& v)
            {
                const uint32_t iLen = static_cast<uint32_t>(v.size());
                WriteInt32(iLen);
                for (uint32_t i = 0; i < iLen; ++i)
                {
                    WriteInt64(v[i]);
                }
            }

            void WriteVector(const std::vector<uint64_t>& v)
            {
                const uint32_t iLen = static_cast<uint32_t>(v.size());
                WriteInt32(iLen);
                for (uint32_t i = 0; i < iLen; ++i)
                {
                    WriteInt64(v[i]);
                }
            }

            uint8_t ReadInt8()
            {
                return (*_pdata++);
            }

            uint16_t ReadInt16()
            {
                uint16_t n = _pdata[0];
                n = static_cast<uint16_t>(n << 8);
                n = static_cast<uint16_t>(n | _pdata[1]);
                _pdata += 2;
                return n;
            }

            uint32_t ReadInt32()
            {
                uint32_t n = _pdata[0];
                n <<= 8;
                n |= _pdata[1];
                n <<= 8;
                n |= _pdata[2];
                n <<= 8;
                n |= _pdata[3];
                _pdata += 4;
                assert(_pfree >= _pdata);
                return n;
            }

            uint64_t ReadInt64()
            {
                uint64_t n = _pdata[0];
                n <<= 8;
                n |= _pdata[1];
                n <<= 8;
                n |= _pdata[2];
                n <<= 8;
                n |= _pdata[3];
                n <<= 8;
                n |= _pdata[4];
                n <<= 8;
                n |= _pdata[5];
                n <<= 8;
                n |= _pdata[6];
                n <<= 8;
                n |= _pdata[7];
                _pdata += 8;
                assert(_pfree >= _pdata);
                return n;
            }

            bool ReadInt8(uint8_t &n)
            {
                if (_pdata + 1 > _pfree) return false;
                n = ReadInt8();
                return true;
            }

            bool ReadInt16(uint16_t &n)
            {
                if (_pdata + 2 > _pfree) return false;
                n = ReadInt16();
                return true;
            }

            bool ReadInt32(uint32_t &n)
            {
                if (_pdata + 4 > _pfree) return false;
                n = ReadInt32();
                return true;
            }

            bool ReadInt64(uint64_t &n)
            {
                if (_pdata + 8 > _pfree) return false;
                n = ReadInt64();
                return true;
            }

            bool ReadBytes(void *dst, int len)
            {
                if (_pdata + len > _pfree)
                {
                    return false;
                }
                memcpy(dst, _pdata, len);
                _pdata += len;
                assert(_pfree >= _pdata);
                return true;
            }

            bool ReadString(char *&str, int len)
            {
                if (_pdata + sizeof(int) > _pfree)
                {
                    return false;
                }
                int slen = ReadInt32();
                if (_pfree - _pdata < slen)
                {
                    slen = static_cast<int32_t>(_pfree - _pdata);
                }
                if (str == NULL && slen > 0)
                {
                    str = (char*) malloc(slen);
                    len = slen;
                }
                if (len > slen)
                {
                    len = slen;
                }
                if (len > 0)
                {
                    memcpy(str, _pdata, len);
                    str[len - 1] = '\0';
                }
                _pdata += slen;
                assert(_pfree >= _pdata);
                return true;
            }

            bool ReadVector(std::vector<int32_t>& v)
            {
                const uint32_t len = ReadInt32();
                for (uint32_t i = 0; i < len; ++i)
                {
                    v.push_back(ReadInt32());
                }
                return true;
            }

            bool ReadVector(std::vector<uint32_t>& v)
            {
                const uint32_t len = ReadInt32();
                for (uint32_t i = 0; i < len; ++i)
                {
                    v.push_back(ReadInt32());
                }
                return true;
            }

            bool ReadVector(std::vector<int64_t>& v)
            {
                const uint32_t len = ReadInt32();
                for (uint32_t i = 0; i < len; ++i)
                {
                    v.push_back(ReadInt64());
                }
                return true;
            }

            bool ReadVector(std::vector<uint64_t>& v)
            {
                const uint32_t len = ReadInt32();
                for (uint32_t i = 0; i < len; ++i)
                {
                    v.push_back(ReadInt64());
                }
                return true;
            }

            void EnsureFree(int len)
            {
                Expand(len);
            }

            int FindBytes(const char *findstr, int len)
            {
                int dLen = static_cast<int32_t>(_pfree - _pdata - len + 1);
                for (int i = 0; i < dLen; i++)
                {
                    if (_pdata[i] == findstr[0] && memcmp(_pdata + i, findstr, len) == 0)
                    {
                        return i;
                    }
                }
                return -1;
            }

        public:

            inline void Expand(int need)
            {
                if (_pstart == NULL)
                {
                    int len = 256;
                    while (len < need)
                        len <<= 1;
                    _pfree = _pdata = _pstart = (unsigned char*) malloc(len);
                    _pend = _pstart + len;
                }
                else if (_pend - _pfree < need)
                {
                    int flen = static_cast<int32_t>((_pend - _pfree) + (_pdata - _pstart));
                    int dlen = static_cast<int32_t>(_pfree - _pdata);

                    if (flen < need || flen * 4 < dlen)
                    {
                        int bufsize = static_cast<int32_t>((_pend - _pstart) * 2);
                        while (bufsize - dlen < need)
                            bufsize <<= 1;

                        unsigned char *newbuf = (unsigned char *) malloc(bufsize);
                        if (newbuf == NULL)
                        {
                            printf("expand data buffer failed, length: %d", bufsize);
                        }
                        assert(newbuf != NULL);
                        if (dlen > 0)
                        {
                            memcpy(newbuf, _pdata, dlen);
                        }
                        free(_pstart);

                        _pdata = _pstart = newbuf;
                        _pfree = _pstart + dlen;
                        _pend = _pstart + bufsize;
                    }
                    else
                    {
                        memmove(_pstart, _pdata, dlen);
                        _pfree = _pstart + dlen;
                        _pdata = _pstart;
                    }
                }
            }

            unsigned char *_pstart;         // buffer start
            unsigned char *_pend;           // buffer end
            unsigned char *_pfree;          // free part
            unsigned char *_pdata;          // data part
    };
}

#endif /* __DATABUFFER_H__ */
