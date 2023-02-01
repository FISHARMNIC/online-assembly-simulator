char **parser_parse(char *str)
{
    char **obuffer = calloc(10, sizeof(char *));
    char **obuffwh = obuffer;

    char *curs = calloc(100, 1);
    char *curswh = curs;

    while (*str != 0)
    {
        if (*str == '[')
        {
            if (strlen(curs) >= 1)
            {
                *curswh = 0;
                *(obuffwh++) = realloc(curs, strlen(curs));
            }
            char *new = malloc(2);
            *new = '[';
            *(new + 1) = 0;
            *(obuffwh++) = new;
            curs = calloc(100, 1);
            curswh = curs;
        }
        else if (*str == ' ' || *str == ',' || *str == ']')
        {
            if (strlen(curs) >= 1)
            {
                *curswh = 0;
                *(obuffwh++) = realloc(curs, strlen(curs));
            }
            curs = calloc(100, 1);
            curswh = curs;
        }
        else
        {
            *(curswh++) = *str;
            // //printf("%c-%c\n", *(curswh-1), *(str-1));
        }
        str++;
    }
    *curswh = 0;
    *(obuffwh) = realloc(curs, strlen(curs));
    return obuffer;
}