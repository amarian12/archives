
/*
 *  md2.c : MD2 hash algorithm.
 *
 * Part of the Python Cryptography Toolkit, version 1.1
 *
 * Distribute and use freely; there are no restrictions on further 
 * dissemination and usage except those imposed by the laws of your 
 * country of residence.
 *
 */
  

#include <string.h>

typedef unsigned char U8;
typedef unsigned int U32;

typedef struct {
  PCTObject_HEAD
  U8 C[16], X[48];
  int count;
  U8 buf[16];
} MD2object;

static void MD2init (ptr)
     MD2object *ptr;
{
  memset(ptr->X, 0, 48);
  memset(ptr->C, 0, 16);
  ptr->count=0;
}

static U8 S[256] = {
  41, 46, 67, 201, 162, 216, 124, 1, 61, 54, 84, 161, 236, 240, 6,
  19, 98, 167, 5, 243, 192, 199, 115, 140, 152, 147, 43, 217, 188,
  76, 130, 202, 30, 155, 87, 60, 253, 212, 224, 22, 103, 66, 111, 24,
  138, 23, 229, 18, 190, 78, 196, 214, 218, 158, 222, 73, 160, 251,
  245, 142, 187, 47, 238, 122, 169, 104, 121, 145, 21, 178, 7, 63,
  148, 194, 16, 137, 11, 34, 95, 33, 128, 127, 93, 154, 90, 144, 50,
  39, 53, 62, 204, 231, 191, 247, 151, 3, 255, 25, 48, 179, 72, 165,
  181, 209, 215, 94, 146, 42, 172, 86, 170, 198, 79, 184, 56, 210,
  150, 164, 125, 182, 118, 252, 107, 226, 156, 116, 4, 241, 69, 157,
  112, 89, 100, 113, 135, 32, 134, 91, 207, 101, 230, 45, 168, 2, 27,
  96, 37, 173, 174, 176, 185, 246, 28, 70, 97, 105, 52, 64, 126, 15,
  85, 71, 163, 35, 221, 81, 175, 58, 195, 92, 249, 206, 186, 197,
  234, 38, 44, 83, 13, 110, 133, 40, 132, 9, 211, 223, 205, 244, 65,
  129, 77, 82, 106, 220, 55, 200, 108, 193, 171, 250, 36, 225, 123,
  8, 12, 189, 177, 74, 120, 136, 149, 139, 227, 99, 232, 109, 233,
  203, 213, 254, 59, 0, 29, 57, 242, 239, 183, 14, 102, 88, 208, 228,
  166, 119, 114, 248, 235, 117, 75, 10, 49, 68, 80, 180, 143, 237,
  31, 26, 219, 153, 141, 51, 159, 17, 131, 20
};

static void
MD2copy(src, dest)
     MD2object *src, *dest;
{
  dest->count=src->count;  
  memcpy(dest->buf, src->buf, dest->count);
  memcpy(dest->X, src->X, 48);
  memcpy(dest->C, src->C, 16);
}


static void MD2update (self, buf, len)
MD2object *self;
const U8 *buf;
U32 len;
{
  U32 L;
  while (len) 
    {
      L=(16-self->count) < len ? (16-self->count) : len;
      memcpy(self->buf+self->count, buf, L);
      self->count+=L;
      buf+=L;
      len-=L;
      if (self->count==16) 
	{
	  U8 t;
	  int i,j;
	  
	  self->count=0;
	  memcpy(self->X+16, self->buf, 16);
	  t=self->C[15];
	  for(i=0; i<16; i++)
	    {
	      self->X[32+i]=self->X[16+i]^self->X[i];
	      t=self->C[i]^=S[self->buf[i]^t];
	    }
	  
	  t=0;
	  for(i=0; i<18; i++)
	    {
	      for(j=0; j<48; j++)
		t=self->X[j]^=S[t];
	      t=(t+i) & 0xFF;
	    }
	}
    }
}

static PyObject *
MD2digest (self)
     const MD2object *self;
{
  U8 padding[16];
  U32 padlen;
  MD2object temp;
  int i;
  
  memcpy(&temp, self, sizeof(MD2object));
  padlen= 16-self->count;
  for(i=0; i<padlen; i++) padding[i]=padlen;
  MD2update(&temp, padding, padlen);
  MD2update(&temp, temp.C, 16);
  return PyString_FromStringAndSize(temp.X, 16);
}
