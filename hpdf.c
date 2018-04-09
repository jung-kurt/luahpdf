/* Lua binding to Haru Free PDF Library 2.0.0

$Id: hpdf.c 32002 2010-11-10 12:37:13Z kurt $

*/

#include <hpdf.h>
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>

#ifndef HPDF_SHARED
  typedef void *HPDF_HANDLE;
#endif

#define CnHpdfStr "hpdf"

#define CnUnsupported 0

#define HPDF_CALLBACK __stdcall

#define LclCheckReal(L, P) (HPDF_REAL) luaL_checknumber((L), (P))

#define LclCheckU16(L, P) (HPDF_UINT16) luaL_checkinteger((L), (P))

#if !defined(LUA_VERSION_NUM) || LUA_VERSION_NUM < 502
LUALIB_API void luaL_setfuncs (
  lua_State *L,
  const luaL_Reg *l,
  int nup)

{
  int i;

  luaL_checkstack(L, nup, "too many upvalues");
  for (; l->name != NULL; l++) {  /* fill the table with given functions */
    for (i = 0; i < nup; i++) { /* copy upvalues to the top */
      lua_pushvalue(L, -nup);
    }
    lua_pushcclosure(L, l->func, nup);  /* closure with those upvalues */
    lua_setfield(L, -(nup + 2), l->name);
  }
  lua_pop(L, nup);  /* remove upvalues */
}
#define lua_rawlen lua_objlen
#endif

typedef enum LclHndCatEnum {
  CnHndAnnotation,
  CnHndDestination,
  CnHndDoc,
  CnHndEncoder,
  CnHndExtGState,
  CnHndFont,
  CnHndImage,
  CnHndOutline,
  CnHndPage,
  CnHndXObject
} LclHndCatType;

const char * CnPageLayout[] = {
  "HPDF_PAGE_LAYOUT_SINGLE",
  "HPDF_PAGE_LAYOUT_ONE_COLUMN",
  "HPDF_PAGE_LAYOUT_TWO_COLUMN_LEFT",
  "HPDF_PAGE_LAYOUT_TWO_COLUMN_RIGHT",
  "HPDF_PAGE_LAYOUT_EOF",
  NULL};

const char * CnPageMode[] = {
  "HPDF_PAGE_MODE_USE_NONE",
  "HPDF_PAGE_MODE_USE_OUTLINE",
  "HPDF_PAGE_MODE_USE_THUMBS",
  "HPDF_PAGE_MODE_FULL_SCREEN",
  "HPDF_PAGE_MODE_EOF",
  NULL};

const char * CnPageSizes[] = {
  "HPDF_PAGE_SIZE_LETTER",
  "HPDF_PAGE_SIZE_LEGAL",
  "HPDF_PAGE_SIZE_A3",
  "HPDF_PAGE_SIZE_A4",
  "HPDF_PAGE_SIZE_A5",
  "HPDF_PAGE_SIZE_B4",
  "HPDF_PAGE_SIZE_B5",
  "HPDF_PAGE_SIZE_EXECUTIVE",
  "HPDF_PAGE_SIZE_US4x6",
  "HPDF_PAGE_SIZE_US4x8",
  "HPDF_PAGE_SIZE_US5x7",
  "HPDF_PAGE_SIZE_COMM10",
  "HPDF_PAGE_SIZE_EOF",
  NULL};

const char * CnPageDirection[] = {
  "HPDF_PAGE_PORTRAIT",
  "HPDF_PAGE_LANDSCAPE",
  NULL};

const char * CnPageNumStyle[] = {
  "HPDF_PAGE_NUM_STYLE_DECIMAL",
  "HPDF_PAGE_NUM_STYLE_UPPER_ROMAN",
  "HPDF_PAGE_NUM_STYLE_LOWER_ROMAN",
  "HPDF_PAGE_NUM_STYLE_UPPER_LETTERS",
  "HPDF_PAGE_NUM_STYLE_LOWER_LETTERS",
  "HPDF_PAGE_NUM_STYLE_EOF",
  NULL};

const char * CnEncoderType[] = {
  "HPDF_ENCODER_TYPE_SINGLE_BYTE",
  "HPDF_ENCODER_TYPE_DOUBLE_BYTE",
  "HPDF_ENCODER_TYPE_UNINITIALIZED",
  "HPDF_ENCODER_UNKNOWN",
  NULL};

const char * CnByteType[] = {
  "HPDF_BYTE_TYPE_SINGLE",
  "HPDF_BYTE_TYPE_LEAD",
  "HPDF_BYTE_TYPE_TRIAL",
  "HPDF_BYTE_TYPE_UNKNOWN",
  NULL};

const char * CnWritingMode[] = {
  "HPDF_WMODE_HORIZONTAL",
  "HPDF_WMODE_VERTICAL",
  "HPDF_WMODE_EOF",
  NULL};

const char * CnAnnotHighlightMode[] = {
  "HPDF_ANNOT_NO_HIGHTLIGHT",
  "HPDF_ANNOT_INVERT_BOX",
  "HPDF_ANNOT_INVERT_BORDER",
  "HPDF_ANNOT_DOWN_APPEARANCE",
  "HPDF_ANNOT_HIGHTLIGHT_MODE_EOF",
  NULL};

const char * CnAnnotIcon[] = {
  "HPDF_ANNOT_ICON_COMMENT",
  "HPDF_ANNOT_ICON_KEY",
  "HPDF_ANNOT_ICON_NOTE",
  "HPDF_ANNOT_ICON_HELP",
  "HPDF_ANNOT_ICON_NEW_PARAGRAPH",
  "HPDF_ANNOT_ICON_PARAGRAPH",
  "HPDF_ANNOT_ICON_INSERT",
  "HPDF_ANNOT_ICON_EOF",
  NULL};

const char * CnColorSpace[] = {
  "HPDF_CS_DEVICE_GRAY",
  "HPDF_CS_DEVICE_RGB",
  "HPDF_CS_DEVICE_CMYK",
  "HPDF_CS_CAL_GRAY",
  "HPDF_CS_CAL_RGB",
  "HPDF_CS_LAB",
  "HPDF_CS_ICC_BASED",
  "HPDF_CS_SEPARATION",
  "HPDF_CS_DEVICE_N",
  "HPDF_CS_INDEXED",
  "HPDF_CS_PATTERN",
  "HPDF_CS_EOF",
  NULL};

const char * CnInfoType[] = {
  /* date-time type parameters */
  "HPDF_INFO_CREATION_DATE",
  "HPDF_INFO_MOD_DATE",
  /* string type parameters */
  "HPDF_INFO_AUTHOR",
  "HPDF_INFO_CREATOR",
  "HPDF_INFO_PRODUCER",
  "HPDF_INFO_TITLE",
  "HPDF_INFO_SUBJECT",
  "HPDF_INFO_KEYWORDS",
  "HPDF_INFO_EOF",
  NULL};

const char * CnEncryptMode[] = {
  "",
  "",
  "HPDF_ENCRYPT_R2",
  "HPDF_ENCRYPT_R3",
  NULL};

const char * CnBlendMode[] = {
  "HPDF_BM_NORMAL",
  "HPDF_BM_MULTIPLY",
  "HPDF_BM_SCREEN",
  "HPDF_BM_OVERLAY",
  "HPDF_BM_DARKEN",
  "HPDF_BM_LIGHTEN",
  "HPDF_BM_COLOR_DODGE",
  "HPDF_BM_COLOR_BUM",
  "HPDF_BM_HARD_LIGHT",
  "HPDF_BM_SOFT_LIGHT",
  "HPDF_BM_DIFFERENCE",
  "HPDF_BM_EXCLUSHON",
  "HPDF_BM_EOF",
  NULL};

const char * CnLineCap[] = {
  "HPDF_BUTT_END",
  "HPDF_ROUND_END",
  "HPDF_PROJECTING_SCUARE_END",
  "HPDF_LINECAP_EOF",
  NULL};

const char * CnLineJoin[] = {
  "HPDF_MITER_JOIN",
  "HPDF_ROUND_JOIN",
  "HPDF_BEVEL_JOIN",
  "HPDF_LINEJOIN_EOF",
  NULL};

const char * CnTextRenderingMode[] = {
  "HPDF_FILL",
  "HPDF_STROKE",
  "HPDF_FILL_THEN_STROKE",
  "HPDF_INVISIBLE",
  "HPDF_FILL_CLIPPING",
  "HPDF_STROKE_CLIPPING",
  "HPDF_FILL_STROKE_CLIPPING",
  "HPDF_CLIPPING",
  "HPDF_RENDERING_MODE_EOF",
  NULL};

const char * CnTextAlignment[] = {
  "HPDF_TALIGN_LEFT",
  "HPDF_TALIGN_RIGHT",
  "HPDF_TALIGN_CENTER",
  "HPDF_TALIGN_JUSTIFY",
  NULL};

const char * CnTransitionStyle[] = {
  "HPDF_TS_WIPE_RIGHT",
  "HPDF_TS_WIPE_UP",
  "HPDF_TS_WIPE_LEFT",
  "HPDF_TS_WIPE_DOWN",
  "HPDF_TS_BARN_DOORS_HORIZONTAL_OUT",
  "HPDF_TS_BARN_DOORS_HORIZONTAL_IN",
  "HPDF_TS_BARN_DOORS_VERTICAL_OUT",
  "HPDF_TS_BARN_DOORS_VERTICAL_IN",
  "HPDF_TS_BOX_OUT",
  "HPDF_TS_BOX_IN",
  "HPDF_TS_BLINDS_HORIZONTAL",
  "HPDF_TS_BLINDS_VERTICAL",
  "HPDF_TS_DISSOLVE",
  "HPDF_TS_GLITTER_RIGHT",
  "HPDF_TS_GLITTER_DOWN",
  "HPDF_TS_GLITTER_TOP_LEFT_TO_BOTTOM_RIGHT",
  "HPDF_TS_REPLACE",
  "HPDF_TS_EOF",
  NULL};

typedef struct LclErrMsgStruct
{
  int value;
  const char * name;
} LclErrMsgType;

const LclErrMsgType CnErrorMessages[] = {
  {HPDF_ARRAY_COUNT_ERR, "Internal error. Data consistency was lost."},
  {HPDF_ARRAY_ITEM_NOT_FOUND, "Internal error. Data consistency was lost."},
  {HPDF_ARRAY_ITEM_UNEXPECTED_TYPE, "Internal error. Data consistency was lost."},
  {HPDF_BINARY_LENGTH_ERR, "Data length > HPDF_LIMIT_MAX_STRING_LEN."},
  {HPDF_CANNOT_GET_PALLET, "Cannot get pallet data from PNG image."},
  {HPDF_DICT_COUNT_ERR, "Dictionary elements > HPDF_LIMIT_MAX_DICT_ELEMENT"},
  {HPDF_DICT_ITEM_NOT_FOUND, "Internal error. Data consistency was lost."},
  {HPDF_DICT_ITEM_UNEXPECTED_TYPE, "Internal error. Data consistency was lost."},
  {HPDF_DICT_STREAM_LENGTH_NOT_FOUND, "Internal error. Data consistency was lost."},
  {HPDF_DOC_ENCRYPTDICT_NOT_FOUND, "HPDF_SetEncryptMode() or HPDF_SetPermission() called before password set."},
  {HPDF_DOC_INVALID_OBJECT, "Internal error. Data consistency was lost."},
  {HPDF_DUPLICATE_REGISTRATION, "Tried to re-register a registered font."},
  {HPDF_EXCEED_JWW_CODE_NUM_LIMIT, "Cannot register a character to the Japanese word wrap characters list."},
  {HPDF_ENCRYPT_INVALID_PASSWORD, "1. Tried to set the owner password to NULL.\n"
    "2. Owner and user password are the same."},
  {HPDF_ERR_UNKNOWN_CLASS, "Internal error. Data consistency was lost."},
  {HPDF_EXCEED_GSTATE_LIMIT, "Stack depth > HPDF_LIMIT_MAX_GSTATE."},
  {HPDF_FAILD_TO_ALLOC_MEM, "Memory allocation failed."},
  {HPDF_FILE_IO_ERROR, "File processing failed. (Detailed code is set.)"},
  {HPDF_FILE_OPEN_ERROR, "Cannot open a file. (Detailed code is set.)"},
  {HPDF_FONT_EXISTS, "Tried to load a font that has been registered."},
  {HPDF_FONT_INVALID_WIDTHS_TABLE, "1. Font-file format is invalid.\n"
    "2. Internal error. Data consistency was lost."},
  {HPDF_INVALID_AFM_HEADER, "Cannot recognize header of afm file."},
  {HPDF_INVALID_ANNOTATION, "Specified annotation handle is invalid."},
  {HPDF_INVALID_BIT_PER_COMPONENT, "Bit-per-component of a image which was set as mask-image is invalid."},
  {HPDF_INVALID_CHAR_MATRICS_DATA, "Cannot recognize char-matrics-data of afm file."},
  {HPDF_INVALID_COLOR_SPACE, "1. Invalid color_space parameter of HPDF_LoadRawImage.\n"
    "2. Color-space of a image which was set as mask-image is invalid.\n"
    "3. Invoked function invalid in present color-space."},
  {HPDF_INVALID_COMPRESSION_MODE, "Invalid value set when invoking HPDF_SetCommpressionMode()."},
  {HPDF_INVALID_DATE_TIME, "An invalid date-time value was set."},
  {HPDF_INVALID_DESTINATION, "An invalid destination handle was set."},
  {HPDF_INVALID_DOCUMENT, "An invalid document handle was set."},
  {HPDF_INVALID_DOCUMENT_STATE, "Function invalid in the present state was invoked."},
  {HPDF_INVALID_ENCODER, "An invalid encoder handle was set."},
  {HPDF_INVALID_ENCODER_TYPE, "Combination between font and encoder is wrong."},
  {HPDF_INVALID_ENCODING_NAME, "An Invalid encoding name is specified."},
  {HPDF_INVALID_ENCRYPT_KEY_LEN, "Encryption key length is invalid."},
  {HPDF_INVALID_FONTDEF_DATA, "1. An invalid font handle was set.\n"
    "2. Unsupported font format."},
  {HPDF_INVALID_FONTDEF_TYPE, "Internal error. Data consistency was lost."},
  {HPDF_INVALID_FONT_NAME, "Font with the specified name is not found."},
  {HPDF_INVALID_IMAGE, "Unsupported image format."},
  {HPDF_INVALID_JPEG_DATA, "Unsupported image format."},
  {HPDF_INVALID_N_DATA, "Cannot read a postscript-name from an afm file."},
  {HPDF_INVALID_OBJECT, "1. An invalid object is set.\n"
    "2. Internal error. Data consistency was lost."},
  {HPDF_INVALID_OBJ_ID, "Internal error. Data consistency was lost."},
  {HPDF_INVALID_OPERATION, "Invoked HPDF_Image_SetColorMask() against the image-object which was set a mask-image."},
  {HPDF_INVALID_OUTLINE, "An invalid outline-handle was specified."},
  {HPDF_INVALID_PAGE, "An invalid page-handle was specified."},
  {HPDF_INVALID_PAGES, "An invalid pages-handle was specified. (internal error)"},
  {HPDF_INVALID_PARAMETER, "An invalid value is set."},
  {HPDF_INVALID_PNG_IMAGE, "Invalid PNG image format."},
  {HPDF_INVALID_STREAM, "Internal error. Data consistency was lost."},
  {HPDF_MISSING_FILE_NAME_ENTRY, "Internal error. \"_FILE_NAME\" entry for delayed loading is missing."},
  {HPDF_INVALID_TTC_FILE, "Invalid .TTC file format."},
  {HPDF_INVALID_TTC_INDEX, "Index parameter > number of included fonts."},
  {HPDF_INVALID_WX_DATA, "Cannot read a width-data from an afm file."},
  {HPDF_ITEM_NOT_FOUND, "Internal error. Data consistency was lost."},
  {HPDF_LIBPNG_ERROR, "Error returned from PNGLIB while loading image."},
  {HPDF_NAME_INVALID_VALUE, "Internal error. Data consistency was lost."},
  {HPDF_NAME_OUT_OF_RANGE, "Internal error. Data consistency was lost."},
  {HPDF_PAGES_MISSING_KIDS_ENTRY, "Internal error. Data consistency was lost."},
  {HPDF_PAGE_CANNOT_FIND_OBJECT, "Internal error. Data consistency was lost."},
  {HPDF_PAGE_CANNOT_GET_ROOT_PAGES, "Internal error. Data consistency was lost."},
  {HPDF_PAGE_CANNOT_RESTORE_GSTATE, "There are no graphics-states to be restored."},
  {HPDF_PAGE_CANNOT_SET_PARENT, "Internal error. Data consistency was lost."},
  {HPDF_PAGE_FONT_NOT_FOUND, "The current font is not set."},
  {HPDF_PAGE_INVALID_FONT, "An invalid font-handle was specified."},
  {HPDF_PAGE_INVALID_FONT_SIZE, "An invalid font-size was set."},
  {HPDF_PAGE_INVALID_GMODE, "Invalid Graphics mode."},
  {HPDF_PAGE_INVALID_INDEX, "Internal error. Data consistency was lost."},
  {HPDF_PAGE_INVALID_ROTATE_VALUE, "Specified value is not multiple of 90."},
  {HPDF_PAGE_INVALID_SIZE, "An invalid page-size was set."},
  {HPDF_PAGE_INVALID_XOBJECT, "An invalid image-handle was set."},
  {HPDF_PAGE_OUT_OF_RANGE, "The specified value is out of range."},
  {HPDF_REAL_OUT_OF_RANGE, "The specified value is out of range."},
  {HPDF_STREAM_EOF, "Unexpected EOF marker was detected."},
  {HPDF_STREAM_READLN_CONTINUE, "Internal error. Data consistency was lost."},
  {HPDF_STRING_OUT_OF_RANGE, "The length of the text is too long."},
  {HPDF_THIS_FUNC_WAS_SKIPPED, "Function not executed because of other errors."},
  {HPDF_TTF_CANNOT_EMBEDDING_FONT, "Font cannot be embedded. (license restriction)"},
  {HPDF_TTF_INVALID_CMAP, "Unsupported ttf format. (cannot find unicode cmap)"},
  {HPDF_TTF_INVALID_FOMAT, "Unsupported ttf format."},
  {HPDF_TTF_MISSING_TABLE, "Unsupported ttf format. (cannot find a necessary table)"},
  {HPDF_UNSUPPORTED_FONT_TYPE, "Internal error. Data consistency was lost."},
  {HPDF_UNSUPPORTED_FUNC, "1. Library not configured to use PNGLIB.\n"
    "2. Internal error. Data consistency was lost."},
  {HPDF_UNSUPPORTED_JPEG_FORMAT, "Unsupported JPEG format."},
  {HPDF_UNSUPPORTED_TYPE1_FONT, "Failed to parse .PFB file."},
  {HPDF_XREF_COUNT_ERR, "Internal error. Data consistency was lost."},
  {HPDF_ZLIB_ERROR, "Error while executing ZLIB function."},
  {HPDF_INVALID_PAGE_INDEX, "An invalid page index was passed."},
  {HPDF_INVALID_URI, "An invalid URI was set."},
  /*{HPDF_PAGELAYOUT_OUT_OF_RANGE, "An invalid page-layout was set."},*/
  /*{HPDF_PAGEMODE_OUT_OF_RANGE, "An invalid page-mode was set."},*/
  /*{HPDF_PAGENUM_STYLE_OUT_OF_RANGE, "An invalid page-num-style was set."},*/
  {HPDF_ANNOT_INVALID_ICON, "An invalid icon was set."},
  {HPDF_ANNOT_INVALID_BORDER_STYLE, "An invalid border-style was set."},
  {HPDF_PAGE_INVALID_DIRECTION, "An invalid page-direction was set."},
  {HPDF_INVALID_FONT, "An invalid font-handle was specified."},
  {0, NULL}};

