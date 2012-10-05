/*
  Copyright (c) 2012, Paul Baecher
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
      * Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.
      * Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.
      * Neither the name of the <organization> nor the
        names of its contributors may be used to endorse or promote products
        derived from this software without specific prior written permission.
  
  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL <THE COPYRIGHT HOLDER> BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef LCFG_STRING_H
#define LCFG_STRING_H

struct lcfg_string *  lcfg_string_new();
struct lcfg_string *  lcfg_string_new_copy(struct lcfg_string *);
int                   lcfg_string_set(struct lcfg_string *, const char *);
int                   lcfg_string_cat_char(struct lcfg_string *, char);
int                   lcfg_string_cat_cstr(struct lcfg_string *, const char *);
int                   lcfg_string_cat_uint(struct lcfg_string *, unsigned int);
int                   lcfg_string_find(struct lcfg_string *, char);
int                   lcfg_string_rfind(struct lcfg_string *, char);
void                  lcfg_string_trunc(struct lcfg_string *, unsigned int);
const char *          lcfg_string_cstr(struct lcfg_string *);
unsigned int          lcfg_string_len(struct lcfg_string *);
void                  lcfg_string_delete(struct lcfg_string *);

#endif
