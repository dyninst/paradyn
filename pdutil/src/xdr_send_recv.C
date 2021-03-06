/*
 * Copyright (c) 1996-2004 Barton P. Miller
 * 
 * We provide the Paradyn Parallel Performance Tools (below
 * described as "Paradyn") on an AS IS basis, and do not warrant its
 * validity or performance.  We reserve the right to update, modify,
 * or discontinue this software at any time.  We shall have no
 * obligation to supply such updates or modifications or any other
 * form of support to you.
 * 
 * This license is for research uses.  For such uses, there is no
 * charge. We define "research use" to mean you may freely use it
 * inside your organization for whatever purposes you see fit. But you
 * may not re-distribute Paradyn or parts of Paradyn, in any form
 * source or binary (including derivatives), electronic or otherwise,
 * to any other organization or entity without our permission.
 * 
 * (for other uses, please contact us at paradyn@cs.wisc.edu)
 * 
 * All warranties, including without limitation, any warranty of
 * merchantability or fitness for a particular purpose, are hereby
 * excluded.
 * 
 * By your use of Paradyn, you understand and agree that we (or any
 * other person or entity with proprietary rights in Paradyn) are
 * under no obligation to provide either maintenance services,
 * update services, notices of latent defects, or correction of
 * defects for Paradyn.
 * 
 * Even if advised of the possibility of such damages, under no
 * circumstances shall we (or any other person or entity with
 * proprietary rights in the software licensed hereunder) be liable
 * to you or any third party for direct, indirect, or consequential
 * damages of any character regardless of type of action, including,
 * without limitation, loss of profits, loss of use, loss of good
 * will, or computer failure or malfunction.  You agree to indemnify
 * us (and any other person or entity with proprietary rights in the
 * software licensed hereunder) for any and all liability it may
 * incur to third parties resulting from your use of Paradyn.
 */

// xdr_send_recv.C

#include "pdutil/h/xdr_send_recv.h"
#include <assert.h>

bool P_xdr_send(XDR *xdr, const bool &b) {
   assert(xdr->x_op == XDR_ENCODE);
   bool_t actual_b = b; // needed since they're probably of different types
   return xdr_bool(xdr, &actual_b) != 0;
}

bool P_xdr_send(XDR *xdr, const short &s) {
   assert(xdr->x_op == XDR_ENCODE);
   short actual_s = s; 
   return xdr_short(xdr, &actual_s) != 0;
}

bool P_xdr_send(XDR *xdr, const unsigned short &us) {
   assert(xdr->x_op == XDR_ENCODE);
   unsigned short actual_us = us;
   return xdr_u_short(xdr, &actual_us) != 0;
}

#if defined(i386_unknown_nt4_0) && defined(_MSC_VER) && (_MSC_VER < 1300)

bool P_xdr_send(XDR *xdr, const int &i) {
   int j = i;
   assert(xdr->x_op == XDR_ENCODE);
   return xdr_int(xdr, &j);
}

bool P_xdr_send(XDR *xdr, const unsigned &u) {
   unsigned v = u;
   assert(xdr->x_op == XDR_ENCODE);
   return xdr_u_int(xdr, &v);
}

bool P_xdr_send(XDR *xdr, const long &l) {
   long k = l;
   assert(xdr->x_op == XDR_ENCODE);
   return xdr_long(xdr, &k);
}

bool P_xdr_send(XDR *xdr, const unsigned long &ul) {
   unsigned long vl = ul;
   assert(xdr->x_op == XDR_ENCODE);
   return xdr_u_long(xdr, &vl);
}

#endif //defined(i386_unknown_nt4_0) && defined(_MSC_VER) && (_MSC_VER < 1300)

bool P_xdr_send(XDR *xdr, const uint32_t &num) {
   assert(xdr->x_op == XDR_ENCODE);
   uint32_t actual_num = num;
   return xdr_u_int(xdr, (unsigned *)&actual_num) != 0;
}