/* * */

void HPDF_STDCALL LclErrHandler(
  HPDF_STATUS ErrNum,
  HPDF_STATUS DetailNum,
  void * UserPtr)

{
  char ErrStr[128];
  lua_State * L = (lua_State *) UserPtr;
  const LclErrMsgType * it;

  /* TODO
  [ ] Make error more descriptive
  [ ] Use Lua concatentation instead of sprintf
  */

  for (it=CnErrorMessages; it->name; ++it)
  {
    if (it->value==ErrNum)
    {
      lua_pushstring(L, it->name);
      if (DetailNum != 0)
      {
        lua_pushfstring(L, "(detail %d)", DetailNum);
        lua_concat(L, 2);
      }
      break;
    }
  }
  if (!it->name)
  {
    sprintf(ErrStr, "HPDF error 0x%04X, detail %u", (HPDF_UINT) ErrNum,
      (HPDF_UINT) DetailNum);
    lua_pushstring(L, ErrStr);
  }
  lua_error(L);
}

/* * */

static void LclHandlePush(
  lua_State * L,
  HPDF_HANDLE Hnd,
  LclHndCatType Cat)

{
  /* Stk: ...  */
  lua_pushvalue(L, lua_upvalueindex(1));
  /* Stk: ... HndTbl */
  lua_pushlightuserdata(L, Hnd);
  /* Stk: ... HndTbl Hnd */
  lua_pushinteger(L, Cat);
  /* Stk: ... HndTbl Hnd Cat */
  lua_rawset(L, -3);
  /* Stk: ... HndTbl */
  lua_pop(L, 1);
  /* Stk: ...  */
  lua_pushlightuserdata(L, Hnd);
  /* Stk: ... Hnd */
}

/* * */

static HPDF_HANDLE LclHandleGet(
  lua_State * L,
  LclHndCatType Cat,
  int StkPos)

{
  HPDF_HANDLE Hnd;
  int ChkCat;

  luaL_argcheck(L, lua_isuserdata(L, StkPos), StkPos, "expecting handle");
  Hnd = lua_touserdata(L, StkPos);
  /* Stk: ...  */
  lua_pushvalue(L, lua_upvalueindex(1));
  /* Stk: ... HndTbl */
  lua_pushlightuserdata(L, Hnd);
  /* Stk: ... HndTbl Hnd */
  lua_rawget(L, -2);
  /* Stk: ... HndTbl HndCat? */
  luaL_argcheck(L, lua_isnumber(L, -1), StkPos, "expecting HPDF handle");
  ChkCat = lua_tointeger(L, -1);
  luaL_argcheck(L, (Cat == ChkCat), StkPos, "unexpected HPDF handle type");
  lua_pop(L, 2);
  return Hnd;
}

/* * */

static HPDF_HANDLE LclOptHandleGet(
  lua_State * L,
  LclHndCatType Cat,
  int StkPos)

  /* If handle is missing or nil at StkPos, return (HPDF_HANDLE) 0. */

{
  int Tp = lua_type(L, StkPos);
  return ((LUA_TNIL == Tp) || (LUA_TNONE == Tp)) ? (HPDF_HANDLE) 0 :
    LclHandleGet(L, Cat, StkPos);
}

/* * */

static HPDF_REAL LclTblNum(
  lua_State * L,
  const char * NameStr)

{
  HPDF_REAL Num = 0;

  /* Stk: ... Tbl */
  lua_pushstring(L, NameStr);
  /* Stk: ... Tbl NameStr */
  lua_rawget(L, -2);
  /* Stk: ... Tbl Num? */
  if (lua_isnumber(L, -1)) {
    Num = (HPDF_REAL) lua_tonumber(L, -1);
  } else {
    luaL_error(L, "field " LUA_QS " not found", NameStr);
  }
  lua_pop(L, 1);
  /* Stk: ... Tbl */
  return Num;
}

/* * */

static HPDF_INT LclTblIntOpt(
  lua_State * L,
  const char * NameStr,
  lua_Integer Default)

{
  HPDF_INT Num = 0;

  /* Stk: ... Tbl */
  lua_pushstring(L, NameStr);
  /* Stk: ... Tbl NameStr */
  lua_rawget(L, -2);
  /* Stk: ... Tbl Num? */
  if (lua_isnumber(L, -1)) {
    Num = lua_tointeger(L, -1);
  } else {
    Num = Default;
  }
  lua_pop(L, 1);
  /* Stk: ... Tbl */
  return Num;
}

/* * */

static HPDF_INT LclTblInt(
  lua_State * L,
  const char * NameStr)

{
  HPDF_INT Num = 0;

  /* Stk: ... Tbl */
  lua_pushstring(L, NameStr);
  /* Stk: ... Tbl NameStr */
  lua_rawget(L, -2);
  /* Stk: ... Tbl Num? */
  if (lua_isnumber(L, -1)) {
    Num = lua_tointeger(L, -1);
  } else {
    luaL_error(L, "field " LUA_QS " not found", NameStr);
  }
  lua_pop(L, 1);
  /* Stk: ... Tbl */
  return Num;
}

/* * */

static char LclTblChar(
  lua_State * L,
  const char * NameStr)

{
  const char * Str;
  char Ch = ' ';

  /* Stk: ... RctTbl */
  lua_pushstring(L, NameStr);
  /* Stk: ... RctTbl NameStr */
  lua_rawget(L, -2);
  /* Stk: ... RctTbl Str? */
  Str = lua_tostring(L, -1);
  if (! Str) {
    luaL_error(L, "expecting character field " LUA_QS, NameStr);
  } else {
    Ch = *Str;
  }
  lua_pop(L, 1);
  /* Stk: ... RctTbl */
  return Ch;
}

/* * */

static void LclRectGet(
  lua_State * L,
  HPDF_Rect * RctPtr,
  int StkPos)

{
  /* Stk: ... */
  luaL_argcheck(L, lua_istable(L, StkPos), StkPos, "expecting rectangle table");
  lua_pushvalue(L, StkPos);
  /* Stk: ... RctTbl? */
  RctPtr->left = LclTblNum(L, "left");
  RctPtr->bottom = LclTblNum(L, "bottom");
  RctPtr->right = LclTblNum(L, "right");
  RctPtr->top = LclTblNum(L, "top");
  /* Stk: ... RctTbl */
  lua_pop(L, 1);
  /* Stk: ...  */
}

/* * */

static void LclSetNumField(
  lua_State * L,
  HPDF_REAL Num,
  const char * FldStr)

  /* Sets field T[FldStr] = Num where T is table at top of stack */

{
  /* Stk: ... Tbl */
  lua_pushstring(L, FldStr);
  /* Stk: ... Tbl FldStr */
  lua_pushnumber(L, Num);
  /* Stk: ... Tbl FldStr Num */
  lua_rawset(L, -3);
  /* Stk: ... Tbl */
}

/* * */

static void LclSetIntField(
  lua_State * L,
  HPDF_UINT Num,
  const char * FldStr)

  /* Sets field T[FldStr] = Num where T is table at top of stack */

{
  /* Stk: ... Tbl */
  lua_pushinteger(L, Num);
  /* Stk: ... Tbl Num */
  lua_setfield(L, -2, FldStr);
  /* Stk: ... Tbl */
}

/* * */

static void LclTransMatrixPush(
  lua_State * L,
  HPDF_TransMatrix Tm)

{
  /* Stk: ... */
  lua_newtable(L);
  /* Stk: ... Tbl */
  LclSetNumField(L, Tm.a, "a");
  LclSetNumField(L, Tm.b, "b");
  LclSetNumField(L, Tm.c, "c");
  LclSetNumField(L, Tm.d, "d");
  LclSetNumField(L, Tm.x, "x");
  LclSetNumField(L, Tm.y, "y");
  /* Stk: ... Tbl */
}

/* * */

static void LclPointPush(
  lua_State * L,
  HPDF_Point Pt)

{
  /* Stk: ... */
  lua_newtable(L);
  /* Stk: ... Tbl */
  LclSetNumField(L, Pt.x, "x");
  LclSetNumField(L, Pt.y, "y");
  /* Stk: ... Tbl */
}

/* * */

static void LclRgbPush(
  lua_State * L,
  HPDF_RGBColor Clr)

{
  /* Stk: ... */
  lua_newtable(L);
  /* Stk: ... Tbl */
  LclSetNumField(L, Clr.r, "r");
  LclSetNumField(L, Clr.g, "g");
  LclSetNumField(L, Clr.b, "b");
  /* Stk: ... Tbl */
}

/* * */

static void LclCmykPush(
  lua_State * L,
  HPDF_CMYKColor Clr)

{
  /* Stk: ... */
  lua_newtable(L);
  /* Stk: ... Tbl */
  LclSetNumField(L, Clr.c, "c");
  LclSetNumField(L, Clr.m, "m");
  LclSetNumField(L, Clr.y, "y");
  LclSetNumField(L, Clr.k, "k");
  /* Stk: ... Tbl */
}

/* * */

static void LclDateGet(
  lua_State * L,
  HPDF_Date * DatePtr,
  int StkPos)

{
  /* Stk: ... */
  luaL_argcheck(L, lua_istable(L, StkPos), StkPos, "expecting date table");
  lua_pushvalue(L, StkPos);
  /* Stk: ... DateTbl? */
  DatePtr->year = LclTblInt(L, "year");
  DatePtr->month = LclTblInt(L, "month");
  DatePtr->day = LclTblInt(L, "day");
  DatePtr->hour = LclTblInt(L, "hour");
  DatePtr->minutes = LclTblInt(L, "minutes");
  DatePtr->seconds = LclTblInt(L, "seconds");
  DatePtr->ind = LclTblChar(L, "ind");
  DatePtr->off_hour = LclTblIntOpt(L, "off_hour", 0);
  DatePtr->off_minutes = LclTblIntOpt(L, "off_minutes", 0);
  /* Stk: ... DateTbl */
  lua_pop(L, 1);
  /* Stk: ...  */
}

/* * */

static int LclGetHandleTable(lua_State *L)

  /* handle table <- hpdf.GetHandleTable() */

{
  /* Stk: ...  */
    lua_pushvalue(L, lua_upvalueindex(1));
  /* Stk: ... HndTbl */
  return 1;
}

/* * */

static int LclGetVersion(lua_State *L)

  /* string <- hpdf.GetVersion() */

{
  const char * result = HPDF_GetVersion();
  lua_pushstring(L, result);
  return 1;
}

/* * */

#if CnUnsupported

static int LclNewEx(lua_State *L)

  /* HPDF_Doc <- hpdf.NewEx(user_error_fn, user_alloc_fn,
       user_free_fn, mem_pool_buf_size, user_data) */

{
  HPDF_Error_Handler user_error_fn = LclHandleGet(L, "HPDF_Error_Handler", 1);
  HPDF_Alloc_Func user_alloc_fn = LclHandleGet(L, "HPDF_Alloc_Func", 2);
  HPDF_Free_Func user_free_fn = LclHandleGet(L, "HPDF_Free_Func", 3);
  HPDF_UINT mem_pool_buf_size = luaL_checkinteger(L, 4);
  /*  id * user_data  ! Get argument 5 (void *) here */
  HPDF_Doc result = HPDF_NewEx(user_error_fn, user_alloc_fn,
    user_free_fn, mem_pool_buf_size, user_data);
  LclHandlePush(L, result, CnHndDoc);
  return 1;
}

#endif

/* * */

#if CnUnsupported

static int LclNew(lua_State *L)

  /* HPDF_Doc <- hpdf.New(user_error_fn, user_data)
  API change
  Doc <- hpdf.New() */

{
  HPDF_Error_Handler user_error_fn = LclHandleGet(L, "HPDF_Error_Handler", 1);
  /*  void * user_data  ! Get argument 2 (void *) here */
  HPDF_Doc result = HPDF_New(user_error_fn, user_data);
  LclHandlePush(L, result, CnHndDoc);
  return 1;
}

#endif

/* * */

static int LclNew(lua_State *L)

  /* HPDF_Doc <- hpdf.New() */

{
  HPDF_Doc result = HPDF_New(LclErrHandler, L);
  LclHandlePush(L, result, CnHndDoc);
  return 1;
}

/* * */

#if CnUnsupported

static int LclSetErrorHandler(lua_State *L)

  /* HPDF_STATUS <- hpdf.SetErrorHandler(pdf, user_error_fn) */

{
  HPDF_Doc pdf = LclHandleGet(L, CnHndDoc, 1);
  HPDF_Error_Handler user_error_fn = LclHandleGet(L, "HPDF_Error_Handler", 2);
  HPDF_STATUS result = HPDF_SetErrorHandler(pdf, user_error_fn);
  lua_pushinteger(L, result);
  return 1;
}

#endif

/* * */

static int LclFree(lua_State *L)

  /* hpdf.Free(pdf) */

{
  HPDF_Doc pdf = LclHandleGet(L, CnHndDoc, 1);
  HPDF_Free(pdf);
  return 0;
}

