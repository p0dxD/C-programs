#include "hw5.h"

/**
* Uses a mutex to lock an output stream so it is not interleaved when * printed to by different threads.
* @param lock Mutex used to lock output stream.
* @param stream Output stream to write to.
* @param fmt format string used for varargs.
*/
void itoa(long v, char s[], int b) ;

void reverse(char s[]);
void sfwrite(pthread_mutex_t *lock, FILE* stream, char *fmt, ...) {
        pthread_mutex_lock(lock);
        const char *p;
        va_list argp;
        int i;
        char *s;
        char fmtbuf[500];

        va_start(argp, fmt);

        for (p = fmt; *p != '\0'; p++) {
                if (*p != '%') {
                        fputc(*p, stream);
                        continue;
                }

                switch (*++p) {
                case 'c':
                        i = va_arg(argp, int);
                        fputc(i, stream);
                        break;

                case 'd':
                        i = va_arg(argp, int);
                        itoa(i, fmtbuf, 10);
                        fputs(fmtbuf, stream);
                        break;

                case 's':
                        s = va_arg(argp, char *);
                        fputs(s, stream);
                        break;

                case 'x':
                        i = va_arg(argp, int);
                        itoa(i, fmtbuf, 16);
                        fputs(fmtbuf, stream);
                        break;

                case '%':
                        fputc('%', stream);
                        break;
                }
        }
        fputc('\n', stream); /*so we know where it ends*/
        va_end(argp);
        //fclose(stream);/*close the lab*/
        pthread_mutex_unlock(lock);
}

// /*From the rio functions*/
//  void reverse(char s[])
// {
//     int c, i, j;

//     for (i = 0, j = strlen(s)-1; i < j; i++, j--) {
//         c = s[i];
//         s[i] = s[j];
//         s[j] = c;
//     }
// }

// /* sio_ltoa - Convert long to base b string (from K&R) */
//  void itoa(long v, char s[], int b)
// {
//     int c, i = 0;

//     do {
//         s[i++] = ((c = (v % b)) < 10)  ?  c + '0' : c - 10 + 'a';
//     } while ((v /= b) > 0);
//     s[i] = '\0';
//     reverse(s);
// }