bool P_xdr_send(XDR *xdr, const int32_t &num) {
   assert(xdr->x_op == XDR_ENCODE);
   int32_t actual_num = num;
   return xdr_int(xdr, (int *)&actual_num) != 0;
}

#if !defined(i386_unknown_nt4_0)

bool P_xdr_send(XDR *xdr, const uint64_t &num) {
   assert(xdr->x_op == XDR_ENCODE);
   uint64_t actual_num = num;
   return xdr_u_hyper(xdr, &actual_num);
}

bool P_xdr_send(XDR *xdr, const int64_t &num) {
   assert(xdr->x_op == XDR_ENCODE);
   int64_t actual_num = num;
   return xdr_hyper(xdr, &actual_num);
}

#endif //!defined(i386_unknown_nt4_0)

bool P_xdr_send(XDR *xdr, const float &f) {
   assert(xdr->x_op == XDR_ENCODE);
   float actual_f = f;
   return xdr_float(xdr, &actual_f) != 0;
}

bool P_xdr_send(XDR *xdr, const double &d) {
   assert(xdr->x_op == XDR_ENCODE);
   double actual_d = d;
   return xdr_double(xdr, &actual_d) != 0;
}

bool P_xdr_send(XDR *xdr, const byteArray &ba) {
  unsigned length = ba.length();
  if (!P_xdr_send(xdr, length))
    return false;

  if (length > 0) {
    char *buffer = static_cast<char*>(const_cast<void*>(ba.getArray()));
    if (!xdr_bytes(xdr, &buffer, &length, ba.length()))
    return false;
  }

  return true;
}

#if defined (ca_use_pdstring)
bool P_xdr_send(XDR *xdr, const pdstring &s) {
   unsigned len = s.length();
   if (!P_xdr_send(xdr, len))
      return false;

   if (len == 0)
      return true;
   
   char *buffer = const_cast<char*>(s.c_str());
      // xdr doesn't want to use "const char *" so we use "char *"

   if (!xdr_bytes(xdr, &buffer, &len, len + 1))
      return false;
   
   return true;
}
#endif

bool P_xdr_send(XDR *xdr, const std::string &s) {
   unsigned len = s.length();
   if (!P_xdr_send(xdr, len))
      return false;

   if (len == 0)
      return true;
   
   char *buffer = const_cast<char*>(s.c_str());
      // xdr doesn't want to use "const char *" so we use "char *"

   if (!xdr_bytes(xdr, &buffer, &len, len + 1))
      return false;
   
   return true;
}

// ----------------------------------------------------------------------

bool P_xdr_recv(XDR *xdr, bool &b) {
   assert(xdr->x_op == XDR_DECODE);
   bool_t actual_b;
   const bool result = xdr_bool(xdr, &actual_b) != 0; 
   b = actual_b;
   return result;
}

bool P_xdr_recv(XDR *xdr, short &s) {
   assert(xdr->x_op == XDR_DECODE);
   return xdr_short(xdr, &s) != 0;
}

bool P_xdr_recv(XDR *xdr, unsigned short &us) {
   assert(xdr->x_op == XDR_DECODE);
   return xdr_u_short(xdr, &us) != 0;
}

#if defined(i386_unknown_nt4_0) && defined(_MSC_VER) && (_MSC_VER < 1300)

bool P_xdr_recv(XDR *xdr, int &num) {
   assert(xdr->x_op == XDR_DECODE);
   return xdr_int(xdr, &num) != 0;
}

bool P_xdr_recv(XDR *xdr, unsigned &num) {
   assert(xdr->x_op == XDR_DECODE);
   return xdr_u_int(xdr, &num);
}

bool P_xdr_recv(XDR *xdr, long &l) {
   assert(xdr->x_op == XDR_DECODE);
   return xdr_long(xdr, &l);
}

bool P_xdr_recv(XDR *xdr, unsigned long &ul) {
   assert(xdr->x_op == XDR_DECODE);
   return xdr_u_long(xdr, &ul);
}