/* * */

static int LclNewDoc(lua_State *L)

  /* HPDF_STATUS <- hpdf.NewDoc(pdf) */

{
  HPDF_Doc pdf = LclHandleGet(L, CnHndDoc, 1);
  HPDF_STATUS result = HPDF_NewDoc(pdf);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclFreeDoc(lua_State *L)

  /* hpdf.FreeDoc(pdf) */

{
  HPDF_Doc pdf = LclHandleGet(L, CnHndDoc, 1);
  HPDF_FreeDoc(pdf);
  return 0;
}

/* * */

static int LclHasDoc(lua_State *L)

  /* boolean <- hpdf.HasDoc(pdf) */

{
  HPDF_Doc pdf = LclHandleGet(L, CnHndDoc, 1);
  HPDF_BOOL result = HPDF_HasDoc(pdf);
  lua_pushboolean(L, result);
  return 1;
}

/* * */

static int LclFreeDocAll(lua_State *L)

  /* hpdf.FreeDocAll(pdf) */

{
  HPDF_Doc pdf = LclHandleGet(L, CnHndDoc, 1);
  HPDF_FreeDocAll(pdf);
  return 0;
}

/* * */

static int LclSaveToStream(lua_State *L)

  /* HPDF_STATUS <- hpdf.SaveToStream(pdf) */

{
  HPDF_Doc pdf = LclHandleGet(L, CnHndDoc, 1);
  HPDF_STATUS result = HPDF_SaveToStream(pdf);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclGetStreamSize(lua_State *L)

  /* integer <- hpdf.GetStreamSize(pdf) */

{
  HPDF_Doc pdf = LclHandleGet(L, CnHndDoc, 1);
  HPDF_UINT32 result = HPDF_GetStreamSize(pdf);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

#if CnUnsupported

static int LclReadFromStream(lua_State *L)

  /* HPDF_STATUS <- hpdf.ReadFromStream(pdf, buf, size) */

{
  HPDF_Doc pdf = LclHandleGet(L, CnHndDoc, 1);
/*  HPDF_BYTE * buf  ! Get argument 2 (HPDF_BYTE *) here */
/*  HPDF_UINT32 * size  ! Get argument 3 (HPDF_UINT32 *) here */
  HPDF_STATUS result = HPDF_ReadFromStream(pdf, buf, size);
  lua_pushinteger(L, result);
  return 1;
}

#endif

/* * */

static int LclResetStream(lua_State *L)

  /* HPDF_STATUS <- hpdf.ResetStream(pdf) */

{
  HPDF_Doc pdf = LclHandleGet(L, CnHndDoc, 1);
  HPDF_STATUS result = HPDF_ResetStream(pdf);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclSaveToFile(lua_State *L)

  /* HPDF_STATUS <- hpdf.SaveToFile(pdf, file_name) */

{
  FILE * fl;
  HPDF_STATUS result;
  HPDF_Doc pdf = LclHandleGet(L, CnHndDoc, 1);
  const char * file_name = luaL_checkstring(L, 2);
  fl = fopen(file_name, "we");
  if (fl) {
    fclose(fl);
    result = HPDF_SaveToFile(pdf, file_name);
  } else {
    result = 0x1016;
  }
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclGetError(lua_State *L)

  /* HPDF_STATUS <- hpdf.GetError(pdf) */

{
  HPDF_Doc pdf = LclHandleGet(L, CnHndDoc, 1);
  HPDF_STATUS result = HPDF_GetError(pdf);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclGetErrorDetail(lua_State *L)

  /* HPDF_STATUS <- hpdf.GetErrorDetail(pdf) */

{
  HPDF_Doc pdf = LclHandleGet(L, CnHndDoc, 1);
  HPDF_STATUS result = HPDF_GetErrorDetail(pdf);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclResetError(lua_State *L)

  /* hpdf.ResetError(pdf) */

{
  HPDF_Doc pdf = LclHandleGet(L, CnHndDoc, 1);
  HPDF_ResetError(pdf);
  return 0;
}

/* * */

static int LclSetPagesConfiguration(lua_State *L)

  /* HPDF_STATUS <- hpdf.SetPagesConfiguration(pdf, page_per_pages) */

{
  HPDF_Doc pdf = LclHandleGet(L, CnHndDoc, 1);
  HPDF_UINT page_per_pages = luaL_checkinteger(L, 2);
  HPDF_STATUS result = HPDF_SetPagesConfiguration(pdf, page_per_pages);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclGetPageByIndex(lua_State *L)

  /* HPDF_Page <- hpdf.GetPageByIndex(pdf, index) */

{
  HPDF_Doc pdf = LclHandleGet(L, CnHndDoc, 1);
  HPDF_UINT index = luaL_checkinteger(L, 2);
  HPDF_Page result = HPDF_GetPageByIndex(pdf, index);
  LclHandlePush(L, result, CnHndPage);
  return 1;
}

/* * */

static int LclGetPageLayout(lua_State *L)

  /* HPDF_PageLayout <- hpdf.GetPageLayout(pdf) */

{
  HPDF_Doc pdf = LclHandleGet(L, CnHndDoc, 1);
  HPDF_PageLayout result = HPDF_GetPageLayout(pdf);
  lua_pushstring(L, CnPageLayout[result]);
  return 1;
}

/* * */

static int LclSetPageLayout(lua_State *L)

  /* HPDF_STATUS <- hpdf.SetPageLayout(pdf, layout) */

{
  HPDF_Doc pdf = LclHandleGet(L, CnHndDoc, 1);
  HPDF_PageLayout layout = luaL_checkoption(L, 2,
    "HPDF_PAGE_LAYOUT_SINGLE", CnPageLayout);
  HPDF_STATUS result = HPDF_SetPageLayout(pdf, layout);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclGetPageMode(lua_State *L)

  /* HPDF_PageMode <- hpdf.GetPageMode(pdf) */

{
  HPDF_Doc pdf = LclHandleGet(L, CnHndDoc, 1);
  HPDF_PageMode result = HPDF_GetPageMode(pdf);
  lua_pushstring(L, CnPageMode[result]);
  return 1;
}

/* * */

static int LclSetPageMode(lua_State *L)

  /* HPDF_STATUS <- hpdf.SetPageMode(pdf, mode) */

{
  HPDF_Doc pdf = LclHandleGet(L, CnHndDoc, 1);
  HPDF_PageMode mode = luaL_checkoption(L, 2,
    "HPDF_PAGE_MODE_USE_NONE", CnPageMode);
  HPDF_STATUS result = HPDF_SetPageMode(pdf, mode);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclGetViewerPreference(lua_State *L)

  /* integer <- hpdf.GetViewerPreference(pdf) */

{
  HPDF_Doc pdf = LclHandleGet(L, CnHndDoc, 1);
  HPDF_UINT result = HPDF_GetViewerPreference(pdf);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclSetViewerPreference(lua_State *L)

  /* HPDF_STATUS <- hpdf.SetViewerPreference(pdf, value) */

{
  HPDF_Doc pdf = LclHandleGet(L, CnHndDoc, 1);
  HPDF_UINT value = luaL_checkinteger(L, 2);
  HPDF_STATUS result = HPDF_SetViewerPreference(pdf, value);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclSetOpenAction(lua_State *L)

  /* HPDF_STATUS <- hpdf.SetOpenAction(pdf, open_action) */

{
  HPDF_Doc pdf = LclHandleGet(L, CnHndDoc, 1);
  HPDF_Destination open_action = LclHandleGet(L, CnHndDestination, 2);
  HPDF_STATUS result = HPDF_SetOpenAction(pdf, open_action);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclGetCurrentPage(lua_State *L)

  /* HPDF_Page <- hpdf.GetCurrentPage(pdf) */

{
  HPDF_Doc pdf = LclHandleGet(L, CnHndDoc, 1);
  HPDF_Page result = HPDF_GetCurrentPage(pdf);
  LclHandlePush(L, result, CnHndPage);
  return 1;
}

/* * */

static int LclAddPage(lua_State *L)

  /* HPDF_Page <- hpdf.AddPage(pdf) */

{
  HPDF_Doc pdf = LclHandleGet(L, CnHndDoc, 1);
  HPDF_Page result = HPDF_AddPage(pdf);
  LclHandlePush(L, result, CnHndPage);
  return 1;
}

/* * */

static int LclInsertPage(lua_State *L)

  /* HPDF_Page <- hpdf.InsertPage(pdf, page) */

{
  HPDF_Doc pdf = LclHandleGet(L, CnHndDoc, 1);
  HPDF_Page page = LclHandleGet(L, CnHndPage, 2);
  HPDF_Page result = HPDF_InsertPage(pdf, page);
  LclHandlePush(L, result, CnHndPage);
  return 1;
}

/* * */

static int LclPage_SetWidth(lua_State *L)

  /* HPDF_STATUS <- hpdf.Page_SetWidth(page, value) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_REAL value = LclCheckReal(L, 2);
  HPDF_STATUS result = HPDF_Page_SetWidth(page, value);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclPage_SetHeight(lua_State *L)

  /* HPDF_STATUS <- hpdf.Page_SetHeight(page, value) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_REAL value = LclCheckReal(L, 2);
  HPDF_STATUS result = HPDF_Page_SetHeight(page, value);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclPage_SetSize(lua_State *L)

  /* HPDF_STATUS <- hpdf.Page_SetSize(page, size, direction) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_PageSizes size = luaL_checkoption(L, 2, "HPDF_PAGE_SIZE_LETTER",
    CnPageSizes);
  HPDF_PageDirection direction = luaL_checkoption(L, 3,
    "HPDF_PAGE_PORTRAIT", CnPageDirection);
  HPDF_STATUS result = HPDF_Page_SetSize(page, size, direction);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclPage_SetRotate(lua_State *L)

  /* HPDF_STATUS <- hpdf.Page_SetRotate(page, angle) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_UINT16 angle = LclCheckU16(L, 2);
  HPDF_STATUS result = HPDF_Page_SetRotate(page, angle);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclGetFont(lua_State *L)

  /* HPDF_Font <- hpdf.GetFont(pdf, font_name, encoding_name) */

{
  HPDF_Doc pdf = LclHandleGet(L, CnHndDoc, 1);
  const char * font_name = luaL_checkstring(L, 2);
  const char * encoding_name = luaL_optstring(L, 3, (char *) 0);
  HPDF_Font result = HPDF_GetFont(pdf, font_name, encoding_name);
  LclHandlePush(L, result, CnHndFont);
  return 1;
}

/* * */

static int LclLoadType1FontFromFile(lua_State *L)

  /* string <- hpdf.LoadType1FontFromFile(pdf, afm_file_name, data_file_name) */

{
  HPDF_Doc pdf = LclHandleGet(L, CnHndDoc, 1);
  const char * afm_file_name = luaL_checkstring(L, 2);
  const char * data_file_name = luaL_checkstring(L, 3);
  const char * result = HPDF_LoadType1FontFromFile(pdf,
    afm_file_name, data_file_name);
  lua_pushstring(L, result);
  return 1;
}

/* * */

static int LclLoadTTFontFromFile(lua_State *L)

  /* string <- hpdf.LoadTTFontFromFile(pdf, file_name, embedding) */

{
  HPDF_Doc pdf = LclHandleGet(L, CnHndDoc, 1);
  const char * file_name = luaL_checkstring(L, 2);
  HPDF_BOOL embedding = lua_toboolean(L, 3);
  const char * result = HPDF_LoadTTFontFromFile(pdf, file_name, embedding);
  lua_pushstring(L, result);
  return 1;
}

/* * */

static int LclLoadTTFontFromFile2(lua_State *L)

  /* string <- hpdf.LoadTTFontFromFile2(pdf, file_name, index, embedding) */

{
  HPDF_Doc pdf = LclHandleGet(L, CnHndDoc, 1);
  const char * file_name = luaL_checkstring(L, 2);
  HPDF_UINT index = luaL_checkinteger(L, 3);
  HPDF_BOOL embedding = lua_toboolean(L, 4);
  const char * result = HPDF_LoadTTFontFromFile2(pdf,
    file_name, index, embedding);
  lua_pushstring(L, result);
  return 1;
}

/* * */

static int LclAddPageLabel(lua_State *L)

  /* HPDF_STATUS <- hpdf.AddPageLabel(pdf, page_num, style, first_page, prefix) */

{
  HPDF_Doc pdf = LclHandleGet(L, CnHndDoc, 1);
  HPDF_UINT page_num = luaL_checkinteger(L, 2);
  HPDF_PageNumStyle style = luaL_checkoption(L, 3,
    "HPDF_PAGE_NUM_STYLE_DECIMAL", CnPageNumStyle);
  HPDF_UINT first_page = luaL_checkinteger(L, 4);
  const char * prefix = luaL_checkstring(L, 5);
  HPDF_STATUS result = HPDF_AddPageLabel(pdf, page_num,
    style, first_page, prefix);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclUseJPFonts(lua_State *L)

  /* HPDF_STATUS <- hpdf.UseJPFonts(pdf) */

{
  HPDF_Doc pdf = LclHandleGet(L, CnHndDoc, 1);
  HPDF_STATUS result = HPDF_UseJPFonts(pdf);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclUseKRFonts(lua_State *L)

  /* HPDF_STATUS <- hpdf.UseKRFonts(pdf) */

{
  HPDF_Doc pdf = LclHandleGet(L, CnHndDoc, 1);
  HPDF_STATUS result = HPDF_UseKRFonts(pdf);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclUseCNSFonts(lua_State *L)

  /* HPDF_STATUS <- hpdf.UseCNSFonts(pdf) */

{
  HPDF_Doc pdf = LclHandleGet(L, CnHndDoc, 1);
  HPDF_STATUS result = HPDF_UseCNSFonts(pdf);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclUseCNTFonts(lua_State *L)

  /* HPDF_STATUS <- hpdf.UseCNTFonts(pdf) */

{
  HPDF_Doc pdf = LclHandleGet(L, CnHndDoc, 1);
  HPDF_STATUS result = HPDF_UseCNTFonts(pdf);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclCreateOutline(lua_State *L)

  /* HPDF_Outline <- hpdf.CreateOutline(pdf, parent, title, encoder) */

{
  HPDF_Doc pdf = LclHandleGet(L, CnHndDoc, 1);
  HPDF_Outline parent = LclOptHandleGet(L, CnHndOutline, 2);
  const char * title = luaL_checkstring(L, 3);
  HPDF_Encoder encoder = LclOptHandleGet(L, CnHndEncoder, 4);
  HPDF_Outline result = HPDF_CreateOutline(pdf, parent, title, encoder);
  LclHandlePush(L, result, CnHndOutline);
  return 1;
}

/* * */

static int LclOutline_SetOpened(lua_State *L)

  /* HPDF_STATUS <- hpdf.Outline_SetOpened(outline, opened) */

{
  HPDF_Outline outline = LclHandleGet(L, CnHndOutline, 1);
  HPDF_BOOL opened = lua_toboolean(L, 2);
  HPDF_STATUS result = HPDF_Outline_SetOpened(outline, opened);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclOutline_SetDestination(lua_State *L)

  /* HPDF_STATUS <- hpdf.Outline_SetDestination(outline, dst) */

{
  HPDF_Outline outline = LclHandleGet(L, CnHndOutline, 1);
  HPDF_Destination dst = LclHandleGet(L, CnHndDestination, 2);
  HPDF_STATUS result = HPDF_Outline_SetDestination(outline, dst);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclPage_CreateDestination(lua_State *L)

  /* HPDF_Destination <- hpdf.Page_CreateDestination(page) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_Destination result = HPDF_Page_CreateDestination(page);
  LclHandlePush(L, result, CnHndDestination);
  return 1;
}

/* * */

static int LclDestination_SetXYZ(lua_State *L)

  /* HPDF_STATUS <- hpdf.Destination_SetXYZ(dst, left, top, zoom) */

{
  HPDF_Destination dst = LclHandleGet(L, CnHndDestination, 1);
  HPDF_REAL left = LclCheckReal(L, 2);
  HPDF_REAL top = LclCheckReal(L, 3);
  HPDF_REAL zoom = LclCheckReal(L, 4);
  HPDF_STATUS result = HPDF_Destination_SetXYZ(dst, left, top, zoom);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclDestination_SetFit(lua_State *L)

  /* HPDF_STATUS <- hpdf.Destination_SetFit(dst) */

{
  HPDF_Destination dst = LclHandleGet(L, CnHndDestination, 1);
  HPDF_STATUS result = HPDF_Destination_SetFit(dst);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclDestination_SetFitH(lua_State *L)

  /* HPDF_STATUS <- hpdf.Destination_SetFitH(dst, top) */

{
  HPDF_Destination dst = LclHandleGet(L, CnHndDestination, 1);
  HPDF_REAL top = LclCheckReal(L, 2);
  HPDF_STATUS result = HPDF_Destination_SetFitH(dst, top);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclDestination_SetFitV(lua_State *L)

  /* HPDF_STATUS <- hpdf.Destination_SetFitV(dst, left) */

{
  HPDF_Destination dst = LclHandleGet(L, CnHndDestination, 1);
  HPDF_REAL left = LclCheckReal(L, 2);
  HPDF_STATUS result = HPDF_Destination_SetFitV(dst, left);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclDestination_SetFitR(lua_State *L)

  /* HPDF_STATUS <- hpdf.Destination_SetFitR(dst, left, bottom, right, top) */

{
  HPDF_Destination dst = LclHandleGet(L, CnHndDestination, 1);
  HPDF_REAL left = LclCheckReal(L, 2);
  HPDF_REAL bottom = LclCheckReal(L, 3);
  HPDF_REAL right = LclCheckReal(L, 4);
  HPDF_REAL top = LclCheckReal(L, 5);
  HPDF_STATUS result = HPDF_Destination_SetFitR(dst, left, bottom, right, top);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclDestination_SetFitB(lua_State *L)

  /* HPDF_STATUS <- hpdf.Destination_SetFitB(dst) */

{
  HPDF_Destination dst = LclHandleGet(L, CnHndDestination, 1);
  HPDF_STATUS result = HPDF_Destination_SetFitB(dst);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclDestination_SetFitBH(lua_State *L)

  /* HPDF_STATUS <- hpdf.Destination_SetFitBH(dst, top) */

{
  HPDF_Destination dst = LclHandleGet(L, CnHndDestination, 1);
  HPDF_REAL top = LclCheckReal(L, 2);
  HPDF_STATUS result = HPDF_Destination_SetFitBH(dst, top);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclDestination_SetFitBV(lua_State *L)

  /* HPDF_STATUS <- hpdf.Destination_SetFitBV(dst, left) */

{
  HPDF_Destination dst = LclHandleGet(L, CnHndDestination, 1);
  HPDF_REAL left = LclCheckReal(L, 2);
  HPDF_STATUS result = HPDF_Destination_SetFitBV(dst, left);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclGetEncoder(lua_State *L)

  /* HPDF_Encoder <- hpdf.GetEncoder(pdf, encoding_name) */

{
  HPDF_Doc pdf = LclHandleGet(L, CnHndDoc, 1);
  const char * encoding_name = luaL_checkstring(L, 2);
  HPDF_Encoder result = HPDF_GetEncoder(pdf, encoding_name);
  LclHandlePush(L, result, CnHndEncoder);
  return 1;
}

/* * */

static int LclGetCurrentEncoder(lua_State *L)

  /* HPDF_Encoder <- hpdf.GetCurrentEncoder(pdf) */

{
  HPDF_Doc pdf = LclHandleGet(L, CnHndDoc, 1);
  HPDF_Encoder result = HPDF_GetCurrentEncoder(pdf);
  LclHandlePush(L, result, CnHndEncoder);
  return 1;
}

/* * */

static int LclSetCurrentEncoder(lua_State *L)

  /* HPDF_STATUS <- hpdf.SetCurrentEncoder(pdf, encoding_name) */

{
  HPDF_Doc pdf = LclHandleGet(L, CnHndDoc, 1);
  const char * encoding_name = luaL_checkstring(L, 2);
  HPDF_STATUS result = HPDF_SetCurrentEncoder(pdf, encoding_name);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclEncoder_GetType(lua_State *L)

  /* HPDF_EncoderType <- hpdf.Encoder_GetType(encoder) */

{
  HPDF_Encoder encoder = LclHandleGet(L, CnHndEncoder, 1);
  HPDF_EncoderType result = HPDF_Encoder_GetType(encoder);
  lua_pushstring(L, CnEncoderType[result]);
  return 1;
}

/* * */

static int LclEncoder_GetByteType(lua_State *L)

  /* HPDF_ByteType <- hpdf.Encoder_GetByteType(encoder, text, index) */

{
  HPDF_Encoder encoder = LclHandleGet(L, CnHndEncoder, 1);
  const char * text = luaL_checkstring(L, 2);
  HPDF_UINT index = luaL_checkinteger(L, 3);
  HPDF_ByteType result = HPDF_Encoder_GetByteType(encoder, text, index);
  lua_pushstring(L, CnByteType[result]);
  return 1;
}

/* * */

static int LclEncoder_GetUnicode(lua_State *L)

  /* HPDF_UNICODE <- hpdf.Encoder_GetUnicode(encoder, code) */

{
  HPDF_Encoder encoder = LclHandleGet(L, CnHndEncoder, 1);
  HPDF_UINT16 code = LclCheckU16(L, 2);
  HPDF_UNICODE result = HPDF_Encoder_GetUnicode(encoder, code);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclEncoder_GetWritingMode(lua_State *L)

  /* HPDF_WritingMode <- hpdf.Encoder_GetWritingMode(encoder) */

{
  HPDF_Encoder encoder = LclHandleGet(L, CnHndEncoder, 1);
  HPDF_WritingMode result = HPDF_Encoder_GetWritingMode(encoder);
  lua_pushstring(L, CnWritingMode[result]);
  return 1;
}

/* * */

static int LclUseJPEncodings(lua_State *L)

  /* HPDF_STATUS <- hpdf.UseJPEncodings(pdf) */

{
  HPDF_Doc pdf = LclHandleGet(L, CnHndDoc, 1);
  HPDF_STATUS result = HPDF_UseJPEncodings(pdf);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclUseKREncodings(lua_State *L)

  /* HPDF_STATUS <- hpdf.UseKREncodings(pdf) */

{
  HPDF_Doc pdf = LclHandleGet(L, CnHndDoc, 1);
  HPDF_STATUS result = HPDF_UseKREncodings(pdf);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclUseCNSEncodings(lua_State *L)

  /* HPDF_STATUS <- hpdf.UseCNSEncodings(pdf) */

{
  HPDF_Doc pdf = LclHandleGet(L, CnHndDoc, 1);
  HPDF_STATUS result = HPDF_UseCNSEncodings(pdf);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclUseCNTEncodings(lua_State *L)

  /* HPDF_STATUS <- hpdf.UseCNTEncodings(pdf) */

{
  HPDF_Doc pdf = LclHandleGet(L, CnHndDoc, 1);
  HPDF_STATUS result = HPDF_UseCNTEncodings(pdf);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclUseUTFEncodings(lua_State *L)

  /* HPDF_STATUS <- hpdf.UseUTFEncodings(pdf) */
 
{
  HPDF_Doc pdf = LclHandleGet(L, CnHndDoc, 1);
  HPDF_STATUS result = HPDF_UseUTFEncodings(pdf);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclPage_CreateTextAnnot(lua_State *L)

  /* HPDF_Annotation <- hpdf.Page_CreateTextAnnot(page, rect, text, encoder) */

{
  HPDF_Annotation result;
  HPDF_Rect rect;
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  const char * text = luaL_checkstring(L, 3);
  HPDF_Encoder encoder = LclHandleGet(L, CnHndEncoder, 4);
  LclRectGet(L, &rect, 2);
  result = HPDF_Page_CreateTextAnnot(page, rect, text, encoder);
  LclHandlePush(L, result, CnHndAnnotation);
  return 1;
}

/* * */

static int LclPage_CreateLinkAnnot(lua_State *L)

  /* HPDF_Annotation <- hpdf.Page_CreateLinkAnnot(page, rect, dst) */

{
  HPDF_Rect rect;
  HPDF_Annotation result;
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_Destination dst = LclHandleGet(L, CnHndDestination, 3);
  LclRectGet(L, &rect, 2);
  result = HPDF_Page_CreateLinkAnnot(page, rect, dst);
  LclHandlePush(L, result, CnHndAnnotation);
  return 1;
}

/* * */

static int LclPage_CreateURILinkAnnot(lua_State *L)

  /* HPDF_Annotation <- hpdf.Page_CreateURILinkAnnot(page, rect, uri) */

{
  HPDF_Rect rect;
  HPDF_Annotation result;
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  const char * uri = luaL_checkstring(L, 3);
  LclRectGet(L, &rect, 2);
  result = HPDF_Page_CreateURILinkAnnot(page, rect, uri);
  LclHandlePush(L, result, CnHndAnnotation);
  return 1;
}

/* * */

static int LclLinkAnnot_SetHighlightMode(lua_State *L)

  /* HPDF_STATUS <- hpdf.LinkAnnot_SetHighlightMode(annot, mode) */

{
  HPDF_Annotation annot = LclHandleGet(L, CnHndAnnotation, 1);
  HPDF_AnnotHighlightMode mode = luaL_checkoption(L, 2,
    "HPDF_ANNOT_NO_HIGHTLIGHT", CnAnnotHighlightMode);
  HPDF_STATUS result = HPDF_LinkAnnot_SetHighlightMode(annot, mode);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclLinkAnnot_SetBorderStyle(lua_State *L)

  /* HPDF_STATUS <- hpdf.LinkAnnot_SetBorderStyle(annot, width,
    dash_on, dash_off) */

{
  HPDF_Annotation annot = LclHandleGet(L, CnHndAnnotation, 1);
  HPDF_REAL width = LclCheckReal(L, 2);
  HPDF_UINT16 dash_on = LclCheckU16(L, 3);
  HPDF_UINT16 dash_off = LclCheckU16(L, 4);
  HPDF_STATUS result = HPDF_LinkAnnot_SetBorderStyle(annot, width,
    dash_on, dash_off);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclTextAnnot_SetIcon(lua_State *L)

  /* HPDF_STATUS <- hpdf.TextAnnot_SetIcon(annot, icon) */

{
  HPDF_Annotation annot = LclHandleGet(L, CnHndAnnotation, 1);
  HPDF_AnnotIcon icon = luaL_checkoption(L, 2, (char *) 0, CnAnnotIcon);
  HPDF_STATUS result = HPDF_TextAnnot_SetIcon(annot, icon);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclTextAnnot_SetOpened(lua_State *L)

  /* HPDF_STATUS <- hpdf.TextAnnot_SetOpened(annot, opened) */

{
  HPDF_Annotation annot = LclHandleGet(L, CnHndAnnotation, 1);
  HPDF_BOOL opened = lua_toboolean(L, 2);
  HPDF_STATUS result = HPDF_TextAnnot_SetOpened(annot, opened);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclLoadPngImageFromFile(lua_State *L)

  /* HPDF_Image <- hpdf.LoadPngImageFromFile(pdf, filename) */

{
  HPDF_Doc pdf = LclHandleGet(L, CnHndDoc, 1);
  const char * filename = luaL_checkstring(L, 2);
  HPDF_Image result = HPDF_LoadPngImageFromFile(pdf, filename);
  LclHandlePush(L, result, CnHndImage);
  return 1;
}

/* * */

static int LclLoadPngImageFromFile2(lua_State *L)

  /* HPDF_Image <- hpdf.LoadPngImageFromFile2(pdf, filename) */

{
  HPDF_Doc pdf = LclHandleGet(L, CnHndDoc, 1);
  const char * filename = luaL_checkstring(L, 2);
  HPDF_Image result = HPDF_LoadPngImageFromFile2(pdf, filename);
  LclHandlePush(L, result, CnHndImage);
  return 1;
}

/* * */

static int LclLoadJpegImageFromFile(lua_State *L)

  /* HPDF_Image <- hpdf.LoadJpegImageFromFile(pdf, filename) */

{
  HPDF_Doc pdf = LclHandleGet(L, CnHndDoc, 1);
  const char * filename = luaL_checkstring(L, 2);
  HPDF_Image result = HPDF_LoadJpegImageFromFile(pdf, filename);
  LclHandlePush(L, result, CnHndImage);
  return 1;
}

/* * */

static int LclLoadRawImageFromFile(lua_State *L)

  /* HPDF_Image <- hpdf.LoadRawImageFromFile(pdf, filename,
     width, height, color_space) */

{
  HPDF_Doc pdf = LclHandleGet(L, CnHndDoc, 1);
  const char * filename = luaL_checkstring(L, 2);
  HPDF_UINT width = luaL_checkinteger(L, 3);
  HPDF_UINT height = luaL_checkinteger(L, 4);
  HPDF_ColorSpace color_space = luaL_checkoption(L, 5,
    (char *) 0, CnColorSpace);
  HPDF_Image result = HPDF_LoadRawImageFromFile(pdf, filename,
    width, height, color_space);
  LclHandlePush(L, result, CnHndImage);
  return 1;
}

/* * */

static int LclLoadRawImageFromMem(lua_State *L)

  /* HPDF_Image <- hpdf.LoadRawImageFromMem(pdf, buf, width,
     height, color_space, bits_per_component) */

{
  HPDF_Doc pdf = LclHandleGet(L, CnHndDoc, 1);
  const HPDF_BYTE * buf = (HPDF_BYTE *) luaL_checkstring(L, 2);
  HPDF_UINT width = luaL_checkinteger(L, 3);
  HPDF_UINT height = luaL_checkinteger(L, 4);
  HPDF_ColorSpace color_space = luaL_checkoption(L, 5, (char *) 0,
    CnColorSpace);
  HPDF_UINT bits_per_component = luaL_checkinteger(L, 6);
  HPDF_Image result = HPDF_LoadRawImageFromMem(pdf, buf, width,
    height, color_space, bits_per_component);
  LclHandlePush(L, result, CnHndImage);
  return 1;
}

/* * */

static int LclImage_GetSize(lua_State *L)

  /* HPDF_Point <- hpdf.Image_GetSize(image) */

{
  int RetCount;
  HPDF_Image image = LclHandleGet(L, CnHndImage, 1);
  HPDF_Point result = HPDF_Image_GetSize(image);
  if ((result.x == 0) && (result.y == 0)) {
    RetCount = 0;
  } else {
    LclPointPush(L, result);
    RetCount = 1;
  }
  return RetCount;
}

/* * */

static int LclImage_GetSize2(lua_State *L)

  /* HPDF_STATUS <- hpdf.Image_GetSize2(image, size)
     API change
     Wd, Ht <- hpdf.Image_GetSize2(image) */

{
  HPDF_Point size;
  int RetCount;
  HPDF_Image image = LclHandleGet(L, CnHndImage, 1);
  HPDF_STATUS result = HPDF_Image_GetSize2(image, &size);
  if (HPDF_OK == result) {
    lua_pushnumber(L, size.x);
    lua_pushnumber(L, size.y);
    RetCount = 2;
  } else {
   RetCount = 0;
  }
  return RetCount;
}

/* * */

static int LclImage_GetWidth(lua_State *L)

  /* integer <- hpdf.Image_GetWidth(image) */

{
  HPDF_Image image = LclHandleGet(L, CnHndImage, 1);
  HPDF_UINT result = HPDF_Image_GetWidth(image);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclImage_GetHeight(lua_State *L)

  /* integer <- hpdf.Image_GetHeight(image) */

{
  HPDF_Image image = LclHandleGet(L, CnHndImage, 1);
  HPDF_UINT result = HPDF_Image_GetHeight(image);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclImage_GetBitsPerComponent(lua_State *L)

  /* integer <- hpdf.Image_GetBitsPerComponent(image) */

{
  HPDF_Image image = LclHandleGet(L, CnHndImage, 1);
  HPDF_UINT result = HPDF_Image_GetBitsPerComponent(image);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclImage_GetColorSpace(lua_State *L)

  /* string <- hpdf.Image_GetColorSpace(image) */

{
  HPDF_Image image = LclHandleGet(L, CnHndImage, 1);
  const char * result = HPDF_Image_GetColorSpace(image);
  lua_pushstring(L, result);
  return 1;
}

/* * */

static int LclImage_SetColorMask(lua_State *L)

  /* HPDF_STATUS <- hpdf.Image_SetColorMask(image, rmin, rmax,
     gmin, gmax, bmin, bmax) */

{
  HPDF_Image image = LclHandleGet(L, CnHndImage, 1);
  HPDF_UINT rmin = luaL_checkinteger(L, 2);
  HPDF_UINT rmax = luaL_checkinteger(L, 3);
  HPDF_UINT gmin = luaL_checkinteger(L, 4);
  HPDF_UINT gmax = luaL_checkinteger(L, 5);
  HPDF_UINT bmin = luaL_checkinteger(L, 6);
  HPDF_UINT bmax = luaL_checkinteger(L, 7);
  HPDF_STATUS result = HPDF_Image_SetColorMask(image, rmin, rmax,
    gmin, gmax, bmin, bmax);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclImage_SetMaskImage(lua_State *L)

  /* HPDF_STATUS <- hpdf.Image_SetMaskImage(image, mask_image) */

{
  HPDF_Image image = LclHandleGet(L, CnHndImage, 1);
  HPDF_Image mask_image = LclHandleGet(L, CnHndImage, 2);
  HPDF_STATUS result = HPDF_Image_SetMaskImage(image, mask_image);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclSetInfoAttr(lua_State *L)

  /* HPDF_STATUS <- hpdf.SetInfoAttr(pdf, type, value) */

{
  HPDF_Doc pdf = LclHandleGet(L, CnHndDoc, 1);
  HPDF_InfoType type = luaL_checkoption(L, 2, (char *) 0, CnInfoType);
  const char * value = luaL_checkstring(L, 3);
  HPDF_STATUS result = HPDF_SetInfoAttr(pdf, type, value);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclGetInfoAttr(lua_State *L)

  /* string <- hpdf.GetInfoAttr(pdf, type) */

{
  HPDF_Doc pdf = LclHandleGet(L, CnHndDoc, 1);
  HPDF_InfoType type = luaL_checkoption(L, 2, (char *) 0, CnInfoType);
  const char * result = HPDF_GetInfoAttr(pdf, type);
  lua_pushstring(L, result);
  return 1;
}

/* * */

static int LclSetInfoDateAttr(lua_State *L)

  /* HPDF_STATUS <- hpdf.SetInfoDateAttr(pdf, type, value) */

{
  HPDF_Date value;
  HPDF_STATUS result;
  HPDF_Doc pdf = LclHandleGet(L, CnHndDoc, 1);
  HPDF_InfoType type = luaL_checkoption(L, 2, (char *) 0, CnInfoType);
  LclDateGet(L, &value, 3);
  result = HPDF_SetInfoDateAttr(pdf, type, value);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclSetPassword(lua_State *L)

  /* HPDF_STATUS <- hpdf.SetPassword(pdf, owner_passwd, user_passwd) */

{
  HPDF_Doc pdf = LclHandleGet(L, CnHndDoc, 1);
  const char * owner_passwd = luaL_checkstring(L, 2);
  const char * user_passwd = luaL_checkstring(L, 3);
  HPDF_STATUS result = HPDF_SetPassword(pdf, owner_passwd, user_passwd);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclSetPermission(lua_State *L)

  /* HPDF_STATUS <- hpdf.SetPermission(pdf, permission) */

{
  HPDF_Doc pdf = LclHandleGet(L, CnHndDoc, 1);
  HPDF_UINT permission = luaL_checkinteger(L, 2);
  HPDF_STATUS result = HPDF_SetPermission(pdf, permission);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclSetEncryptionMode(lua_State *L)

  /* HPDF_STATUS <- hpdf.SetEncryptionMode(pdf, mode, key_len) */

{
  HPDF_Doc pdf = LclHandleGet(L, CnHndDoc, 1);
  HPDF_EncryptMode mode = luaL_checkoption(L, 2, (char *) 0, CnEncryptMode);
  HPDF_UINT key_len = luaL_checkinteger(L, 3);
  HPDF_STATUS result = HPDF_SetEncryptionMode(pdf, mode, key_len);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclSetCompressionMode(lua_State *L)

  /* HPDF_STATUS <- hpdf.SetCompressionMode(pdf, mode) */

{
  HPDF_Doc pdf = LclHandleGet(L, CnHndDoc, 1);
  HPDF_UINT mode = luaL_checkinteger(L, 2);
  HPDF_STATUS result = HPDF_SetCompressionMode(pdf, mode);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclFont_GetFontName(lua_State *L)

  /* string <- hpdf.Font_GetFontName(font) */

{
  HPDF_Font font = LclHandleGet(L, CnHndFont, 1);
  const char * result = HPDF_Font_GetFontName(font);
  lua_pushstring(L, result);
  return 1;
}

/* * */

static int LclFont_GetEncodingName(lua_State *L)

  /* string <- hpdf.Font_GetEncodingName(font) */

{
  HPDF_Font font = LclHandleGet(L, CnHndFont, 1);
  const char * result = HPDF_Font_GetEncodingName(font);
  lua_pushstring(L, result);
  return 1;
}

/* * */

static int LclFont_GetUnicodeWidth(lua_State *L)

  /* integer <- hpdf.Font_GetUnicodeWidth(font, code) */

{
  HPDF_Font font = LclHandleGet(L, CnHndFont, 1);
  HPDF_UNICODE code = LclCheckU16(L, 2);
  HPDF_INT result = HPDF_Font_GetUnicodeWidth(font, code);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclFont_GetBBox(lua_State *L)

  /* HPDF_Box <- hpdf.Font_GetBBox(font) */

{
  HPDF_Font font = LclHandleGet(L, CnHndFont, 1);
  HPDF_Box result = HPDF_Font_GetBBox(font);
  /* Stk: ... */
  lua_newtable(L);
  /* Stk: ... Tbl */
  LclSetNumField(L, result.left, "left");
  LclSetNumField(L, result.bottom, "bottom");
  LclSetNumField(L, result.right, "right");
  LclSetNumField(L, result.top, "top");
  /* Stk: ... Tbl */
  return 1;
}

/* * */

static int LclFont_GetAscent(lua_State *L)

  /* integer <- hpdf.Font_GetAscent(font) */

{
  HPDF_Font font = LclHandleGet(L, CnHndFont, 1);
  HPDF_INT result = HPDF_Font_GetAscent(font);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclFont_GetDescent(lua_State *L)

  /* integer <- hpdf.Font_GetDescent(font) */

{
  HPDF_Font font = LclHandleGet(L, CnHndFont, 1);
  HPDF_INT result = HPDF_Font_GetDescent(font);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclFont_GetXHeight(lua_State *L)

  /* integer <- hpdf.Font_GetXHeight(font) */

{
  HPDF_Font font = LclHandleGet(L, CnHndFont, 1);
  HPDF_UINT result = HPDF_Font_GetXHeight(font);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclFont_GetCapHeight(lua_State *L)

  /* integer <- hpdf.Font_GetCapHeight(font) */

{
  HPDF_Font font = LclHandleGet(L, CnHndFont, 1);
  HPDF_UINT result = HPDF_Font_GetCapHeight(font);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclFont_TextWidth(lua_State *L)

  /* HPDF_TextWidth <- hpdf.Font_TextWidth(font, text, len) */

{
  HPDF_Font font = LclHandleGet(L, CnHndFont, 1);
  const HPDF_BYTE * text = (HPDF_BYTE *) luaL_checkstring(L, 2);
  HPDF_UINT len = luaL_checkinteger(L, 3);
  HPDF_TextWidth result = HPDF_Font_TextWidth(font, text, len);
  /* Stk: ... */
  lua_newtable(L);
  /* Stk: ... Tbl */

  LclSetIntField(L, result.numchars, "numchars");
  LclSetIntField(L, result.numwords, "numwords");
  LclSetIntField(L, result.width, "width");
  LclSetIntField(L, result.numspace, "numspace");
  /* Stk: ... Tbl */
  return 1;
}

/* * */

static int LclFont_MeasureText(lua_State *L)

  /* integer <- hpdf.Font_MeasureText(font, text, width,
     font_size, char_space, word_space, wordwrap, real_width) */

{
  HPDF_UINT result;
  HPDF_REAL real_width, * real_width_ptr;
  HPDF_Font font = LclHandleGet(L, CnHndFont, 1);
  size_t len_;
  const HPDF_BYTE * text = (HPDF_BYTE *) luaL_checklstring(L, 2, &len_);
  HPDF_UINT len = (HPDF_UINT)len_;
  HPDF_REAL width = LclCheckReal(L, 3);
  HPDF_REAL font_size = LclCheckReal(L, 4);
  HPDF_REAL char_space = LclCheckReal(L, 5);
  HPDF_REAL word_space = LclCheckReal(L, 6);
  HPDF_BOOL wordwrap = lua_toboolean(L, 7);
  if (lua_isnumber(L, 8)) {
    real_width = (HPDF_REAL) lua_tonumber(L, 8);
    real_width_ptr = &real_width;
  } else {
    real_width_ptr = (HPDF_REAL *) 0;
  }
  result = HPDF_Font_MeasureText(font, text, len, width,
    font_size, char_space, word_space, wordwrap, real_width_ptr);
  lua_pushinteger(L, result);
  if (real_width_ptr) {
    lua_pushnumber(L, real_width);
    return 2;
  } else
    return 1;
}

/* * */

static int LclCreateExtGState(lua_State *L)

  /* HPDF_ExtGState <- hpdf.CreateExtGState(pdf) */

{
  HPDF_Doc pdf = LclHandleGet(L, CnHndDoc, 1);
  HPDF_ExtGState result = HPDF_CreateExtGState(pdf);
  LclHandlePush(L, result, CnHndExtGState);
  return 1;
}

/* * */

static int LclExtGState_SetAlphaStroke(lua_State *L)

  /* HPDF_STATUS <- hpdf.ExtGState_SetAlphaStroke(ext_gstate, value) */

{
  HPDF_ExtGState ext_gstate = LclHandleGet(L, CnHndExtGState, 1);
  HPDF_REAL value = LclCheckReal(L, 2);
  HPDF_STATUS result = HPDF_ExtGState_SetAlphaStroke(ext_gstate, value);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclExtGState_SetAlphaFill(lua_State *L)

  /* HPDF_STATUS <- hpdf.ExtGState_SetAlphaFill(ext_gstate, value) */

{
  HPDF_ExtGState ext_gstate = LclHandleGet(L, CnHndExtGState, 1);
  HPDF_REAL value = LclCheckReal(L, 2);
  HPDF_STATUS result = HPDF_ExtGState_SetAlphaFill(ext_gstate, value);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclExtGState_SetBlendMode(lua_State *L)

  /* HPDF_STATUS <- hpdf.ExtGState_SetBlendMode(ext_gstate, mode) */

{
  HPDF_ExtGState ext_gstate = LclHandleGet(L, CnHndExtGState, 1);
  HPDF_BlendMode mode = luaL_checkoption(L, 2, (char *) 0, CnBlendMode);
  HPDF_STATUS result = HPDF_ExtGState_SetBlendMode(ext_gstate, mode);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclPage_TextWidth(lua_State *L)

  /* number <- hpdf.Page_TextWidth(page, text) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  const char * text = luaL_checkstring(L, 2);
  HPDF_REAL result = HPDF_Page_TextWidth(page, text);
  lua_pushnumber(L, result);
  return 1;
}

/* * */

static int LclPage_MeasureText(lua_State *L)

  /* integer <- hpdf.Page_MeasureText(page, text, width, wordwrap, real_width) */

{
  HPDF_UINT result;
  HPDF_REAL real_width, * real_width_ptr;
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  const char * text = luaL_checkstring(L, 2);
  HPDF_REAL width = LclCheckReal(L, 3);
  HPDF_BOOL wordwrap = lua_toboolean(L, 4);
  if (lua_isnumber(L, 5)) {
    real_width = (HPDF_REAL) lua_tonumber(L, 5);
    real_width_ptr = &real_width;
  } else {
    real_width_ptr = (HPDF_REAL *) 0;
  }
  result = HPDF_Page_MeasureText(page, text, width, wordwrap, real_width_ptr);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclPage_GetWidth(lua_State *L)

  /* number <- hpdf.Page_GetWidth(page) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_REAL result = HPDF_Page_GetWidth(page);
  lua_pushnumber(L, result);
  return 1;
}

/* * */

static int LclPage_GetHeight(lua_State *L)

  /* number <- hpdf.Page_GetHeight(page) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_REAL result = HPDF_Page_GetHeight(page);
  lua_pushnumber(L, result);
  return 1;
}

/* * */

static int LclPage_GetGMode(lua_State *L)

  /* integer <- hpdf.Page_GetGMode(page) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_UINT16 result = HPDF_Page_GetGMode(page);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclPage_GetCurrentPos(lua_State *L)

  /* HPDF_Point <- hpdf.Page_GetCurrentPos(page) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_Point result = HPDF_Page_GetCurrentPos(page);
  LclPointPush(L, result);
  return 1;
}

/* * */

static int LclPage_GetCurrentPos2(lua_State *L)

  /* HPDF_STATUS <- hpdf.Page_GetCurrentPos2(page, pos)
     API change
     X, Y <- hpdf.Page_GetCurrentPos2(page) */

{
  int RetCount;
  HPDF_Point pos;

  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_STATUS result = HPDF_Page_GetCurrentPos2(page, &pos);
  if (HPDF_OK == result) {
    /* Stk: ... */
    lua_pushnumber(L, pos.x);
    /* Stk: ... X */
    lua_pushnumber(L, pos.y);
    /* Stk: ... X Y */
    RetCount = 2;
  } else {
    RetCount = 0;
  }
  return RetCount;
}

/* * */

static int LclPage_GetCurrentTextPos(lua_State *L)

  /* HPDF_Point <- hpdf.Page_GetCurrentTextPos(page) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_Point result = HPDF_Page_GetCurrentTextPos(page);
  LclPointPush(L, result);
  return 1;
}

/* * */

static int LclPage_GetCurrentTextPos2(lua_State *L)

  /* HPDF_STATUS <- hpdf.Page_GetCurrentTextPos2(page, pos)
     API change
     X, Y <- hpdf.Page_GetCurrentTextPos2(page) */

{
  HPDF_Point pos;
  int RetCount;
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_STATUS result = HPDF_Page_GetCurrentTextPos2(page, &pos);
  if (HPDF_OK == result) {
    /* Stk: ... */
    lua_pushnumber(L, pos.x);
    /* Stk: ... X */
    lua_pushnumber(L, pos.y);
    /* Stk: ... Y */
    RetCount = 2;
  } else {
    RetCount = 0;
  }
  return RetCount;
}

/* * */

static int LclPage_GetCurrentFont(lua_State *L)

  /* HPDF_Font <- hpdf.Page_GetCurrentFont(page) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_Font result = HPDF_Page_GetCurrentFont(page);
  LclHandlePush(L, result, CnHndFont);
  return 1;
}

/* * */

static int LclPage_GetCurrentFontSize(lua_State *L)

  /* number <- hpdf.Page_GetCurrentFontSize(page) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_REAL result = HPDF_Page_GetCurrentFontSize(page);
  lua_pushnumber(L, result);
  return 1;
}

/* * */

static int LclPage_GetTransMatrix(lua_State *L)

  /* HPDF_TransMatrix <- hpdf.Page_GetTransMatrix(page) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_TransMatrix result = HPDF_Page_GetTransMatrix(page);
  LclTransMatrixPush(L, result);
  return 1;
}

/* * */

static int LclPage_GetLineWidth(lua_State *L)

  /* number <- hpdf.Page_GetLineWidth(page) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_REAL result = HPDF_Page_GetLineWidth(page);
  lua_pushnumber(L, result);
  return 1;
}

/* * */

static int LclPage_GetLineCap(lua_State *L)

  /* HPDF_LineCap <- hpdf.Page_GetLineCap(page) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_LineCap result = HPDF_Page_GetLineCap(page);
  lua_pushstring(L, CnLineCap[result]);
  return 1;
}

/* * */

static int LclPage_GetLineJoin(lua_State *L)

  /* HPDF_LineJoin <- hpdf.Page_GetLineJoin(page) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_LineJoin result = HPDF_Page_GetLineJoin(page);
  lua_pushstring(L, CnLineJoin[result]);
  return 1;
}

/* * */

static int LclPage_GetMiterLimit(lua_State *L)

  /* number <- hpdf.Page_GetMiterLimit(page) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_REAL result = HPDF_Page_GetMiterLimit(page);
  lua_pushnumber(L, result);
  return 1;
}

/* * */

static int LclPage_GetDash(lua_State *L)

  /* HPDF_DashMode <- hpdf.Page_GetDash(page) */

{
  int J;
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_DashMode result = HPDF_Page_GetDash(page);
  /* Stk: ... */
  lua_newtable(L);
  /* Stk: ... Tbl */
  lua_pushstring(L, "ptn");
  /* Stk: ... Tbl "ptn" */
  lua_newtable(L);
  /* Stk: ... Tbl "ptn" Tbl */
  for (J = 0; J < 8; J++) {
    lua_pushnumber(L, result.ptn[J]);
    /* Stk: ... Tbl "ptn" Tbl Num */
    lua_rawseti(L, -2, J + 1);
    /* Stk: ... Tbl "ptn" Tbl */
  }
  /* Stk: ... Tbl "ptn" Tbl */
  lua_rawset(L, -3);
  /* Stk: ... Tbl */
  LclSetIntField(L, result.num_ptn, "num_ptn");
  LclSetIntField(L, result.phase, "phase");
  /* Stk: ... Tbl */
  return 1;
}

/* * */

static int LclPage_GetFlat(lua_State *L)

  /* number <- hpdf.Page_GetFlat(page) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_REAL result = HPDF_Page_GetFlat(page);
  lua_pushnumber(L, result);
  return 1;
}

/* * */

static int LclPage_GetCharSpace(lua_State *L)

  /* number <- hpdf.Page_GetCharSpace(page) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_REAL result = HPDF_Page_GetCharSpace(page);
  lua_pushnumber(L, result);
  return 1;
}

/* * */

static int LclPage_GetWordSpace(lua_State *L)

  /* number <- hpdf.Page_GetWordSpace(page) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_REAL result = HPDF_Page_GetWordSpace(page);
  lua_pushnumber(L, result);
  return 1;
}

/* * */

static int LclPage_GetHorizontalScalling(lua_State *L)

  /* number <- hpdf.Page_GetHorizontalScalling(page) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_REAL result = HPDF_Page_GetHorizontalScalling(page);
  lua_pushnumber(L, result);
  return 1;
}

/* * */

static int LclPage_GetTextLeading(lua_State *L)

  /* number <- hpdf.Page_GetTextLeading(page) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_REAL result = HPDF_Page_GetTextLeading(page);
  lua_pushnumber(L, result);
  return 1;
}

/* * */

static int LclPage_GetTextRenderingMode(lua_State *L)

  /* HPDF_TextRenderingMode <- hpdf.Page_GetTextRenderingMode(page) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_TextRenderingMode result = HPDF_Page_GetTextRenderingMode(page);
  lua_pushstring(L, CnTextRenderingMode[result]);
  return 1;
}

/* * */

static int LclPage_GetTextRaise(lua_State *L)

  /* number <- hpdf.Page_GetTextRaise(page) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_REAL result = HPDF_Page_GetTextRaise(page);
  lua_pushnumber(L, result);
  return 1;
}

/* * */

static int LclPage_GetTextRise(lua_State *L)

  /* number <- hpdf.Page_GetTextRise(page) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_REAL result = HPDF_Page_GetTextRise(page);
  lua_pushnumber(L, result);
  return 1;
}

/* * */

static int LclPage_GetRGBFill(lua_State *L)

  /* HPDF_RGBColor <- hpdf.Page_GetRGBFill(page) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_RGBColor result = HPDF_Page_GetRGBFill(page);
  LclRgbPush(L, result);
  return 1;
}

/* * */

static int LclPage_GetRGBStroke(lua_State *L)

  /* HPDF_RGBColor <- hpdf.Page_GetRGBStroke(page) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_RGBColor result = HPDF_Page_GetRGBStroke(page);
  LclRgbPush(L, result);
  return 1;
}

/* * */

static int LclPage_GetCMYKFill(lua_State *L)

  /* HPDF_CMYKColor <- hpdf.Page_GetCMYKFill(page) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_CMYKColor result = HPDF_Page_GetCMYKFill(page);
  LclCmykPush(L, result);
  return 1;
}

/* * */

static int LclPage_GetCMYKStroke(lua_State *L)

  /* HPDF_CMYKColor <- hpdf.Page_GetCMYKStroke(page) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_CMYKColor result = HPDF_Page_GetCMYKStroke(page);
  LclCmykPush(L, result);
  return 1;
}

/* * */

static int LclPage_GetGrayFill(lua_State *L)

  /* number <- hpdf.Page_GetGrayFill(page) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_REAL result = HPDF_Page_GetGrayFill(page);
  lua_pushnumber(L, result);
  return 1;
}

/* * */

static int LclPage_GetGrayStroke(lua_State *L)

  /* number <- hpdf.Page_GetGrayStroke(page) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_REAL result = HPDF_Page_GetGrayStroke(page);
  lua_pushnumber(L, result);
  return 1;
}

/* * */

static int LclPage_GetStrokingColorSpace(lua_State *L)

  /* HPDF_ColorSpace <- hpdf.Page_GetStrokingColorSpace(page) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_ColorSpace result = HPDF_Page_GetStrokingColorSpace(page);
  lua_pushstring(L, CnColorSpace[result]);
  return 1;
}

/* * */

static int LclPage_GetFillingColorSpace(lua_State *L)

  /* HPDF_ColorSpace <- hpdf.Page_GetFillingColorSpace(page) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_ColorSpace result = HPDF_Page_GetFillingColorSpace(page);
  lua_pushstring(L, CnColorSpace[result]);
  return 1;
}

/* * */

static int LclPage_GetTextMatrix(lua_State *L)

  /* HPDF_TransMatrix <- hpdf.Page_GetTextMatrix(page) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_TransMatrix result = HPDF_Page_GetTextMatrix(page);
  LclTransMatrixPush(L, result);
  return 1;
}

/* * */

static int LclPage_GetGStateDepth(lua_State *L)

  /* integer <- hpdf.Page_GetGStateDepth(page) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_UINT result = HPDF_Page_GetGStateDepth(page);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclPage_SetLineWidth(lua_State *L)

  /* HPDF_STATUS <- hpdf.Page_SetLineWidth(page, line_width) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_REAL line_width = LclCheckReal(L, 2);
  HPDF_STATUS result = HPDF_Page_SetLineWidth(page, line_width);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclPage_SetLineCap(lua_State *L)

  /* HPDF_STATUS <- hpdf.Page_SetLineCap(page, line_cap) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_LineCap line_cap = luaL_checkoption(L, 2, (char *) 0, CnLineCap);
  HPDF_STATUS result = HPDF_Page_SetLineCap(page, line_cap);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclPage_SetLineJoin(lua_State *L)

  /* HPDF_STATUS <- hpdf.Page_SetLineJoin(page, line_join) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_LineJoin line_join = luaL_checkoption(L, 2, (char *) 0, CnLineJoin);
  HPDF_STATUS result = HPDF_Page_SetLineJoin(page, line_join);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclPage_SetMiterLimit(lua_State *L)

  /* HPDF_STATUS <- hpdf.Page_SetMiterLimit(page, miter_limit) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_REAL miter_limit = LclCheckReal(L, 2);
  HPDF_STATUS result = HPDF_Page_SetMiterLimit(page, miter_limit);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclPage_SetDash(lua_State *L)

  /* HPDF_STATUS <- hpdf.Page_SetDash(page, dash_ptn, num_param, phase) */

{
  int Tp;
  HPDF_DashMode mode;
  HPDF_STATUS result;
  HPDF_Page page;

  page = LclHandleGet(L, CnHndPage, 1);
  Tp = lua_type(L, 2);

  mode.num_ptn = 0;
  mode.phase = 0;

  if (!(LUA_TNIL == Tp) && !(LUA_TNONE == Tp)) {
    luaL_argcheck(L, lua_istable(L, 2), 2, "expecting dash mode table");
    mode.num_ptn = lua_rawlen(L, 2);
    if (mode.num_ptn <= 8) {
      mode.phase = luaL_checkinteger(L, 3);
      int J;
      for (J = 0; J < mode.num_ptn; J++) {
        int Dash;
        /* Stk: ... */
        lua_rawgeti(L, 2, J + 1);
        /* Stk: ... ptn[J] */
        Dash = lua_tointeger(L, -1);
        mode.ptn[J] = Dash ? Dash : 1;
        lua_pop(L, 1);
        /* Stk: ... */
      }
    }
    else {
      lua_pushstring(L, "dash table must have 0 to 8 elements");
      lua_error(L);
    }
  }
  result = HPDF_Page_SetDash(page, mode.ptn, mode.num_ptn, mode.phase);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclPage_SetFlat(lua_State *L)

  /* HPDF_STATUS <- hpdf.Page_SetFlat(page, flatness) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_REAL flatness = LclCheckReal(L, 2);
  HPDF_STATUS result = HPDF_Page_SetFlat(page, flatness);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclPage_SetExtGState(lua_State *L)

  /* HPDF_STATUS <- hpdf.Page_SetExtGState(page, ext_gstate) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_ExtGState ext_gstate = LclHandleGet(L, CnHndExtGState, 2);
  HPDF_STATUS result = HPDF_Page_SetExtGState(page, ext_gstate);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclPage_GSave(lua_State *L)

  /* HPDF_STATUS <- hpdf.Page_GSave(page) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_STATUS result = HPDF_Page_GSave(page);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclPage_GRestore(lua_State *L)

  /* HPDF_STATUS <- hpdf.Page_GRestore(page) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_STATUS result = HPDF_Page_GRestore(page);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclPage_Concat(lua_State *L)

  /* HPDF_STATUS <- hpdf.Page_Concat(page, a, b, c, d, x, y) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_REAL a = LclCheckReal(L, 2);
  HPDF_REAL b = LclCheckReal(L, 3);
  HPDF_REAL c = LclCheckReal(L, 4);
  HPDF_REAL d = LclCheckReal(L, 5);
  HPDF_REAL x = LclCheckReal(L, 6);
  HPDF_REAL y = LclCheckReal(L, 7);
  HPDF_STATUS result = HPDF_Page_Concat(page, a, b, c, d, x, y);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclPage_MoveTo(lua_State *L)

  /* HPDF_STATUS <- hpdf.Page_MoveTo(page, x, y) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_REAL x = LclCheckReal(L, 2);
  HPDF_REAL y = LclCheckReal(L, 3);
  HPDF_STATUS result = HPDF_Page_MoveTo(page, x, y);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclPage_LineTo(lua_State *L)

  /* HPDF_STATUS <- hpdf.Page_LineTo(page, x, y) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_REAL x = LclCheckReal(L, 2);
  HPDF_REAL y = LclCheckReal(L, 3);
  HPDF_STATUS result = HPDF_Page_LineTo(page, x, y);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclPage_CurveTo(lua_State *L)

  /* HPDF_STATUS <- hpdf.Page_CurveTo(page, x1, y1, x2, y2, x3, y3) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_REAL x1 = LclCheckReal(L, 2);
  HPDF_REAL y1 = LclCheckReal(L, 3);
  HPDF_REAL x2 = LclCheckReal(L, 4);
  HPDF_REAL y2 = LclCheckReal(L, 5);
  HPDF_REAL x3 = LclCheckReal(L, 6);
  HPDF_REAL y3 = LclCheckReal(L, 7);
  HPDF_STATUS result = HPDF_Page_CurveTo(page, x1, y1, x2, y2, x3, y3);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclPage_CurveTo2(lua_State *L)

  /* HPDF_STATUS <- hpdf.Page_CurveTo2(page, x2, y2, x3, y3) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_REAL x2 = LclCheckReal(L, 2);
  HPDF_REAL y2 = LclCheckReal(L, 3);
  HPDF_REAL x3 = LclCheckReal(L, 4);
  HPDF_REAL y3 = LclCheckReal(L, 5);
  HPDF_STATUS result = HPDF_Page_CurveTo2(page, x2, y2, x3, y3);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclPage_CurveTo3(lua_State *L)

  /* HPDF_STATUS <- hpdf.Page_CurveTo3(page, x1, y1, x3, y3) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_REAL x1 = LclCheckReal(L, 2);
  HPDF_REAL y1 = LclCheckReal(L, 3);
  HPDF_REAL x3 = LclCheckReal(L, 4);
  HPDF_REAL y3 = LclCheckReal(L, 5);
  HPDF_STATUS result = HPDF_Page_CurveTo3(page, x1, y1, x3, y3);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclPage_ClosePath(lua_State *L)

  /* HPDF_STATUS <- hpdf.Page_ClosePath(page) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_STATUS result = HPDF_Page_ClosePath(page);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclPage_Rectangle(lua_State *L)

  /* HPDF_STATUS <- hpdf.Page_Rectangle(page, x, y, width, height) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_REAL x = LclCheckReal(L, 2);
  HPDF_REAL y = LclCheckReal(L, 3);
  HPDF_REAL width = LclCheckReal(L, 4);
  HPDF_REAL height = LclCheckReal(L, 5);
  HPDF_STATUS result = HPDF_Page_Rectangle(page, x, y, width, height);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclPage_Stroke(lua_State *L)

  /* HPDF_STATUS <- hpdf.Page_Stroke(page) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_STATUS result = HPDF_Page_Stroke(page);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclPage_ClosePathStroke(lua_State *L)

  /* HPDF_STATUS <- hpdf.Page_ClosePathStroke(page) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_STATUS result = HPDF_Page_ClosePathStroke(page);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclPage_Fill(lua_State *L)

  /* HPDF_STATUS <- hpdf.Page_Fill(page) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_STATUS result = HPDF_Page_Fill(page);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclPage_Eofill(lua_State *L)

  /* HPDF_STATUS <- hpdf.Page_Eofill(page) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_STATUS result = HPDF_Page_Eofill(page);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclPage_FillStroke(lua_State *L)

  /* HPDF_STATUS <- hpdf.Page_FillStroke(page) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_STATUS result = HPDF_Page_FillStroke(page);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclPage_EofillStroke(lua_State *L)

  /* HPDF_STATUS <- hpdf.Page_EofillStroke(page) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_STATUS result = HPDF_Page_EofillStroke(page);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclPage_ClosePathFillStroke(lua_State *L)

  /* HPDF_STATUS <- hpdf.Page_ClosePathFillStroke(page) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_STATUS result = HPDF_Page_ClosePathFillStroke(page);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclPage_ClosePathEofillStroke(lua_State *L)

  /* HPDF_STATUS <- hpdf.Page_ClosePathEofillStroke(page) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_STATUS result = HPDF_Page_ClosePathEofillStroke(page);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclPage_EndPath(lua_State *L)

  /* HPDF_STATUS <- hpdf.Page_EndPath(page) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_STATUS result = HPDF_Page_EndPath(page);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclPage_Clip(lua_State *L)

  /* HPDF_STATUS <- hpdf.Page_Clip(page) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_STATUS result = HPDF_Page_Clip(page);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclPage_Eoclip(lua_State *L)

  /* HPDF_STATUS <- hpdf.Page_Eoclip(page) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_STATUS result = HPDF_Page_Eoclip(page);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclPage_BeginText(lua_State *L)

  /* HPDF_STATUS <- hpdf.Page_BeginText(page) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_STATUS result = HPDF_Page_BeginText(page);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclPage_EndText(lua_State *L)

  /* HPDF_STATUS <- hpdf.Page_EndText(page) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_STATUS result = HPDF_Page_EndText(page);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclPage_SetCharSpace(lua_State *L)

  /* HPDF_STATUS <- hpdf.Page_SetCharSpace(page, value) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_REAL value = LclCheckReal(L, 2);
  HPDF_STATUS result = HPDF_Page_SetCharSpace(page, value);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclPage_SetWordSpace(lua_State *L)

  /* HPDF_STATUS <- hpdf.Page_SetWordSpace(page, value) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_REAL value = LclCheckReal(L, 2);
  HPDF_STATUS result = HPDF_Page_SetWordSpace(page, value);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclPage_SetHorizontalScalling(lua_State *L)

  /* HPDF_STATUS <- hpdf.Page_SetHorizontalScalling(page, value) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_REAL value = LclCheckReal(L, 2);
  HPDF_STATUS result = HPDF_Page_SetHorizontalScalling(page, value);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclPage_SetTextLeading(lua_State *L)

  /* HPDF_STATUS <- hpdf.Page_SetTextLeading(page, value) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_REAL value = LclCheckReal(L, 2);
  HPDF_STATUS result = HPDF_Page_SetTextLeading(page, value);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclPage_SetFontAndSize(lua_State *L)

  /* HPDF_STATUS <- hpdf.Page_SetFontAndSize(page, font, size) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_Font font = LclHandleGet(L, CnHndFont, 2);
  HPDF_REAL size = LclCheckReal(L, 3);
  HPDF_STATUS result = HPDF_Page_SetFontAndSize(page, font, size);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclPage_SetTextRenderingMode(lua_State *L)

  /* HPDF_STATUS <- hpdf.Page_SetTextRenderingMode(page, mode) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_TextRenderingMode mode = luaL_checkoption(L, 2,
    (char *) 0, CnTextRenderingMode);
  HPDF_STATUS result = HPDF_Page_SetTextRenderingMode(page, mode);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclPage_SetTextRise(lua_State *L)

  /* HPDF_STATUS <- hpdf.Page_SetTextRise(page, value) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_REAL value = LclCheckReal(L, 2);
  HPDF_STATUS result = HPDF_Page_SetTextRise(page, value);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclPage_SetTextRaise(lua_State *L)

  /* HPDF_STATUS <- hpdf.Page_SetTextRaise(page, value) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_REAL value = LclCheckReal(L, 2);
  HPDF_STATUS result = HPDF_Page_SetTextRaise(page, value);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclPage_MoveTextPos(lua_State *L)

  /* HPDF_STATUS <- hpdf.Page_MoveTextPos(page, x, y) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_REAL x = LclCheckReal(L, 2);
  HPDF_REAL y = LclCheckReal(L, 3);
  HPDF_STATUS result = HPDF_Page_MoveTextPos(page, x, y);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclPage_MoveTextPos2(lua_State *L)

  /* HPDF_STATUS <- hpdf.Page_MoveTextPos2(page, x, y) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_REAL x = LclCheckReal(L, 2);
  HPDF_REAL y = LclCheckReal(L, 3);
  HPDF_STATUS result = HPDF_Page_MoveTextPos2(page, x, y);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclPage_SetTextMatrix(lua_State *L)

  /* HPDF_STATUS <- hpdf.Page_SetTextMatrix(page, a, b, c, d, x, y) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_REAL a = LclCheckReal(L, 2);
  HPDF_REAL b = LclCheckReal(L, 3);
  HPDF_REAL c = LclCheckReal(L, 4);
  HPDF_REAL d = LclCheckReal(L, 5);
  HPDF_REAL x = LclCheckReal(L, 6);
  HPDF_REAL y = LclCheckReal(L, 7);
  HPDF_STATUS result = HPDF_Page_SetTextMatrix(page, a, b, c, d, x, y);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclPage_MoveToNextLine(lua_State *L)

  /* HPDF_STATUS <- hpdf.Page_MoveToNextLine(page) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_STATUS result = HPDF_Page_MoveToNextLine(page);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclPage_ShowText(lua_State *L)

  /* HPDF_STATUS <- hpdf.Page_ShowText(page, text) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  const char * text = luaL_checkstring(L, 2);
  HPDF_STATUS result = HPDF_Page_ShowText(page, text);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclPage_ShowTextNextLine(lua_State *L)

  /* HPDF_STATUS <- hpdf.Page_ShowTextNextLine(page, text) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  const char * text = luaL_checkstring(L, 2);
  HPDF_STATUS result = HPDF_Page_ShowTextNextLine(page, text);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclPage_ShowTextNextLineEx(lua_State *L)

  /* HPDF_STATUS <- hpdf.Page_ShowTextNextLineEx(page, word_space,
     char_space, text) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_REAL word_space = LclCheckReal(L, 2);
  HPDF_REAL char_space = LclCheckReal(L, 3);
  const char * text = luaL_checkstring(L, 4);
  HPDF_STATUS result = HPDF_Page_ShowTextNextLineEx(page, word_space,
    char_space, text);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclPage_SetGrayFill(lua_State *L)

  /* HPDF_STATUS <- hpdf.Page_SetGrayFill(page, gray) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_REAL gray = LclCheckReal(L, 2);
  HPDF_STATUS result = HPDF_Page_SetGrayFill(page, gray);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclPage_SetGrayStroke(lua_State *L)

  /* HPDF_STATUS <- hpdf.Page_SetGrayStroke(page, gray) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_REAL gray = LclCheckReal(L, 2);
  HPDF_STATUS result = HPDF_Page_SetGrayStroke(page, gray);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclPage_SetRGBFill(lua_State *L)

  /* HPDF_STATUS <- hpdf.Page_SetRGBFill(page, r, g, b) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_REAL r = LclCheckReal(L, 2);
  HPDF_REAL g = LclCheckReal(L, 3);
  HPDF_REAL b = LclCheckReal(L, 4);
  HPDF_STATUS result = HPDF_Page_SetRGBFill(page, r, g, b);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclPage_SetRGBStroke(lua_State *L)

  /* HPDF_STATUS <- hpdf.Page_SetRGBStroke(page, r, g, b) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_REAL r = LclCheckReal(L, 2);
  HPDF_REAL g = LclCheckReal(L, 3);
  HPDF_REAL b = LclCheckReal(L, 4);
  HPDF_STATUS result = HPDF_Page_SetRGBStroke(page, r, g, b);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclPage_SetCMYKFill(lua_State *L)

  /* HPDF_STATUS <- hpdf.Page_SetCMYKFill(page, c, m, y, k) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_REAL c = LclCheckReal(L, 2);
  HPDF_REAL m = LclCheckReal(L, 3);
  HPDF_REAL y = LclCheckReal(L, 4);
  HPDF_REAL k = LclCheckReal(L, 5);
  HPDF_STATUS result = HPDF_Page_SetCMYKFill(page, c, m, y, k);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclPage_SetCMYKStroke(lua_State *L)

  /* HPDF_STATUS <- hpdf.Page_SetCMYKStroke(page, c, m, y, k) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_REAL c = LclCheckReal(L, 2);
  HPDF_REAL m = LclCheckReal(L, 3);
  HPDF_REAL y = LclCheckReal(L, 4);
  HPDF_REAL k = LclCheckReal(L, 5);
  HPDF_STATUS result = HPDF_Page_SetCMYKStroke(page, c, m, y, k);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclPage_ExecuteXObject(lua_State *L)

  /* HPDF_STATUS <- hpdf.Page_ExecuteXObject(page, obj) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_XObject obj = LclHandleGet(L, CnHndXObject, 2);
  HPDF_STATUS result = HPDF_Page_ExecuteXObject(page, obj);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclPage_DrawImage(lua_State *L)

  /* HPDF_STATUS <- hpdf.Page_DrawImage(page, image, x, y, width, height) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_Image image = LclHandleGet(L, CnHndImage, 2);
  HPDF_REAL x = LclCheckReal(L, 3);
  HPDF_REAL y = LclCheckReal(L, 4);
  HPDF_REAL width = LclCheckReal(L, 5);
  HPDF_REAL height = LclCheckReal(L, 6);
  HPDF_STATUS result = HPDF_Page_DrawImage(page, image, x, y, width, height);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclPage_Circle(lua_State *L)

  /* HPDF_STATUS <- hpdf.Page_Circle(page, x, y, ray) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_REAL x = LclCheckReal(L, 2);
  HPDF_REAL y = LclCheckReal(L, 3);
  HPDF_REAL ray = LclCheckReal(L, 4);
  HPDF_STATUS result = HPDF_Page_Circle(page, x, y, ray);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclPage_Ellipse(lua_State *L)

  /* HPDF_STATUS <- hpdf.Page_Ellipse(page, x, y, xray, yray) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_REAL x = LclCheckReal(L, 2);
  HPDF_REAL y = LclCheckReal(L, 3);
  HPDF_REAL xray = LclCheckReal(L, 4);
  HPDF_REAL yray = LclCheckReal(L, 5);
  HPDF_STATUS result = HPDF_Page_Ellipse(page, x, y, xray, yray);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclPage_Arc(lua_State *L)

  /* HPDF_STATUS <- hpdf.Page_Arc(page, x, y, ray, ang1, ang2) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_REAL x = LclCheckReal(L, 2);
  HPDF_REAL y = LclCheckReal(L, 3);
  HPDF_REAL ray = LclCheckReal(L, 4);
  HPDF_REAL ang1 = LclCheckReal(L, 5);
  HPDF_REAL ang2 = LclCheckReal(L, 6);
  HPDF_STATUS result = HPDF_Page_Arc(page, x, y, ray, ang1, ang2);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclPage_TextOut(lua_State *L)

  /* HPDF_STATUS <- hpdf.Page_TextOut(page, xpos, ypos, text) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_REAL xpos = LclCheckReal(L, 2);
  HPDF_REAL ypos = LclCheckReal(L, 3);
  const char * text = luaL_checkstring(L, 4);
  HPDF_STATUS result = HPDF_Page_TextOut(page, xpos, ypos, text);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static int LclPage_TextRect(lua_State *L)

  /* HPDF_STATUS <- hpdf.Page_TextRect(page, left, top,
     right, bottom, text, align) */

{
  HPDF_UINT len;
  HPDF_STATUS result;
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_REAL left = LclCheckReal(L, 2);
  HPDF_REAL top = LclCheckReal(L, 3);
  HPDF_REAL right = LclCheckReal(L, 4);
  HPDF_REAL bottom = LclCheckReal(L, 5);
  const char * text = luaL_checkstring(L, 6);
  HPDF_TextAlignment align = luaL_checkoption(L, 7,
    (char *) 0, CnTextAlignment);
  len = 0;
  result = HPDF_Page_TextRect(page, left, top, right, bottom, text,
    align, &len);
  lua_pushinteger(L, result);
  lua_pushinteger(L, len);
  return 2;
}

/* * */

static int LclPage_SetSlideShow(lua_State *L)

  /* HPDF_STATUS <- hpdf.Page_SetSlideShow(page, type, disp_time, trans_time) */

{
  HPDF_Page page = LclHandleGet(L, CnHndPage, 1);
  HPDF_TransitionStyle type = luaL_checkoption(L, 2,
    (char *) 0, CnTransitionStyle);
  HPDF_REAL disp_time = LclCheckReal(L, 3);
  HPDF_REAL trans_time = LclCheckReal(L, 4);
  HPDF_STATUS result = HPDF_Page_SetSlideShow(page, type,
    disp_time, trans_time);
  lua_pushinteger(L, result);
  return 1;
}

/* * */

static void LclConstRegister(
  lua_State *L)

{
  typedef struct {
    const char *Str;
    lua_Integer Val;
  } KeyIntType, * KeyIntPtrType;

  static KeyIntType IntList[] = {
    {"OK", HPDF_OK},
    {"NOERROR", HPDF_NOERROR},
    /* buffer size which is required when we convert to character string. */
    {"TMP_BUF_SIZ", HPDF_TMP_BUF_SIZ},
    {"SHORT_BUF_SIZ", HPDF_SHORT_BUF_SIZ},
    {"REAL_LEN", HPDF_REAL_LEN},
    {"INT_LEN", HPDF_INT_LEN},
    {"TEXT_DEFAULT_LEN", HPDF_TEXT_DEFAULT_LEN},
    {"UNICODE_HEADER_LEN", HPDF_UNICODE_HEADER_LEN},
    {"DATE_TIME_STR_LEN", HPDF_DATE_TIME_STR_LEN},
    /* length of each item defined in PDF */
    {"BYTE_OFFSET_LEN", HPDF_BYTE_OFFSET_LEN},
    {"OBJ_ID_LEN", HPDF_OBJ_ID_LEN},
    {"GEN_NO_LEN", HPDF_GEN_NO_LEN},
    /* default value of Graphic State */
    {"DEF_PAGE_LAYOUT", HPDF_PAGE_LAYOUT_SINGLE},
    {"DEF_PAGE_MODE", HPDF_PAGE_MODE_USE_NONE},
    {"DEF_WORDSPACE", HPDF_DEF_WORDSPACE},
    {"DEF_CHARSPACE", HPDF_DEF_CHARSPACE},
    {"DEF_FONTSIZE", HPDF_DEF_FONTSIZE},
    {"DEF_HSCALING", HPDF_DEF_HSCALING},
    {"DEF_LEADING", HPDF_DEF_LEADING},
    {"DEF_RENDERING_MODE", HPDF_FILL},
    {"DEF_RISE", HPDF_DEF_RISE},
    {"DEF_RAISE", HPDF_DEF_RISE},
    {"DEF_LINEWIDTH", HPDF_DEF_LINEWIDTH},
    {"DEF_LINECAP", HPDF_BUTT_END},
    {"DEF_LINEJOIN", HPDF_MITER_JOIN},
    {"DEF_MITERLIMIT", HPDF_DEF_MITERLIMIT},
    {"DEF_FLATNESS", HPDF_DEF_FLATNESS},
    {"DEF_PAGE_NUM", HPDF_DEF_PAGE_NUM},
    {"BS_DEF_WIDTH", HPDF_BS_DEF_WIDTH},
    /* compression mode */
    {"COMP_NONE", HPDF_COMP_NONE},
    {"COMP_TEXT", HPDF_COMP_TEXT},
    {"COMP_IMAGE", HPDF_COMP_IMAGE},
    {"COMP_METADATA", HPDF_COMP_METADATA},
    {"COMP_ALL", HPDF_COMP_ALL},
    {"COMP_MASK", HPDF_COMP_MASK},
    /* permission flags (only Revision 2 is supported) */
    {"ENABLE_READ", HPDF_ENABLE_READ},
    {"ENABLE_PRINT", HPDF_ENABLE_PRINT},
    {"ENABLE_EDIT_ALL", HPDF_ENABLE_EDIT_ALL},
    {"ENABLE_COPY", HPDF_ENABLE_COPY},
    {"ENABLE_EDIT", HPDF_ENABLE_EDIT},
    /* viewer preferences definitions */
    {"HIDE_TOOLBAR", HPDF_HIDE_TOOLBAR},
    {"HIDE_MENUBAR", HPDF_HIDE_MENUBAR},
    {"HIDE_WINDOW_UI", HPDF_HIDE_WINDOW_UI},
    {"FIT_WINDOW", HPDF_FIT_WINDOW},
    {"CENTER_WINDOW", HPDF_CENTER_WINDOW},
    /* limitation of object implementation (PDF1.4) */
    {"LIMIT_MAX_INT", HPDF_LIMIT_MAX_INT},
    {"LIMIT_MIN_INT", HPDF_LIMIT_MIN_INT},
    {"LIMIT_MAX_STRING_LEN", HPDF_LIMIT_MAX_STRING_LEN},
    {"LIMIT_MAX_NAME_LEN", HPDF_LIMIT_MAX_NAME_LEN},
    {"LIMIT_MAX_ARRAY", HPDF_LIMIT_MAX_ARRAY},
    {"LIMIT_MAX_DICT_ELEMENT", HPDF_LIMIT_MAX_DICT_ELEMENT},
    {"LIMIT_MAX_XREF_ELEMENT", HPDF_LIMIT_MAX_XREF_ELEMENT},
    {"LIMIT_MAX_GSTATE", HPDF_LIMIT_MAX_GSTATE},
    {"LIMIT_MAX_DEVICE_N", HPDF_LIMIT_MAX_DEVICE_N},
    {"LIMIT_MAX_DEVICE_N_V15", HPDF_LIMIT_MAX_DEVICE_N_V15},
    {"LIMIT_MAX_CID", HPDF_LIMIT_MAX_CID},
    {"MAX_GENERATION_NUM", HPDF_MAX_GENERATION_NUM},
    {"MIN_PAGE_HEIGHT", HPDF_MIN_PAGE_HEIGHT},
    {"MIN_PAGE_WIDTH", HPDF_MIN_PAGE_WIDTH},
    {"MAX_PAGE_HEIGHT", HPDF_MAX_PAGE_HEIGHT},
    {"MAX_PAGE_WIDTH", HPDF_MAX_PAGE_WIDTH},
    {"MIN_MAGNIFICATION_FACTOR", HPDF_MIN_MAGNIFICATION_FACTOR},
    {"MAX_MAGNIFICATION_FACTOR", HPDF_MAX_MAGNIFICATION_FACTOR},
    /* limitation of various properties */
    {"MIN_PAGE_SIZE", HPDF_MIN_PAGE_SIZE},
    {"MAX_PAGE_SIZE", HPDF_MAX_PAGE_SIZE},
    {"MIN_HORIZONTALSCALING", HPDF_MIN_HORIZONTALSCALING},
    {"MAX_HORIZONTALSCALING", HPDF_MAX_HORIZONTALSCALING},
    {"MIN_WORDSPACE", HPDF_MIN_WORDSPACE},
    {"MAX_WORDSPACE", HPDF_MAX_WORDSPACE},
    {"MIN_CHARSPACE", HPDF_MIN_CHARSPACE},
    {"MAX_CHARSPACE", HPDF_MAX_CHARSPACE},
    {"MAX_FONTSIZE", HPDF_MAX_FONTSIZE},
    {"MAX_ZOOMSIZE", HPDF_MAX_ZOOMSIZE},
    {"MAX_LEADING", HPDF_MAX_LEADING},
    {"MAX_LINEWIDTH", HPDF_MAX_LINEWIDTH},
    {"MAX_DASH_PATTERN", HPDF_MAX_DASH_PATTERN},
    {"MAX_JWW_NUM", HPDF_MAX_JWW_NUM},
    /* Graphics mode */
    {"GMODE_PAGE_DESCRIPTION", HPDF_GMODE_PAGE_DESCRIPTION},
    {"GMODE_PATH_OBJECT", HPDF_GMODE_PATH_OBJECT},
    {"GMODE_TEXT_OBJECT", HPDF_GMODE_TEXT_OBJECT},
    {"GMODE_CLIPPING_PATH", HPDF_GMODE_CLIPPING_PATH},
    {"GMODE_SHADING", HPDF_GMODE_SHADING},
    {"GMODE_INLINE_IMAGE", HPDF_GMODE_INLINE_IMAGE},
    {"GMODE_EXTERNAL_OBJECT", HPDF_GMODE_EXTERNAL_OBJECT},
    {(char *) 0, 0}
  };

  KeyIntPtrType KeyIntPtr;

  for (KeyIntPtr = IntList; KeyIntPtr->Str; KeyIntPtr++) {
    /* Stk: ... Namespace */
    lua_pushinteger(L, KeyIntPtr->Val);
    /* Stk: ... Namespace Val */
    lua_setfield(L, -2, KeyIntPtr->Str);
    /* Stk: ... Namespace */
  }

  lua_pushnumber(L, HPDF_LIMIT_MIN_REAL);
  lua_setfield(L, -2, "HPDF_LIMIT_MIN_REAL");

  lua_pushnumber(L, HPDF_LIMIT_MAX_REAL);
  lua_setfield(L, -2, "HPDF_LIMIT_MAX_REAL");

  lua_pushnumber(L, HPDF_DEF_PAGE_WIDTH);
  lua_setfield(L, -2, "DEF_PAGE_WIDTH");

  lua_pushnumber(L, HPDF_DEF_PAGE_HEIGHT);
  lua_setfield(L, -2, "DEF_PAGE_HEIGHT");

  lua_pushstring(L, HPDF_DEF_FONT);
  lua_setfield(L, -2, "DEF_FONT");

  lua_pushstring(L, HPDF_VERSION_TEXT);
  lua_setfield(L, -2, "VERSION_TEXT");

  lua_pushboolean(L, 1);
  lua_setfield(L, -2, "TRUE");

  lua_pushboolean(L, 0);
  lua_setfield(L, -2, "FALSE");

  /* Stk: ... Namespace */

}

/* * */

int luaopen_hpdf(
  lua_State *L)

{

  static const luaL_Reg HpdfMap[] = {
    {"GetHandleTable", LclGetHandleTable},
    {"GetVersion", LclGetVersion},
#if CnUnsupported
    {"NewEx", LclNewEx},
#endif
    {"New", LclNew},
#if CnUnsupported
    {"SetErrorHandler", LclSetErrorHandler},
#endif
    {"Free", LclFree},
    {"NewDoc", LclNewDoc},
    {"FreeDoc", LclFreeDoc},
    {"HasDoc", LclHasDoc},
    {"FreeDocAll", LclFreeDocAll},
    {"SaveToStream", LclSaveToStream},
    {"GetStreamSize", LclGetStreamSize},
#if CnUnsupported
    {"ReadFromStream", LclReadFromStream},
#endif
    {"ResetStream", LclResetStream},
    {"SaveToFile", LclSaveToFile},
    {"GetError", LclGetError},
    {"GetErrorDetail", LclGetErrorDetail},
    {"ResetError", LclResetError},
    {"SetPagesConfiguration", LclSetPagesConfiguration},
    {"GetPageByIndex", LclGetPageByIndex},
    {"GetPageLayout", LclGetPageLayout},
    {"SetPageLayout", LclSetPageLayout},
    {"GetPageMode", LclGetPageMode},
    {"SetPageMode", LclSetPageMode},
    {"GetViewerPreference", LclGetViewerPreference},
    {"SetViewerPreference", LclSetViewerPreference},
    {"SetOpenAction", LclSetOpenAction},
    {"GetCurrentPage", LclGetCurrentPage},
    {"AddPage", LclAddPage},
    {"InsertPage", LclInsertPage},
    {"Page_SetWidth", LclPage_SetWidth},
    {"Page_SetHeight", LclPage_SetHeight},
    {"Page_SetSize", LclPage_SetSize},
    {"Page_SetRotate", LclPage_SetRotate},
    {"GetFont", LclGetFont},
    {"LoadType1FontFromFile", LclLoadType1FontFromFile},
    {"LoadTTFontFromFile", LclLoadTTFontFromFile},
    {"LoadTTFontFromFile2", LclLoadTTFontFromFile2},
    {"AddPageLabel", LclAddPageLabel},
    {"UseJPFonts", LclUseJPFonts},
    {"UseKRFonts", LclUseKRFonts},
    {"UseCNSFonts", LclUseCNSFonts},
    {"UseCNTFonts", LclUseCNTFonts},
    {"CreateOutline", LclCreateOutline},
    {"Outline_SetOpened", LclOutline_SetOpened},
    {"Outline_SetDestination", LclOutline_SetDestination},
    {"Page_CreateDestination", LclPage_CreateDestination},
    {"Destination_SetXYZ", LclDestination_SetXYZ},
    {"Destination_SetFit", LclDestination_SetFit},
    {"Destination_SetFitH", LclDestination_SetFitH},
    {"Destination_SetFitV", LclDestination_SetFitV},
    {"Destination_SetFitR", LclDestination_SetFitR},
    {"Destination_SetFitB", LclDestination_SetFitB},
    {"Destination_SetFitBH", LclDestination_SetFitBH},
    {"Destination_SetFitBV", LclDestination_SetFitBV},
    {"GetEncoder", LclGetEncoder},
    {"GetCurrentEncoder", LclGetCurrentEncoder},
    {"SetCurrentEncoder", LclSetCurrentEncoder},
    {"Encoder_GetType", LclEncoder_GetType},
    {"Encoder_GetByteType", LclEncoder_GetByteType},
    {"Encoder_GetUnicode", LclEncoder_GetUnicode},
    {"Encoder_GetWritingMode", LclEncoder_GetWritingMode},
    {"UseJPEncodings", LclUseJPEncodings},
    {"UseKREncodings", LclUseKREncodings},
    {"UseCNSEncodings", LclUseCNSEncodings},
    {"UseCNTEncodings", LclUseCNTEncodings},
    {"UseUTFEncodings", LclUseUTFEncodings},
    {"Page_CreateTextAnnot", LclPage_CreateTextAnnot},
    {"Page_CreateLinkAnnot", LclPage_CreateLinkAnnot},
    {"Page_CreateURILinkAnnot", LclPage_CreateURILinkAnnot},
    {"LinkAnnot_SetHighlightMode", LclLinkAnnot_SetHighlightMode},
    {"LinkAnnot_SetBorderStyle", LclLinkAnnot_SetBorderStyle},
    {"TextAnnot_SetIcon", LclTextAnnot_SetIcon},
    {"TextAnnot_SetOpened", LclTextAnnot_SetOpened},
    {"LoadPngImageFromFile", LclLoadPngImageFromFile},
    {"LoadPngImageFromFile2", LclLoadPngImageFromFile2},
    {"LoadJpegImageFromFile", LclLoadJpegImageFromFile},
    {"LoadRawImageFromFile", LclLoadRawImageFromFile},
    {"LoadRawImageFromMem", LclLoadRawImageFromMem},
    {"Image_GetSize", LclImage_GetSize},
    {"Image_GetSize2", LclImage_GetSize2},
    {"Image_GetWidth", LclImage_GetWidth},
    {"Image_GetHeight", LclImage_GetHeight},
    {"Image_GetBitsPerComponent", LclImage_GetBitsPerComponent},
    {"Image_GetColorSpace", LclImage_GetColorSpace},
    {"Image_SetColorMask", LclImage_SetColorMask},
    {"Image_SetMaskImage", LclImage_SetMaskImage},
    {"SetInfoAttr", LclSetInfoAttr},
    {"GetInfoAttr", LclGetInfoAttr},
    {"SetInfoDateAttr", LclSetInfoDateAttr},
    {"SetPassword", LclSetPassword},
    {"SetPermission", LclSetPermission},
    {"SetEncryptionMode", LclSetEncryptionMode},
    {"SetCompressionMode", LclSetCompressionMode},
    {"Font_GetFontName", LclFont_GetFontName},
    {"Font_GetEncodingName", LclFont_GetEncodingName},
    {"Font_GetUnicodeWidth", LclFont_GetUnicodeWidth},
    {"Font_GetBBox", LclFont_GetBBox},
    {"Font_GetAscent", LclFont_GetAscent},
    {"Font_GetDescent", LclFont_GetDescent},
    {"Font_GetXHeight", LclFont_GetXHeight},
    {"Font_GetCapHeight", LclFont_GetCapHeight},
    {"Font_TextWidth", LclFont_TextWidth},
    {"Font_MeasureText", LclFont_MeasureText},
    {"CreateExtGState", LclCreateExtGState},
    {"ExtGState_SetAlphaStroke", LclExtGState_SetAlphaStroke},
    {"ExtGState_SetAlphaFill", LclExtGState_SetAlphaFill},
    {"ExtGState_SetBlendMode", LclExtGState_SetBlendMode},
    {"Page_TextWidth", LclPage_TextWidth},
    {"Page_MeasureText", LclPage_MeasureText},
    {"Page_GetWidth", LclPage_GetWidth},
    {"Page_GetHeight", LclPage_GetHeight},
    {"Page_GetGMode", LclPage_GetGMode},
    {"Page_GetCurrentPos", LclPage_GetCurrentPos},
    {"Page_GetCurrentPos2", LclPage_GetCurrentPos2},
    {"Page_GetCurrentTextPos", LclPage_GetCurrentTextPos},
    {"Page_GetCurrentTextPos2", LclPage_GetCurrentTextPos2},
    {"Page_GetCurrentFont", LclPage_GetCurrentFont},
    {"Page_GetCurrentFontSize", LclPage_GetCurrentFontSize},
    {"Page_GetTransMatrix", LclPage_GetTransMatrix},
    {"Page_GetLineWidth", LclPage_GetLineWidth},
    {"Page_GetLineCap", LclPage_GetLineCap},
    {"Page_GetLineJoin", LclPage_GetLineJoin},
    {"Page_GetMiterLimit", LclPage_GetMiterLimit},
    {"Page_GetDash", LclPage_GetDash},
    {"Page_GetFlat", LclPage_GetFlat},
    {"Page_GetCharSpace", LclPage_GetCharSpace},
    {"Page_GetWordSpace", LclPage_GetWordSpace},
    {"Page_GetHorizontalScalling", LclPage_GetHorizontalScalling},
    {"Page_GetTextLeading", LclPage_GetTextLeading},
    {"Page_GetTextRenderingMode", LclPage_GetTextRenderingMode},
    {"Page_GetTextRaise", LclPage_GetTextRaise},
    {"Page_GetTextRise", LclPage_GetTextRise},
    {"Page_GetRGBFill", LclPage_GetRGBFill},
    {"Page_GetRGBStroke", LclPage_GetRGBStroke},
    {"Page_GetCMYKFill", LclPage_GetCMYKFill},
    {"Page_GetCMYKStroke", LclPage_GetCMYKStroke},
    {"Page_GetGrayFill", LclPage_GetGrayFill},
    {"Page_GetGrayStroke", LclPage_GetGrayStroke},
    {"Page_GetStrokingColorSpace", LclPage_GetStrokingColorSpace},
    {"Page_GetFillingColorSpace", LclPage_GetFillingColorSpace},
    {"Page_GetTextMatrix", LclPage_GetTextMatrix},
    {"Page_GetGStateDepth", LclPage_GetGStateDepth},
    {"Page_SetLineWidth", LclPage_SetLineWidth},
    {"Page_SetLineCap", LclPage_SetLineCap},
    {"Page_SetLineJoin", LclPage_SetLineJoin},
    {"Page_SetMiterLimit", LclPage_SetMiterLimit},
    {"Page_SetDash", LclPage_SetDash},
    {"Page_SetFlat", LclPage_SetFlat},
    {"Page_SetExtGState", LclPage_SetExtGState},
    {"Page_GSave", LclPage_GSave},
    {"Page_GRestore", LclPage_GRestore},
    {"Page_Concat", LclPage_Concat},
    {"Page_MoveTo", LclPage_MoveTo},
    {"Page_LineTo", LclPage_LineTo},
    {"Page_CurveTo", LclPage_CurveTo},
    {"Page_CurveTo2", LclPage_CurveTo2},
    {"Page_CurveTo3", LclPage_CurveTo3},
    {"Page_ClosePath", LclPage_ClosePath},
    {"Page_Rectangle", LclPage_Rectangle},
    {"Page_Stroke", LclPage_Stroke},
    {"Page_ClosePathStroke", LclPage_ClosePathStroke},
    {"Page_Fill", LclPage_Fill},
    {"Page_Eofill", LclPage_Eofill},
    {"Page_FillStroke", LclPage_FillStroke},
    {"Page_EofillStroke", LclPage_EofillStroke},
    {"Page_ClosePathFillStroke", LclPage_ClosePathFillStroke},
    {"Page_ClosePathEofillStroke", LclPage_ClosePathEofillStroke},
    {"Page_EndPath", LclPage_EndPath},
    {"Page_Clip", LclPage_Clip},
    {"Page_Eoclip", LclPage_Eoclip},
    {"Page_BeginText", LclPage_BeginText},
    {"Page_EndText", LclPage_EndText},
    {"Page_SetCharSpace", LclPage_SetCharSpace},
    {"Page_SetWordSpace", LclPage_SetWordSpace},
    {"Page_SetHorizontalScalling", LclPage_SetHorizontalScalling},
    {"Page_SetTextLeading", LclPage_SetTextLeading},
    {"Page_SetFontAndSize", LclPage_SetFontAndSize},
    {"Page_SetTextRenderingMode", LclPage_SetTextRenderingMode},
    {"Page_SetTextRise", LclPage_SetTextRise},
    {"Page_SetTextRaise", LclPage_SetTextRaise},
    {"Page_MoveTextPos", LclPage_MoveTextPos},
    {"Page_MoveTextPos2", LclPage_MoveTextPos2},
    {"Page_SetTextMatrix", LclPage_SetTextMatrix},
    {"Page_MoveToNextLine", LclPage_MoveToNextLine},
    {"Page_ShowText", LclPage_ShowText},
    {"Page_ShowTextNextLine", LclPage_ShowTextNextLine},
    {"Page_ShowTextNextLineEx", LclPage_ShowTextNextLineEx},
    {"Page_SetGrayFill", LclPage_SetGrayFill},
    {"Page_SetGrayStroke", LclPage_SetGrayStroke},
    {"Page_SetRGBFill", LclPage_SetRGBFill},
    {"Page_SetRGBStroke", LclPage_SetRGBStroke},
    {"Page_SetCMYKFill", LclPage_SetCMYKFill},
    {"Page_SetCMYKStroke", LclPage_SetCMYKStroke},
    {"Page_ExecuteXObject", LclPage_ExecuteXObject},
    {"Page_DrawImage", LclPage_DrawImage},
    {"Page_Circle", LclPage_Circle},
    {"Page_Ellipse", LclPage_Ellipse},
    {"Page_Arc", LclPage_Arc},
    {"Page_TextOut", LclPage_TextOut},
    {"Page_TextRect", LclPage_TextRect},
    {"Page_SetSlideShow", LclPage_SetSlideShow},
    {NULL, NULL}
  }; /* HpdfMap */

  /* Stk: ModuleStr */
  lua_newtable(L);
  /* Stk: ModuleStr ModuleTbl */
  lua_newtable(L);
  /* Stk: ModuleStr ModuleTbl HndTbl */
  luaL_setfuncs(L, HpdfMap, 1);
  /* Stk: ModuleStr ModuleTbl */
  LclConstRegister(L);
  /* Stk: ModuleStr ModuleTbl */
  return 1;
}
