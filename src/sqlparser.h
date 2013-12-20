
enum { NAME = 128, CREATE = 129, DATABASE = 130, DROP = 131, USE, TABLE, INTEGER,
		NOT, NULLX, PRIMARY, KEY, AUTO_INCREMENT, VARCHAR, NUMBER };
struct yydata {
	char *strval;
};
#define YY_EXTRA_TYPE struct yydata *

#ifndef YY_NULL
#define YY_NULL 0
typedef void *yyscan_t;
#endif
extern char *yyget_text (yyscan_t yyscanner );
extern int yyget_lineno (yyscan_t yyscanner );
extern int yyget_column  (yyscan_t yyscanner);
extern int yylex_init_extra(YY_EXTRA_TYPE yy_user_defined,yyscan_t* ptr_yy_globals );
extern int yylex (yyscan_t yyscanner);
extern int yylex_destroy  (yyscan_t yyscanner);