#endif //defined(i386_unknown_nt4_0) && defined(_MSC_VER) && (_MSC_VER < 1300)

bool P_xdr_recv(XDR *xdr, uint32_t &num) {
   assert(xdr->x_op == XDR_DECODE);
   return xdr_u_int(xdr, (unsigned *)&num) != 0;
}

bool P_xdr_recv(XDR *xdr, int32_t &num) {
   assert(xdr->x_op == XDR_DECODE);
   return xdr_int(xdr, (int *)&num) != 0;
}

#if !defined(i386_unknown_nt4_0)

bool P_xdr_recv(XDR *xdr, uint64_t &num) {
   assert(xdr->x_op == XDR_DECODE);
   return xdr_u_hyper(xdr, &num);
}

bool P_xdr_recv(XDR *xdr, int64_t &num) {
   assert(xdr->x_op == XDR_DECODE);
   return xdr_hyper(xdr, &num);
}

#endif //!defined(i386_unknown_nt4_0)

bool P_xdr_recv(XDR *xdr, float &f) {
   assert(xdr->x_op == XDR_DECODE);
   return xdr_float(xdr, &f) != 0;
}

bool P_xdr_recv(XDR *xdr, double &d) {
   assert(xdr->x_op == XDR_DECODE);
   return xdr_double(xdr, &d) != 0;
}

bool P_xdr_recv(XDR *xdr, byteArray &ba) {
// reminder: as always for recv routines, the 2d arg is raw memory (no ctor has
// even been called!).  At least it's been allocated, though.
  unsigned length;
  if (!P_xdr_recv(xdr, length))
    return false;
  if (length == 0) {
    (void)new((void*)&ba)byteArray(NULL, 0);
    return true;
  }

  char *temp = new char[length];
  unsigned actual_len;
  if (!xdr_bytes(xdr, &temp, &actual_len, length)) {
    delete [] temp;
    return false;
  }
  else if (actual_len != length) {
    delete [] temp;
    return false;
  }
  else {
    (void)new((void*)&ba)byteArray(temp, actual_len);
    delete [] temp;
    return true;
  }
}

#if defined (cap_use_pdstring)
bool P_xdr_recv(XDR *xdr, pdstring &s) {
   // as always "s" is uninitialized, unconstructed raw memory, so we need
   // to manually call the constructor.  As always, constructing a c++ object
   // in-place can best be done via the void* placement operator new.
   
   assert(xdr->x_op == XDR_DECODE);

   unsigned len;
   if (!P_xdr_recv(xdr, len))
      return false;
   
   if (len == 0) {
      (void)new((void*)&s)pdstring();
      return true;
   }

   char *buffer = new char[len + 1];
   buffer[len] = '\0';

   unsigned size = len;
   if (!xdr_bytes(xdr, &buffer, &size, len+1))
      return false;

   //cout << "buffer is " << buffer << ", size is " << size << endl;
   
   (void)new((void*)&s)pdstring(buffer, size);
   
   delete [] buffer;
   
   return true;
}
#endif

bool P_xdr_recv(XDR *xdr, std::string &s) {
   // as always "s" is uninitialized, unconstructed raw memory, so we need
   // to manually call the constructor.  As always, constructing a c++ object
   // in-place can best be done via the void* placement operator new.
   
   assert(xdr->x_op == XDR_DECODE);

   unsigned len;
   if (!P_xdr_recv(xdr, len))
      return false;
   
   if (len == 0) {
      (void)new((void*)&s)std::string();
      return true;
   }

   char *buffer = new char[len + 1];
   buffer[len] = '\0';

   unsigned size = len;
   if (!xdr_bytes(xdr, &buffer, &size, len+1))
      return false;

   //cout << "buffer is " << buffer << ", size is " << size << endl;
   
   (void)new((void*)&s)std::string(buffer, size);
   
   delete [] buffer;
   
   return true;
}
