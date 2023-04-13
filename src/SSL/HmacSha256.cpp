#include <string.h>
#include "hmacsha256.h" 

//#define ROR(n,k) ((n >> k) | (n << (32 - k)))

static uint32_t ror(uint32_t n, uint32_t k)
{
	return (n >> k) | (n << (32 - k));
}

#define ROR(n,k) ror(n,k)

#define CH(x,y,z)  (z ^ (x & (y ^ z)))
#define MAJ(x,y,z) ((x & y) | (z & (x | y)))
#define S0(x)      (ROR(x, 2) ^ ROR(x,13) ^ ROR(x,22))
#define S1(x)      (ROR(x, 6) ^ ROR(x,11) ^ ROR(x,25))
#define R0(x)      (ROR(x, 7) ^ ROR(x,18) ^ (x>>3))
#define R1(x)      (ROR(x,17) ^ ROR(x,19) ^ (x>>10))

static const uint32_t K[64] =
{
	0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
	0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
	0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
	0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
	0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
	0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
	0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
	0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

static void sha256_transform(SHA256_CTX* s, const uint8_t* buf)
{
	uint32_t t1, t2, a, b, c, d, e, f, g, h, m[64];
	uint32_t i, j;

	for (i = 0, j = 0; i < 16; i++, j += 4)
	{
		m[i] = (uint32_t)buf[j] << 24 | (uint32_t)buf[j + 1] << 16 |
			(uint32_t)buf[j + 2] << 8 | (uint32_t)buf[j + 3];
	}
	for (; i < 64; i++)
	{
		m[i] = R1(m[i - 2]) + m[i - 7] + R0(m[i - 15]) + m[i - 16];
	}
	a = s->h[0];
	b = s->h[1];
	c = s->h[2];
	d = s->h[3];
	e = s->h[4];
	f = s->h[5];
	g = s->h[6];
	h = s->h[7];
	for (i = 0; i < 64; i++)
	{
		t1 = h + S1(e) + CH(e, f, g) + K[i] + m[i];
		t2 = S0(a) + MAJ(a, b, c);
		h = g;
		g = f;
		f = e;
		e = d + t1;
		d = c;
		c = b;
		b = a;
		a = t1 + t2;
	}
	s->h[0] += a;
	s->h[1] += b;
	s->h[2] += c;
	s->h[3] += d;
	s->h[4] += e;
	s->h[5] += f;
	s->h[6] += g;
	s->h[7] += h;
}

void sha256_init(SHA256_CTX* s)
{
	s->len = 0;

	s->h[0] = 0x6a09e667;
	s->h[1] = 0xbb67ae85;
	s->h[2] = 0x3c6ef372;
	s->h[3] = 0xa54ff53a;
	s->h[4] = 0x510e527f;
	s->h[5] = 0x9b05688c;
	s->h[6] = 0x1f83d9ab;
	s->h[7] = 0x5be0cd19;
}

void sha256_final(SHA256_CTX* s, uint8_t* md)
{
	uint32_t r = s->len % SHA256_BLOCKLEN;
	int i;

	//pad
	s->buf[r++] = 0x80;
	if (r > 56)
	{
		memset(s->buf + r, 0, SHA256_BLOCKLEN - r);
		r = 0;
		sha256_transform(s, s->buf);
	}
	memset(s->buf + r, 0, 56 - r);
	s->len *= 8;
	s->buf[56] = s->len >> 56;
	s->buf[57] = s->len >> 48;
	s->buf[58] = s->len >> 40;
	s->buf[59] = s->len >> 32;
	s->buf[60] = s->len >> 24;
	s->buf[61] = s->len >> 16;
	s->buf[62] = s->len >> 8;
	s->buf[63] = s->len;
	sha256_transform(s, s->buf);

	for (i = 0; i < SHA256_DIGESTINT; i++)
	{
		md[4 * i] = s->h[i] >> 24;
		md[4 * i + 1] = s->h[i] >> 16;
		md[4 * i + 2] = s->h[i] >> 8;
		md[4 * i + 3] = s->h[i];
	}
	sha256_init(s);
}

void sha256_update(SHA256_CTX* s, const uint8_t* m, uint32_t len)
{
	const uint8_t* p = m;
	uint32_t r = s->len % SHA256_BLOCKLEN;

	s->len += len;
	if (r)
	{
		if (len + r < SHA256_BLOCKLEN)
		{
			memcpy(s->buf + r, p, len);
			return;
		}
		memcpy(s->buf + r, p, SHA256_BLOCKLEN - r);
		len -= SHA256_BLOCKLEN - r;
		p += SHA256_BLOCKLEN - r;
		sha256_transform(s, s->buf);
	}
	for (; len >= SHA256_BLOCKLEN; len -= SHA256_BLOCKLEN, p += SHA256_BLOCKLEN)
	{
		sha256_transform(s, p);
	}
	memcpy(s->buf, p, len);
}

#define INNER_PAD '\x36'
#define OUTER_PAD '\x5c'

void hmac_sha256_init(HMAC_SHA256_CTX* hmac, const uint8_t* key, uint32_t keylen)
{
	SHA256_CTX* sha = &hmac->sha;
	uint32_t i;

	if (keylen <= SHA256_BLOCKLEN)
	{
		memcpy(hmac->buf, key, keylen);
		memset(hmac->buf + keylen, '\0', SHA256_BLOCKLEN - keylen);
	}
	else
	{
		sha256_init(sha);
		sha256_update(sha, key, keylen);
		sha256_final(sha, hmac->buf);
		memset(hmac->buf + SHA256_DIGESTLEN, '\0', SHA256_BLOCKLEN - SHA256_DIGESTLEN);
	}

	for (i = 0; i < SHA256_BLOCKLEN; i++)
	{
		hmac->buf[i] = hmac->buf[i] ^ OUTER_PAD;
	}

	sha256_init(sha);
	sha256_update(sha, hmac->buf, SHA256_BLOCKLEN);
	// copy outer state
	memcpy(hmac->h_outer, sha->h, SHA256_DIGESTLEN);

	for (i = 0; i < SHA256_BLOCKLEN; i++)
	{
		hmac->buf[i] = (hmac->buf[i] ^ OUTER_PAD) ^ INNER_PAD;
	}

	sha256_init(sha);
	sha256_update(sha, hmac->buf, SHA256_BLOCKLEN);
	// copy inner state
	memcpy(hmac->h_inner, sha->h, SHA256_DIGESTLEN);
}

void hmac_sha256_update(HMAC_SHA256_CTX* hmac, const uint8_t* m, uint32_t mlen)
{
	sha256_update(&hmac->sha, m, mlen);
}

void hmac_sha256_final(HMAC_SHA256_CTX* hmac, uint8_t* md)
{
	SHA256_CTX* sha = &hmac->sha;
	sha256_final(sha, md);

	// reset sha to outer state
	memcpy(sha->h, hmac->h_outer, SHA256_DIGESTLEN);
	sha->len = SHA256_BLOCKLEN;

	sha256_update(sha, md, SHA256_DIGESTLEN);
	sha256_final(sha, md); // md = D(outer || D(inner || msg))

	// reset sha to inner state -> reset hmac
	memcpy(sha->h, hmac->h_inner, SHA256_DIGESTLEN);
	sha->len = SHA256_BLOCKLEN;
}

void pbkdf2_sha256(HMAC_SHA256_CTX* hmac,
	const uint8_t* key, uint32_t keylen, const uint8_t* salt, uint32_t saltlen, uint32_t rounds,
	uint8_t* dk, uint32_t dklen)
{
	uint8_t* U;
	uint8_t* T;
	uint8_t count[4];
	uint32_t i, j, k;
	uint32_t len;

	uint32_t hlen = SHA256_DIGESTLEN;
	uint32_t l = dklen / hlen + ((dklen % hlen) ? 1 : 0);
	uint32_t r = dklen - (l - 1) * hlen;

	hmac_sha256_init(hmac, key, keylen);

	U = hmac->buf;
	T = dk;

	len = hlen;
	for (i = 1; i <= l; i++)
	{
		if (i == l) { len = r; }
		count[0] = (i >> 24) & 0xFF;
		count[1] = (i >> 16) & 0xFF;
		count[2] = (i >> 8) & 0xFF;
		count[3] = (i) & 0xFF;
		hmac_sha256_update(hmac, salt, saltlen);
		hmac_sha256_update(hmac, count, 4);
		hmac_sha256_final(hmac, U);
		memcpy(T, U, len);
		for (j = 1; j < rounds; j++)
		{
			hmac_sha256_update(hmac, U, hlen);
			hmac_sha256_final(hmac, U);
			for (k = 0; k < len; k++)
			{
				T[k] ^= U[k];
			}
		}
		T += len;
	}

}

