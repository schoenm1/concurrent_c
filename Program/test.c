#include <stdio.h>
#include <stdarg.h>

//LOG LEVELS
typedef enum
{
  LOG_DEFAULT,
  LOG_INFO,
  LOG_ERROR,
  LOG_DEBUG
}LOG_LEVEL;

void LOG_TRACE(LOG_LEVEL lvl, char *fmt, ... );

int main()
{
  int i =10;
  char *string="Hello World";
  char c='a';

  LOG_TRACE(LOG_INFO, "String - %s\n", string);
  LOG_TRACE(LOG_DEBUG, "Integer - %d\n", i);
  LOG_TRACE(LOG_INFO, "Character - %c\n", c);

  LOG_TRACE(LOG_INFO, "\nTOTAL DATA: %s - %d - %c\n", string, i, c);
  return 1;
}

/* LOG_TRACE(log level, format, args ) */
void LOG_TRACE(LOG_LEVEL lvl, char *fmt, ... )
{
  va_list  list;
  char *s, c;
  int i;

  if( (lvl==LOG_INFO) || (lvl==LOG_ERROR))
  {
     va_start( list, fmt );

     while(*fmt)
     {
        if ( *fmt != '%' )
           putc( *fmt, stdout );
        else
        {
           switch ( *++fmt )
           {
              case 's':
                 /* set r as the next char in list (string) */
                 s = va_arg( list, char * );
                 printf("%s", s);
                 break;

              case 'd':
                 i = va_arg( list, int );
                 printf("%d", i);
                 break;

              case 'c':
                 c = va_arg( list, int);
                 printf("%c",c);
                 break;

              default:
                 putc( *fmt, stdout );
                 break;
           }
        }
        ++fmt;
     }
     va_end( list );
  }
  fflush( stdout );
}
