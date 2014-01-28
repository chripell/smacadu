/* *************************************************************************

   Smart Card Digging Utilities. 
   Objects dumper.

   Written by Ilya O. Levin, http://www.literatecode.com    
   Ported to Linux by Christian Pellegrin

   This source code released for free "as is" under the whatever ABC 
   license without warranty or liability of any kind. Use it at your 
   own risk any way you like for legitimate purposes only. 

   ************************************************************************* 
*/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "pcscwrap.h"

#define MAX_MEMBUF 0xFFFF

static unsigned short dir[256] = {0};
static unsigned int  idr = 0;
static int ret;

long dump_it(sc_context *); 
unsigned char rdresz(sc_context *, unsigned long);

int main(int argc, char* argv[])
{
  sc_context ctx = {0}; 
  long rc, i;
  char *c, *a;

  if (argc>1) c = argv[1]; else
  {
    printf("SMACGET. Smart Card Digging Utilities Version 0.50721 by "
	   "Ilya O. Levin\nEasy and simple export of any accessible data "
	   "object from smart cards\n\n");
    printf("Usage: smacget reader target\n\n"
           "Examples:\n\t smacget Gemplus /3f00/0004\n"
           "\t smacget \"ron SIM Re\" 3f00/2000/2001/0001\n"
           "\n");
    c=sc_listreaders();
    if (c)
    {
      printf("Available readers:\n");
      a = strtok(c, "\n"); 
      while (a != NULL) { printf("* %s\n", a); a = strtok(NULL, "\n");}
    }
    else printf("Unable to list smart card readers\n");
    free (c);
    return 0;
  }

  if (argc>2)
  {
    a = strtok(argv[2], "/\\");
    while (a != NULL)
    {
      dir[idr++] = (unsigned short) strtoul(a, NULL, 16); 
      if (idr == 255) break;
      a = strtok(NULL, "/\\");
    }
  }

  if (idr<1) 
  {
    printf("ERR: bad target\n");
    return -1;
  }

  printf("* connecting to \"%s\" - ", c);
  rc = sc_init(&ctx, c); 
  if (rc == SC_OK)
  {
    printf("ok\n* Active reader: \"%s\"\n", ctx.rdr);
 
    printf("* probing for commands class - ");
    ctx.CLA=0xc0;  /* default for Axalto Cryptoflex */
    rc = sc_selectfile(&ctx, 0x3f00);
    for (i=0; (rc==SC_OK)&&(ctx.sw[0]!=0x61)&&(ctx.sw[0]!=0x90)&&(i<256) ;i++)
    {
       ctx.CLA = (unsigned char) i; rc = sc_selectfile(&ctx, 0x3f00);
    }
    if ( (ctx.sw[0]!=0x61) && (ctx.sw[0]!=0x90) ) {
      printf("failed\n");
      ret = -1;
    }
    else {
       printf("ok [CLA:%02x]\n", ctx.CLA);

       rc=dump_it(&ctx);

       printf("* %s\n", (rc==SC_OK)?"done":"bad luck");
    }
    sc_finish(&ctx);
  } 
  else {
    printf("failed (%s)\n", rc_symb(rc));
    ret = -2;
  }

  printf("RET: %d\n", ret);
  return ret;
} /* main */


/* -------------------------------------------------------------------------- */
long dump_it(sc_context *ctx)
{
  long rc = SC_BAD;
  unsigned long offs, sz;
  char *buf; 
  register unsigned char i;

  if (idr == 0) {
    ret = -3;
    return SC_BAD; 
  }

  printf("* selecting "); 
  for (i = 0; i < (int)idr; i++) 
  {
    printf("/%04x", dir[i]); 
    rc = sc_selectfile(ctx, dir[i]);
    if (rc != SC_OK) {
      printf("- failed (%s)", rc_symb(rc));
      ret = -4;
      break;
    }
  }
  if (rc == SC_OK) 
  {
    if ( (ctx->sw[0] == 0x90)||(ctx->sw[0] == 0x61) )
    {
       printf(" - Ok\n* dumping ... ");
       offs = sz = 0; 
       buf = (char *) malloc(MAX_MEMBUF);
       if (buf != NULL)
       {
          do
          {
             rc = rdresz(ctx, offs);
             rc = rc & 0xFF; 
             for (i = 0; i < (unsigned char)rc; i++) 
             if (offs+i < MAX_MEMBUF) buf[offs+i] = ctx->sw[i];
             offs+=rc; sz+=rc;
             printf("#");
          } while ( (rc>0) && (rc == (sizeof(ctx->sw)-8)) && (sz<MAX_MEMBUF));
          printf(" %lu byte(s) - ", sz);
          if (ctx->rw == 0x6982) printf("access denied, ");
          if (sz > MAX_MEMBUF) sz = MAX_MEMBUF;
          if (sz > 0)
          {
             /* drop to file, re-using offs as a file handler and 
             context's internal buffer as a place for a file name */
             sprintf((char *) ctx->sw, "%04x", dir[idr-1]);
             offs = open((char *)ctx->sw,  
                         O_RDWR | O_CREAT, 
                         0666
                        );
             if (offs!=-1) 
             { 
               sz = (write(offs, buf, sz) == (long) sz); 
               close(offs); 
             }
	     else {
	       ret = -5;
	     }
          }
          printf("%s [%04x]", (sz)?"ok":"failed", ctx->rw);
          free(buf);
       }
       else printf("out of memory");
       rc = (sz)?SC_OK:SC_BAD;
       if (rc != SC_OK)
	 ret = -6;
    }
    else if ( (ctx->rw == 0x6a82) ) {
      printf(" - not found");
      ret = -7;
    }
    else {
      printf(" - bad status [%04x]", ctx->rw);
      ret = -8;
    }
  }
  printf("\n");

  return rc;
} /* dump_it */

/* -------------------------------------------------------------------------- */
unsigned char rdresz(sc_context *ctx, unsigned long offs)
{
  long rc = SC_BAD;
  unsigned char sz = sizeof(ctx->sw)-8 ;

  while (sz)
  {
    rc = sc_readdata(ctx, sz, offs);
    if (rc != SC_OK) {
      ret = -9;
      break;
    }
    if ( (ctx->rw == 0x6B00) ||
         (ctx->rw == 0x6982) || 
         (ctx->rw == 0x6986) || 
         (ctx->rw == 0x6400) || 
         ((ctx->rw & 0xFFF0) == 0x6A80) 
       ) sz=0; 
    if ( (ctx->rw == 0x6700) || 
         (ctx->rw == 0x6282)
       ) sz--; else break;
  }

  return sz;
} /* rdresz */
